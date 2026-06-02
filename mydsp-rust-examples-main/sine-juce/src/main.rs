use std::{error::Error, sync::LazyLock, thread::sleep, time::Duration};

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
            input_channels: 0,
            output_channels: 2,
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
// DSP: sine table
// ---------------------------------------------------------------------------

static TABLE: LazyLock<SineTable> = LazyLock::new(|| SineTable::new(16384));

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
    let config = AudioConfig::default();

    // -----------------------------
    // DSP STATE MOVED INTO MAIN()
    // -----------------------------
    let mut sine = SineWave::new(&TABLE, config.sample_rate);
    sine.set_freq(440.0);

    // -----------------------------
    // PROCESS BLOCK MOVED HERE
    // -----------------------------
    let callback: AudioCallback = Box::new(
        move |_input, output| {
            for n in 0..output.samples() {
                let sample = sine.tick(0.0) * 0.5;

                for c in 0..output.channels() {
                    output[c][n] = sample;
                }
            }
        }
    );

    let backend = GenericAudioBackend::new(callback);

    run_audio(backend, &config)
}

#[test]
fn test() {}
