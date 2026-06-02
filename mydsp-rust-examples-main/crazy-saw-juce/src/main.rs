use std::{error::Error, sync::mpsc::channel, thread::sleep, time::Duration};

use cxx_juce::{
    JUCE,
    juce_audio_devices::{AudioDeviceManager, AudioIODevice, AudioIODeviceCallback},
};
use cxx_juce::juce_audio_devices::{InputAudioSampleBuffer, OutputAudioSampleBuffer};

use mydsp_rust::phasor::Phasor;
use mydsp_rust::echo::Echo;
use mydsp_rust::AudioComponent;

use rand::Rng;

// ---------------------------------------------------------------------------
// Audio config
// ---------------------------------------------------------------------------

pub struct AudioConfig {
    pub input_channels: usize,
    pub output_channels: usize,
    pub sample_rate: f32,
    pub duration: Option<Duration>,
}

impl Default for AudioConfig {
    fn default() -> Self {
        Self {
            input_channels: 0,
            output_channels: 2,
            sample_rate: 48_000.0,
            duration: None,
        }
    }
}

// ---------------------------------------------------------------------------
// Generic audio runner
// ---------------------------------------------------------------------------

fn run_audio<C: AudioIODeviceCallback + 'static>(
    callback: C,
    config: &AudioConfig,
) -> Result<(), Box<dyn Error>> {
    let juce = JUCE::initialise();
    let mut adm = AudioDeviceManager::new(&juce);
    adm.initialise(config.input_channels, config.output_channels)?;

    let _handle = adm.add_audio_callback(callback);

    match config.duration {
        Some(dur) => sleep(dur),
        None => loop { sleep(Duration::from_millis(100)); },
    }

    Ok(())
}

// ---------------------------------------------------------------------------
// DSP callback type (FnMut + HRTB)
// ---------------------------------------------------------------------------

type AudioCallback = Box<
    dyn for<'a, 'b, 'c, 'd>
        FnMut(&'a InputAudioSampleBuffer<'b>, &'c mut OutputAudioSampleBuffer<'d>)
        + Send
        + 'static,
>;

// ---------------------------------------------------------------------------
// Generic backend (FnMut + HRTB)
// ---------------------------------------------------------------------------

struct GenericAudioBackend<T>
where
    T: for<'a, 'b, 'c, 'd>
        FnMut(&'a InputAudioSampleBuffer<'b>, &'c mut OutputAudioSampleBuffer<'d>)
        + Send
        + 'static,
{
    callback: T,
}

impl<T> GenericAudioBackend<T>
where
    T: for<'a, 'b, 'c, 'd>
        FnMut(&'a InputAudioSampleBuffer<'b>, &'c mut OutputAudioSampleBuffer<'d>)
        + Send
        + 'static,
{
    fn new(callback: T) -> Self {
        Self { callback }
    }
}

impl<T> AudioIODeviceCallback for GenericAudioBackend<T>
where
    T: for<'a, 'b, 'c, 'd>
        FnMut(&'a InputAudioSampleBuffer<'b>, &'c mut OutputAudioSampleBuffer<'d>)
        + Send
        + 'static,
{
    fn about_to_start(&mut self, _device: &mut dyn AudioIODevice) {}

    fn process_block(
        &mut self,
        input: &InputAudioSampleBuffer<'_>,
        output: &mut OutputAudioSampleBuffer<'_>,
    ) {
        (self.callback)(input, output);
    }

    fn stopped(&mut self) {}
}

// ---------------------------------------------------------------------------
// Entry point — DSP lives here now
// ---------------------------------------------------------------------------

fn main() -> Result<(), Box<dyn Error>> {
    let config = AudioConfig {
        duration: Some(Duration::from_secs(5)),
        ..AudioConfig::default()
    };

    let (tx, rx) = channel::<f32>();

    // -----------------------------
    // DSP STATE MOVED INTO MAIN()
    // -----------------------------
    let mut saw = Phasor::new(None, config.sample_rate);
    saw.set_freq(440.0);

    let mut echo = Echo::new(10000, 0.2);
    let bypass = false;

    // -----------------------------
    // PROCESS BLOCK MOVED HERE
    // -----------------------------
    let callback: AudioCallback = Box::new(
        move |_input, output| {
            // Drain frequency updates
            while let Ok(f) = rx.try_recv() {
                saw.set_freq(f);
            }

            for n in 0..output.samples() {
                let sample = if bypass {
                    0.0
                } else {
                    echo.tick(saw.tick(0.0) * 0.5) as f32
                };

                for c in 0..output.channels() {
                    output[c][n] = sample;
                }
            }
        }
    );

    let backend = GenericAudioBackend::new(callback);

    // Randomize frequency in a background thread
    std::thread::spawn(move || {
        let mut rng = rand::thread_rng();
        let range = 200.0_f32..2000.0_f32;

        loop {
            let freq = rng.gen_range(range.clone());
            if tx.send(freq).is_err() {
                break;
            }
            sleep(Duration::from_millis(100));
        }
    });

    run_audio(backend, &config)
}

#[test]
fn test() {}
