use std::{error::Error, io::{self, Write}, sync::mpsc::channel, thread::sleep, time::Duration};

use cxx_juce::{
    JUCE,
    juce_audio_devices::{AudioDeviceManager, AudioIODevice, AudioIODeviceCallback},
};
use cxx_juce::juce_audio_devices::{InputAudioSampleBuffer, OutputAudioSampleBuffer};

use mydsp_rust::echo::Echo;
use mydsp_rust::AudioComponent;

// ---------------------------------------------------------------------------
// Audio config (copied from first file)
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
            input_channels: 2,   // echo needs input
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

let setup = adm.audio_device_setup()
    .with_output_device_name("Speakers (Realtek High Definition Audio)"); // or whatever your real device is
adm.set_audio_device_setup(&setup);

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
// Generic backend
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
// Entry point
// ---------------------------------------------------------------------------

fn main() -> Result<(), Box<dyn Error>> {
    let config = AudioConfig {
        duration: Some(Duration::from_secs(5)),
        ..AudioConfig::default()
    };

    let mut feedback = 0.2_f32;
    let mut delay: usize = 100_000;

    let mut echo = Echo::new(delay, feedback);
    let max_delay = echo.get_max_delay();

    let (tx_feedback, rx_feedback) = channel::<f32>();
    let (tx_delay, rx_delay) = channel::<usize>();

    let callback: AudioCallback = Box::new(move |input, output| {
        while let Ok(f) = rx_feedback.try_recv() {
            echo.set_feedback(f);
        }
        while let Ok(d) = rx_delay.try_recv() {
            echo.set_delay(d);
        }

        for n in 0..output.samples() {
            for c in 0..output.channels() {
                let dry = if input.channels() > c { input[c][n] } else { 0.0 };
                output[c][n] = echo.tick(dry);
            }
        }
    });

    let backend = GenericAudioBackend::new(callback);

    // Interactive loop runs in background thread
    std::thread::spawn(move || {
    let result = (|| -> Result<(), Box<dyn std::error::Error>> {
        loop {
            print!("Entrez feedback (f32, actuellement {feedback}) : ");
            io::stdout().flush()?;
            let mut buf = String::new();
            io::stdin().read_line(&mut buf)?;
            match buf.trim().parse::<f32>() {
                Ok(val) if (0.0..=1.0).contains(&val) => feedback = val,
                Ok(_) => println!("Feedback must be between 0.0 and 1.0"),
                Err(_) => println!("Valeur invalide"),
            }
            if tx_feedback.send(feedback).is_err() { break; }

            print!("Entrez delay (usize lower than {max_delay}, actuellement {delay}): ");
            io::stdout().flush()?;
            let mut buf = String::new();
            io::stdin().read_line(&mut buf)?;
            match buf.trim().parse::<usize>() {
                Ok(val) => delay = val,
                Err(_) => println!("Valeur invalide"),
            }
            if tx_delay.send(delay).is_err() { break; }

            println!("feedback: {feedback}, delay = {delay}");
        }
        Ok(())
    })();

    if let Err(e) = result {
        eprintln!("Interactive thread error: {e}");
    }
});
    // JUCE stays on the main thread
    run_audio(backend, &config)
}