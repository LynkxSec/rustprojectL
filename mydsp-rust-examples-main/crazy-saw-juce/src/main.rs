use std::{
    sync::mpsc::channel,
    thread::sleep,
    time::Duration,
};

use audio_backend::{
    AudioCallback,
    AudioConfig,
    GenericAudioBackend,
    run_audio,
};

use mydsp_rust::phasor::Phasor;
use mydsp_rust::echo::Echo;
use mydsp_rust::AudioComponent;

use rand::Rng;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let config = AudioConfig::default();

    let (tx, rx) = channel::<f32>();

    let mut saw = Phasor::new(None, config.sample_rate);
    saw.set_freq(440.0);

    let mut echo = Echo::new(10000, 0.2);
    let bypass = false;

    let callback: AudioCallback = Box::new(
        move |_input, output| {
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

    std::thread::spawn(move || {
        let mut rng = rand::rng();
        let range = 200.0_f32..2000.0_f32;

        loop {
            let freq = rng.random_range(range.clone());
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
