use std::{error::Error, sync::mpsc::channel, thread::sleep, time::Duration};

use cxx_juce::{
    JUCE,
    juce_audio_devices::{AudioDeviceManager, AudioIODevice, AudioIODeviceCallback},
};
use cxx_juce::juce_audio_devices::{InputAudioSampleBuffer, OutputAudioSampleBuffer};

use mydsp_rust::AudioComponent;
use mydsp_rust::phasor::Phasor;
use mydsp_rust::echo::Echo;

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
            input_channels: 0_usize,
            output_channels: 2_usize,
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
// DSP: crazy sawtooth with echo
// ---------------------------------------------------------------------------

struct CrazySaw {
    bypass: bool,
    sawtooth: Phasor,
    echo: Echo,
    rx: std::sync::mpsc::Receiver<f32>,
}

impl CrazySaw {
    fn new(
        freq: f32,
        sample_rate: f32,
        bypass: bool,
        rx: std::sync::mpsc::Receiver<f32>,
    ) -> Self {
        let mut sawtooth = Phasor::new(None, sample_rate);
        sawtooth.set_freq(freq);
        let echo = Echo::new(10000, 0.2);
        Self { bypass, sawtooth, echo, rx }
    }
}

impl AudioIODeviceCallback for CrazySaw {
    fn about_to_start(&mut self, _device: &mut dyn AudioIODevice) {}

 fn process_block(
    &mut self,
    _: &InputAudioSampleBuffer<'_>,
    output: &mut OutputAudioSampleBuffer<'_>,
) {
    let bypass = self.bypass;

    let mut next_sample = || -> f32 {
        while let Ok(freq) = self.rx.try_recv() {
            self.sawtooth.set_freq(freq);
        }
        if bypass {
            0.0
        } else {
            self.echo.tick(self.sawtooth.tick(0.0) * 0.5) as f32
        }
    };

    for n in 0..output.samples() {
        let sample = next_sample(); // closure called each sample
        for c in 0..output.channels() {
            output[c][n] = sample;
        }
    }
}

    fn stopped(&mut self) {}
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

fn main() -> Result<(), Box<dyn Error>> {
    let config = AudioConfig {
        duration: Some(Duration::from_secs(5)),
        ..AudioConfig::default()
    };

    let (tx, rx) = channel::<f32>();

    let dsp = CrazySaw::new(440.0, config.sample_rate, false, rx);

    // Spawn a thread to randomise frequency every 100ms, mirroring the
    // original JACK loop { rng.random_range(...); tx.send(...); sleep(...) }
    std::thread::spawn(move || {
        let mut rng = rand::thread_rng();
        let range = 200.0_f32..2000.0_f32;
        loop {
            let freq = rng.gen_range(range.clone());
            if tx.send(freq).is_err() {
                break; // audio thread dropped, exit
            }
            sleep(Duration::from_millis(100));
        }
    });

    run_audio(dsp, &config)
}

#[test]
fn test() {}