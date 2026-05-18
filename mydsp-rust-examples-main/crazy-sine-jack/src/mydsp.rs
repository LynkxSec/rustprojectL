use mydsp_rust::AudioComponent;
use mydsp_rust::sine::SineWave;
use mydsp_rust::sine_table::SineTable;
use mydsp_rust::echo::Echo;

use std::sync::mpsc::channel;

use rand::Rng;

use std::{sync::LazyLock, thread::sleep, time::Duration};

use jack::{Client, ClientOptions, Control, ProcessScope,
    contrib::ClosureProcessHandler,
};

pub fn mydsp_run() -> () {
    #[cfg(debug_assertions)]
    dbg!("Creating Client");
    let (client, _client_status) = Client::new("crazy-sine-jack", ClientOptions::default())
        .expect("Rust could not open Jack Client ");

    let sr = client.sample_rate() as f32;

    //create 2 output port in the client
    let out_port0 = client
        .register_port("audio_out_0", jack::AudioOut::default())
        .expect("could not register port 0");
    let out_port1 = client
        .register_port("audio_out_1", jack::AudioOut::default())
        .expect("could not register port 1");
    //get the port name assigned by Rust (usually "projet:my_name_given")
    let port_name_0 = &out_port0.name().unwrap().clone();
    let port_name_1 = &out_port1.name().unwrap().clone();
    let mut output_ports = [out_port0, out_port1];

    //mpsc channel for controling Sine in the callback (thread launched)
    let (tx,rx)= channel::<f32>();

    //Launch sine
    // la je comprend pas trop pourquoi on doit encapsuler avec LazyLock ou Lazy
    static TABLE: LazyLock<SineTable> = std::sync::LazyLock::new(|| SineTable::new(16384));
    let mut freq = 440.;
    let mut sine = SineWave::new(&TABLE, sr);
    sine.set_freq(freq);
    //let delay = Delay::new(10000); 
    let mut echo = Echo::new(10000,0.5);
   
    // Define the jack callback:
    let process_callback = move |_: &Client, ps: &ProcessScope| -> Control {
        for f in 0..(ps.n_frames() as usize) {
            while let Ok(freq) = rx.try_recv() {
                sine.set_freq(freq);
            };
            // why tick has an argument?
            let v = echo.tick(sine.tick(0.0) * 0.5) as f32;
            output_ports[0].as_mut_slice(ps)[f] = v;
            output_ports[1].as_mut_slice(ps)[f] = v;
        }
        Control::Continue
    };

    #[cfg(debug_assertions)]
    dbg!("Activate client");
    let proc = ClosureProcessHandler::new(process_callback);
    let active_client = &client
        .activate_async((), proc)
        .expect("Could not activate Jack Client");

    let _out_name0 = active_client.as_client().port_by_name(&port_name_0);
    let _out_name1 = active_client.as_client().port_by_name(&port_name_1);
    #[cfg(debug_assertions)]
    dbg!("ports:", _out_name0, _out_name1);

    //you can obtain the names of the jack port by typing jack_lsp in a shell
    #[cfg(debug_assertions)]
    dbg!("connect ports");
    active_client
        .as_client()
        .connect_ports_by_name(port_name_0, "system:playback_1")
        .expect("could not connect port 0");
    active_client
        .as_client()
        .connect_ports_by_name(port_name_1, "system:playback_2")
        .expect("could not connect port 1");

    let mut rng = rand::rng();
    let sleep_duration = Duration::from_millis(100);
    let range = 200.0..2000.0;
    loop{
        freq = rng.random_range(range.clone());
        tx.send(freq).unwrap();
        sleep(sleep_duration);
    }
}

