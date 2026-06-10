use std::{
    sync::{mpsc::channel, LazyLock},
    thread::sleep,
    time::Duration,
};

use audio_backend::{
    AudioCallback,
    AudioConfig,
    GenericAudioBackend,
    run_audio,
};

use mydsp_rust::AudioComponent as _;
use mydsp_rust::echo::Echo;
use mydsp_rust::sine::SineWave;
use mydsp_rust::sine_table::SineTable;

use rand::Rng;

static TABLE: LazyLock<SineTable> =
    LazyLock::new(|| SineTable::new(16384));

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let config = AudioConfig::default();

    let (tx, rx) = channel::<f32>();

    let mut sine = SineWave::new(&TABLE, config.sample_rate);
    sine.set_freq(440.0);

    let mut echo = Echo::new(10000, 0.5);

    let callback: AudioCallback = Box::new(move |_input, output| {
        while let Ok(freq) = rx.try_recv() {
            sine.set_freq(freq);
        }

        for n in 0..output.samples() {
            let sample =
                echo.tick(sine.tick(0.0) * 0.5) as f32;

            for c in 0..output.channels() {
                output[c][n] = sample;
            }
        }
    });

    let backend = GenericAudioBackend::new(callback);

    std::thread::spawn(move || {
        let mut rng = rand::thread_rng();

        loop {
            let freq = rng.gen_range(200.0..2000.0);

            if tx.send(freq).is_err() {
                break;
            }

            sleep(Duration::from_millis(100));
        }
    });

    run_audio(backend, &config)
}