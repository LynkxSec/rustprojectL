use std::{error::Error, thread::sleep, time::Duration};

use cxx_juce::{
    JUCE,
    juce_audio_devices::{AudioDeviceManager, AudioIODevice, AudioIODeviceCallback},
};
use cxx_juce::juce_audio_devices::{InputAudioSampleBuffer, OutputAudioSampleBuffer};

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
            input_channels: 2_usize,
            output_channels: 2_usize,
            sample_rate: 48_000.0,
            duration: None, // run forever
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
// DSP: passthrough (echo)
// ---------------------------------------------------------------------------

struct Passthrough {
    bypass: bool,
}

impl Passthrough {
    fn new(bypass: bool) -> Self {
        Self { bypass }
    }
}

impl AudioIODeviceCallback for Passthrough {
    fn about_to_start(&mut self, _device: &mut dyn AudioIODevice) {}

    fn process_block(
        &mut self,
        input: &InputAudioSampleBuffer<'_>,
        output: &mut OutputAudioSampleBuffer<'_>,
    ) {
        if self.bypass {
            for c in 0..output.channels() {
                for n in 0..output.samples() {
                    output[c][n] = 0.0;
                }
            }
            return;
        }

        for c in 0..output.channels().min(input.channels()) {
            for n in 0..output.samples() {
                output[c][n] = input[c][n];
            }
        }
    }

    fn stopped(&mut self) {}
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

fn main() -> Result<(), Box<dyn Error>> {
    let config = AudioConfig::default();
    run_audio(Passthrough::new(false), &config)
}

#[test]
fn test() {}