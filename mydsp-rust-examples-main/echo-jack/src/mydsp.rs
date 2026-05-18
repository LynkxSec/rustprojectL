use mydsp_rust::AudioComponent;
use mydsp_rust::echo::Echo;
use std::io::{self, Write};

use std::sync::mpsc::channel;


use std::{thread::sleep, time::Duration};

use jack::{Client, ClientOptions, Control, ProcessScope,
    contrib::ClosureProcessHandler,
};

pub fn mydsp_run() -> () {
    #[cfg(debug_assertions)]
    dbg!("Creating Client");
    let (client, _client_status) = Client::new("bypass-jack", ClientOptions::default())
        .expect("Rust could not open Jack Client ");

    let _sr = client.sample_rate() as f32;

    //create 2 output port in the client
    let out_port0 = client
        .register_port("audio_out_0", jack::AudioOut::default())
        .expect("could not register port 0");
    let out_port1 = client
        .register_port("audio_out_1", jack::AudioOut::default())
        .expect("could not register port 1");
    //create 2 input port in the client
    let in_port0 = client
        .register_port("audio_in_0", jack::AudioIn::default())
        .expect("could not register in port 0");
    let in_port1 = client
        .register_port("audio_in_1", jack::AudioIn::default())
        .expect("could not register in port 1");

        //get the port name assigned by Rust (usually "projet:my_name_given")
    let out_port_name_0 = &out_port0.name().unwrap().clone();
    let out_port_name_1 = &out_port1.name().unwrap().clone();
    let in_port_name_0 = &in_port0.name().unwrap().clone();
    let in_port_name_1 = &in_port1.name().unwrap().clone();
    let mut output_ports = [out_port0, out_port1];
    let input_ports = [in_port0, in_port1];

    //mpsc channel for controling echo in the callback (thread launched)
    let (tx,rx)= channel::<f32>();
    let (tx2,rx2)= channel::<usize>();

    //Launch sawtooth

    let mut feedback = 0.2;
    let mut delay = 100000;
    //let delay = Delay::new(10000); 
    let mut echo = Echo::new(delay,feedback);
    let max_delay = echo.get_max_delay();
   
    // Define the jack callback:
    let process_callback = move |_: &Client, ps: &ProcessScope| -> Control {
        for f in 0..(ps.n_frames() as usize) {
            while let Ok(feedback) = rx.try_recv() {
                echo.set_feedback(feedback);
            }; 
            while let Ok(delay) = rx2.try_recv() {
                echo.set_delay(delay);
            };
            let in_0 = input_ports[0].as_slice(ps);
            let in_1 = input_ports[1].as_slice(ps);
            // why tick has an argument?
            output_ports[0].as_mut_slice(ps)[f] = echo.tick(in_0[f]);
            output_ports[1].as_mut_slice(ps)[f] = echo.tick(in_1[f]);
        }
        Control::Continue
    };

    #[cfg(debug_assertions)]
    dbg!("Activate client");
    let proc = ClosureProcessHandler::new(process_callback);
    let active_client = &client
        .activate_async((), proc)
        .expect("Could not activate Jack Client");

    let _out_name0 = active_client.as_client().port_by_name(&out_port_name_0);
    let _out_name1 = active_client.as_client().port_by_name(&out_port_name_1);
    let _in_name0 = active_client.as_client().port_by_name(&in_port_name_0);
    let _in_name1 = active_client.as_client().port_by_name(&in_port_name_1);
    #[cfg(debug_assertions)]
    dbg!("ports:", _out_name0, _out_name1,_in_name0,_in_name1);

    //you can obtain the names of the jack port by typing jack_lsp in a shell
    #[cfg(debug_assertions)]
    dbg!("connect ports");
    active_client
        .as_client()
        .connect_ports_by_name(out_port_name_0, "system:playback_1")
        .expect("could not connect out port 0");
    active_client
        .as_client()
        .connect_ports_by_name(out_port_name_1, "system:playback_2")
        .expect("could not connect out port 1");
    active_client
        .as_client()
        .connect_ports_by_name("system:capture_1",in_port_name_0)
        .expect("could not connect in port 0");
    active_client
        .as_client()
        .connect_ports_by_name("system:capture_2",in_port_name_1)
        .expect("could not connect in port 1");

    let sleep_duration = Duration::from_millis(100);
    loop{
        print!("Entrez feedback (f32, actuellement {feedback}) : ");
        io::stdout().flush().unwrap();

        let mut input = String::new();
        io::stdin().read_line(&mut input).unwrap();

        match input.trim().parse::<f32>() {
            Ok(val) => {
                if !(0.0..=1.0).contains(&val) {
                    println!("Feedback must be between 0.0 and 1.0");
                } else {
                    feedback = val;
                }
            }
            Err(_) => {
                println!("Valeur invalide");
            }
        }
        tx.send(feedback).unwrap();
        print!("Entrez delay (usize lower than {}, actuellement {delay}): ",max_delay);
        io::stdout().flush().unwrap();

        let mut input = String::new();
        io::stdin().read_line(&mut input).unwrap();

        match input.trim().parse::<usize>() {
            Ok(val) => {
                delay = val;
            }
            Err(_) => {
                println!("Valeur invalide");
            }
        }
        tx2.send(delay).unwrap();
        println!("feedback: {feedback}, delay = {delay}");
        sleep(sleep_duration);
    }
}

