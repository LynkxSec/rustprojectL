use std::{
    error::Error,
    thread::sleep,
    time::Duration,
};

use cxx_juce::{
    JUCE,
    juce_audio_devices::{
        AudioDeviceManager,
        AudioIODevice,
        AudioIODeviceCallback,
        InputAudioSampleBuffer,
        OutputAudioSampleBuffer,
    },
};

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
            duration: Some(Duration::from_secs(5)),
        }
    }
}

pub type AudioCallback = Box<
    dyn for<'a, 'b, 'c, 'd>
        FnMut(
            &'a InputAudioSampleBuffer<'b>,
            &'c mut OutputAudioSampleBuffer<'d>,
        ) + Send
        + 'static,
>;

pub struct GenericAudioBackend<T>
where
    T: for<'a, 'b, 'c, 'd>
        FnMut(
            &'a InputAudioSampleBuffer<'b>,
            &'c mut OutputAudioSampleBuffer<'d>,
        ) + Send
        + 'static,
{
    callback: T,
}

impl<T> GenericAudioBackend<T>
where
    T: for<'a, 'b, 'c, 'd>
        FnMut(
            &'a InputAudioSampleBuffer<'b>,
            &'c mut OutputAudioSampleBuffer<'d>,
        ) + Send
        + 'static,
{
    pub fn new(callback: T) -> Self {
        Self { callback }
    }
}

impl<T> AudioIODeviceCallback for GenericAudioBackend<T>
where
    T: for<'a, 'b, 'c, 'd>
        FnMut(
            &'a InputAudioSampleBuffer<'b>,
            &'c mut OutputAudioSampleBuffer<'d>,
        ) + Send
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

pub fn run_audio<C>(
    callback: C,
    config: &AudioConfig,
) -> Result<(), Box<dyn Error>>
where
    C: AudioIODeviceCallback + 'static,
{
    let juce = JUCE::initialise();

    let mut adm = AudioDeviceManager::new(&juce);

    adm.initialise(
        config.input_channels,
        config.output_channels,
    )?;

    let _handle = adm.add_audio_callback(callback);

    match config.duration {
        Some(duration) => sleep(duration),
        None => loop {
            sleep(Duration::from_millis(100));
        },
    }

    Ok(())
}