use std::{
    io,
    sync::{
        Arc,
        atomic::{AtomicBool, AtomicU32, Ordering},
    },
    time::Duration,
};

use audio_backend::{AudioConfig, run_audio};

use cxx_juce::juce_audio_devices::{
    AudioIODevice,
    AudioIODeviceCallback,
    InputAudioSampleBuffer,
    OutputAudioSampleBuffer,
};

use crossterm::{
    event::{self, DisableMouseCapture, EnableMouseCapture, Event, KeyCode, KeyModifiers},
    execute,
    terminal::{disable_raw_mode, enable_raw_mode, EnterAlternateScreen, LeaveAlternateScreen},
};

use ratatui::{
    backend::CrosstermBackend,
    layout::{Constraint, Direction, Layout},
    style::{Color, Modifier, Style},
    text::{Line, Span},
    widgets::{Block, Borders, Gauge, Paragraph},
    Terminal,
};

use mydsp_rust::{
    AudioComponent as _,
    phasor::Phasor,
};

// ---------------------------------------------------------------------------
// Atomic f32 helpers
// ---------------------------------------------------------------------------

fn load_f32(atom: &AtomicU32) -> f32 {
    f32::from_bits(atom.load(Ordering::Relaxed))
}

fn store_f32(atom: &AtomicU32, val: f32) {
    atom.store(f32::to_bits(val), Ordering::Relaxed);
}

// ---------------------------------------------------------------------------
// Shared DSP parameters
// ---------------------------------------------------------------------------

struct Params {
    freq:           AtomicU32,
    echo_feedback:  AtomicU32,
    volume:         AtomicU32,
    bypass:         AtomicBool,
}

impl Params {
    fn new() -> Self {
        Self {
            freq:          AtomicU32::new(f32::to_bits(440.0)),
            echo_feedback: AtomicU32::new(f32::to_bits(0.5)),
            volume:        AtomicU32::new(f32::to_bits(0.5)),
            bypass:        AtomicBool::new(false),
        }
    }
}

// ---------------------------------------------------------------------------
// ECHO IMPLEMENTATION
// ---------------------------------------------------------------------------

pub struct Echo {
    buffer: Vec<f32>,
    index: usize,
    feedback: f32,
}

impl Echo {
    pub fn new(size: usize, feedback: f32) -> Self {
        Self {
            buffer: vec![0.0; size],
            index: 0,
            feedback: feedback.clamp(0.0, 0.99),
        }
    }

    pub fn set_feedback(&mut self, fb: f32) {
        self.feedback = fb.clamp(0.0, 0.99);
    }

    pub fn tick(&mut self, input: f32) -> f32 {
        let delayed = self.buffer[self.index];

        self.buffer[self.index] = input + delayed * self.feedback;

        self.index += 1;
        if self.index >= self.buffer.len() {
            self.index = 0;
        }

        delayed
    }
}

// ---------------------------------------------------------------------------
// DSP
// ---------------------------------------------------------------------------

struct MyDsp {
    phasor: Phasor,
    echo:   Echo,
    params: Arc<Params>,

    smoothed_feedback: f32,
}

impl AudioIODeviceCallback for MyDsp {
    fn about_to_start(&mut self, _device: &mut dyn AudioIODevice) {}

    fn process_block(
        &mut self,
        _input: &InputAudioSampleBuffer<'_>,
        output: &mut OutputAudioSampleBuffer<'_>,
    ) {
        self.phasor.set_freq(load_f32(&self.params.freq));

        let volume = load_f32(&self.params.volume);
        let bypass = self.params.bypass.load(Ordering::Relaxed);

        // -------------------------------------------------------------------
        // LIVE ECHO FEEDBACK SMOOTHING
        // -------------------------------------------------------------------
        let target_feedback = load_f32(&self.params.echo_feedback);
        let alpha = 0.005;

        self.smoothed_feedback += alpha * (target_feedback - self.smoothed_feedback);
        self.echo.set_feedback(self.smoothed_feedback);

        // -------------------------------------------------------------------

        for n in 0..output.samples() {
            let sample = if bypass {
                0.0
            } else {
                self.echo.tick(self.phasor.tick(0.0) * volume)
            };

            for c in 0..output.channels() {
                output[c][n] = sample;
            }
        }
    }

    fn stopped(&mut self) {}
}

// ---------------------------------------------------------------------------
// TUI slider
// ---------------------------------------------------------------------------

struct Slider {
    label: &'static str,
    unit:  &'static str,
    value: f32,
    min:   f32,
    max:   f32,
    step:  f32,
    write: fn(&Params, f32),
}

impl Slider {
    fn ratio(&self) -> f64 {
        ((self.value - self.min) / (self.max - self.min))
            .clamp(0.0, 1.0) as f64
    }

    fn inc(&mut self, fast: bool) {
        let s = if fast { self.step * 10.0 } else { self.step };
        self.value = (self.value + s).min(self.max);
    }

    fn dec(&mut self, fast: bool) {
        let s = if fast { self.step * 10.0 } else { self.step };
        self.value = (self.value - s).max(self.min);
    }
}

struct App {
    sliders: Vec<Slider>,
    selected: usize,
}

impl App {
    fn new() -> Self {
        Self {
            sliders: vec![
                Slider {
                    label: "Frequency",
                    unit: "Hz",
                    value: 440.0,
                    min: 20.0,
                    max: 2000.0,
                    step: 10.0,
                    write: |p, v| store_f32(&p.freq, v),
                },
                Slider {
                    label: "Echo Feedback",
                    unit: "",
                    value: 0.5,
                    min: 0.0,
                    max: 1.0,
                    step: 0.01,
                    write: |p, v| store_f32(&p.echo_feedback, v),
                },
                Slider {
                    label: "Volume",
                    unit: "",
                    value: 0.5,
                    min: 0.0,
                    max: 1.0,
                    step: 0.01,
                    write: |p, v| store_f32(&p.volume, v),
                },
            ],
            selected: 0,
        }
    }
}

// ---------------------------------------------------------------------------
// TUI rendering
// ---------------------------------------------------------------------------

fn draw(frame: &mut ratatui::Frame, app: &App, bypass: bool) {
let constraints: Vec<Constraint> = app
    .sliders
    .iter()
    .map(|_| Constraint::Length(6))   // <-- FIXED
    .chain([Constraint::Min(1)])
    .collect();


    let chunks = Layout::default()
        .direction(Direction::Vertical)
        .margin(1)
        .constraints(constraints)
        .split(frame.area());

    for (i, slider) in app.sliders.iter().enumerate() {
        let active = i == app.selected;

        let color = if active { Color::Cyan } else { Color::Gray };

        let style = if active {
            Style::default().fg(Color::Cyan).add_modifier(Modifier::BOLD)
        } else {
            Style::default().fg(Color::White)
        };

        let label = if slider.unit.is_empty() {
            format!("{:.2}", slider.value)
        } else {
            format!("{:.1} {}", slider.value, slider.unit)
        };

        let gauge = Gauge::default()
            .block(
                Block::default()
                    .title(Span::styled(format!(" {} ", slider.label), style))
                    .borders(Borders::ALL)
                    .border_style(Style::default().fg(color)),
            )
            .gauge_style(Style::default().fg(color).bg(Color::DarkGray))
            .ratio(slider.ratio())
            .label(label);

        frame.render_widget(gauge, chunks[i]);
    }

    let help = Paragraph::new(Line::from(vec![
        Span::styled("↑ ↓",        Style::default().fg(Color::Yellow)),
        Span::raw(": select  "),
        Span::styled("← →",        Style::default().fg(Color::Yellow)),
        Span::raw(": adjust  "),
        Span::styled("Shift+← →",  Style::default().fg(Color::Yellow)),
        Span::raw(": coarse  "),
        Span::styled("q / Esc",    Style::default().fg(Color::Yellow)),
        Span::raw(": quit"),
    ]));


    frame.render_widget(help, chunks[app.sliders.len()]);
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let params = Arc::new(Params::new());
    let params_tui = Arc::clone(&params);

    // ---------------- TUI THREAD ----------------
    std::thread::spawn(move || {
        enable_raw_mode().unwrap();

        let mut stdout = io::stdout();
        execute!(stdout, EnterAlternateScreen, EnableMouseCapture).unwrap();

        let backend = CrosstermBackend::new(stdout);
        let mut terminal = Terminal::new(backend).unwrap();

        let mut app = App::new();

        loop {
            let bypass = params_tui.bypass.load(Ordering::Relaxed);

            terminal.draw(|f| draw(f, &app, bypass)).unwrap();

            if event::poll(Duration::from_millis(16)).unwrap() {
                if let Event::Key(key) = event::read().unwrap() {
                    let fast = key.modifiers.contains(KeyModifiers::SHIFT);
                    let sel = app.selected;

                    match key.code {
                        KeyCode::Right => {
                            app.sliders[sel].inc(fast);
                            (app.sliders[sel].write)(&params_tui, app.sliders[sel].value);
                        }
                        KeyCode::Left => {
                            app.sliders[sel].dec(fast);
                            (app.sliders[sel].write)(&params_tui, app.sliders[sel].value);
                        }
                        KeyCode::Down => {
                            app.selected = (sel + 1) % app.sliders.len();
                        }
                        KeyCode::Up => {
                            app.selected = (sel + app.sliders.len() - 1) % app.sliders.len();
                        }
                        KeyCode::Char('b') => {
                            let state = params_tui.bypass.load(Ordering::Relaxed);
                            params_tui.bypass.store(!state, Ordering::Relaxed);
                        }
                        KeyCode::Char('q') | KeyCode::Esc => {
                            disable_raw_mode().unwrap();
                            execute!(
                                terminal.backend_mut(),
                                LeaveAlternateScreen,
                                DisableMouseCapture,
                            )
                            .unwrap();
                            terminal.show_cursor().unwrap();
                            std::process::exit(0);
                        }
                        _ => {}
                    }
                }
            }
        }
    });

    // ---------------- AUDIO THREAD ----------------
    let config = AudioConfig {
        duration: None,
        ..AudioConfig::default()
    };

    let dsp = MyDsp {
        phasor: Phasor::new(None, config.sample_rate),
        echo: Echo::new(10000, 0.5),
        params: Arc::clone(&params),
        smoothed_feedback: 0.5,
    };

    run_audio(dsp, &config)?;

    Ok(())
}