use std::{error::Error, sync::Arc, sync::LazyLock, thread::sleep, time::Duration};

use cxx_juce::{
    JUCE,
    juce_audio_devices::{AudioDeviceManager, AudioIODevice, AudioIODeviceCallback},
};
use cxx_juce::juce_audio_devices::{InputAudioSampleBuffer, OutputAudioSampleBuffer};

use mydsp_rust::AudioComponent;
use mydsp_rust::sine::SineWave;
use mydsp_rust::sine_table::SineTable;

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
            duration: Some(Duration::from_secs(2)),
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

    if let Some(dur) = config.duration {
        sleep(dur);
    }

    Ok(())
}

// ---------------------------------------------------------------------------
// DSP: sine tone
// ---------------------------------------------------------------------------

static TABLE: LazyLock<SineTable> = LazyLock::new(|| SineTable::new(16384));

struct SineTone {
    freq: f32,
    my_sine: SineWave,
}

impl AudioIODeviceCallback for SineTone {
    fn about_to_start(&mut self, _device: &mut dyn AudioIODevice) {}

    fn process_block(
        &mut self,
        _: &InputAudioSampleBuffer<'_>,
        o: &mut OutputAudioSampleBuffer<'_>,
    ) {
        for n in 0..o.samples() {
            let sample = self.my_sine.tick(0.0) * 0.5;
            for c in 0..o.channels() {
                o[c][n] = sample;
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

    let freq = 440.0;
    let mut sine = SineWave::new(&TABLE, config.sample_rate);
    sine.set_freq(freq);

    run_audio(SineTone { freq, my_sine: sine }, &config)
}

#[test]
fn test() {}