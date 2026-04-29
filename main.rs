use std::{error::Error, i16, sync::Arc};

use cxx_juce::{juce_audio_devices::{AudioDeviceManager, AudioIODevice, AudioIODeviceCallback}, JUCE};
use hound::WavReader;
use ringbuf::{storage::Heap, traits::{Consumer, Producer, Split}, wrap::caching::Caching, HeapRb, SharedRb};
use tokio::{io::{AsyncReadExt, AsyncWriteExt}, net::{TcpListener, TcpStream}};

use cxx_juce::juce_audio_devices::{InputAudioSampleBuffer, OutputAudioSampleBuffer};

type HrbConsumer = Caching<Arc<SharedRb<Heap<i16>>>, false, true>;

struct Stream {
    rb: HrbConsumer
}

impl AudioIODeviceCallback for Stream {
    fn about_to_start(&mut self, _: &mut dyn AudioIODevice) {}
    fn process_block(
        &mut self,
        _: &InputAudioSampleBuffer<'_>,
        o: &mut OutputAudioSampleBuffer<'_>,
    ) {
        for n in 0..o.samples() {
            for c in 0..o.channels() {
                match self.rb.try_pop() {
                    Some(s) => {
                        o[c][n] = s as f32 / i16::MAX as f32;
                    }
                    None => {
                        o[c][n] = 0.0;
                    }
                }
            }
        }
    }
    fn stopped(&mut self) {}
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let addr = "127.0.0.1:8080";

let mut wav = WavReader::open(
    concat!(env!("CARGO_MANIFEST_DIR"), "/audio/snowmelt.wav")
).expect("Failed to open audio file");

    println!("WAVE format: {:?}", wav.spec());

    let spec = wav.spec();
    let total_samples = wav.len() as usize;
    let sample_rate = spec.sample_rate as u64;
    let channels = spec.channels as u64;
    let duration_secs = total_samples as u64 / (sample_rate * channels);

    println!("Total samples: {}", total_samples);
    println!("Duration: {}s", duration_secs);

    // Ringbuffer large enough to hold the entire file
    let hrb = HeapRb::<i16>::new(total_samples + 4096);
    let (mut jp, jc) = hrb.split();
    let stream = Stream { rb: jc };

    let lsn = TcpListener::bind(addr).await?;

    // Spawn the sender
    let sender = tokio::spawn(async move {
        let mut server_sock = match lsn.accept().await {
            Ok((sock, _)) => sock,
            Err(e) => {
                eprintln!("Accept error: {}", e);
                return;
            }
        };
        for s in wav.samples::<i16>() {
            server_sock.write_i16(s.unwrap()).await.unwrap();
        }
        println!("Sender: finished sending all samples");
    });

    // Connect the client
    let mut client = TcpStream::connect(addr).await?;

    // Read ALL samples into the ringbuffer before starting audio
    println!("Pre-loading buffer...");
    for _ in 0..total_samples {
        match client.read_i16().await {
            Ok(s) => {
                loop {
                    match jp.try_push(s) {
                        Ok(_) => break,
                        Err(_) => tokio::task::yield_now().await,
                    }
                }
            }
            Err(e) => {
                eprintln!("Client read error: {}", e);
                break;
            }
        }
    }
    println!("Buffer fully loaded ({} samples), starting playback...", total_samples);

    // Start audio AFTER buffer is fully loaded
    let juce = JUCE::initialise();
    let mut adm = AudioDeviceManager::new(&juce);
    adm.initialise(0, 2)?;
    let hdl = adm.add_audio_callback(stream);

    // Wait for playback to finish
    tokio::time::sleep(std::time::Duration::from_secs(duration_secs + 1)).await;

    adm.remove_audio_callback(hdl);
    sender.await?;

    println!("Playback complete.");
    Ok(())
}

#[tokio::test]
async fn test() {

}