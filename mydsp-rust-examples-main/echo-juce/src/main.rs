use std::{
    io,
    sync::{Arc, atomic::{AtomicU32, Ordering}},
    time::Duration,
};

use audio_backend::{AudioConfig, run_audio};
use cxx_juce::juce_audio_devices::{
    AudioIODevice, AudioIODeviceCallback,
    InputAudioSampleBuffer, OutputAudioSampleBuffer,
};
use crossterm::{
    event::{self, Event, KeyCode, KeyModifiers},
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
    volume:   AtomicU32, // 0.0–1.0
    feedback: AtomicU32, // 0.0–0.95
}

impl Params {
    fn new() -> Self {
        Self {
            volume:   AtomicU32::new(f32::to_bits(1.0)),
            feedback: AtomicU32::new(f32::to_bits(0.3)),
        }
    }
}

// ---------------------------------------------------------------------------
// DSP (echo)
// ---------------------------------------------------------------------------

struct MyDsp {
    params: Arc<Params>,
    delay:  Vec<f32>,
    index:  usize,
}

impl MyDsp {
    fn new(params: Arc<Params>) -> Self {
        let delay_samples = 44100 / 3; // ~333ms echo
        Self {
            params,
            delay: vec![0.0; delay_samples],
            index: 0,
        }
    }
}

impl AudioIODeviceCallback for MyDsp {
    fn about_to_start(&mut self, _device: &mut dyn AudioIODevice) {}

    fn process_block(
        &mut self,
        input: &InputAudioSampleBuffer<'_>,
        output: &mut OutputAudioSampleBuffer<'_>,
    ) {
        let volume   = load_f32(&self.params.volume) as f64;
        let feedback = load_f32(&self.params.feedback) as f64;

        let in_channels = input.channels().max(1);

        for n in 0..output.samples() {
            let mut dry = 0.0f64;

            for c in 0..input.channels() {
                dry += input[c][n] as f64;
            }
            dry /= in_channels as f64;

            // read from delay line
            let echo = self.delay[self.index] as f64;

            // write new value into delay line
            let new_val = dry + echo * feedback;
            self.delay[self.index] = new_val as f32;

            // advance circular index
            self.index = (self.index + 1) % self.delay.len();

            // output = dry + echo
            let out = (dry + echo) * volume;

            for c in 0..output.channels() {
                output[c][n] = out as f32;
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
        ((self.value - self.min) / (self.max - self.min)).clamp(0.0, 1.0) as f64
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
    sliders:  Vec<Slider>,
    selected: usize,
}

impl App {
    fn new() -> Self {
        Self {
            sliders: vec![
                Slider {
                    label: "Volume",
                    unit:  "",
                    value: 1.0,
                    min:   0.0,
                    max:   1.0,
                    step:  0.01,
                    write: |p, v| store_f32(&p.volume, v),
                },
                Slider {
                    label: "Echo Feedback",
                    unit:  "",
                    value: 0.3,
                    min:   0.0,
                    max:   0.95,
                    step:  0.01,
                    write: |p, v| store_f32(&p.feedback, v),
                },
            ],
            selected: 0,
        }
    }
}

// ---------------------------------------------------------------------------
// TUI rendering
// ---------------------------------------------------------------------------

fn draw(frame: &mut ratatui::Frame, app: &App) {
    let constraints: Vec<Constraint> = app
        .sliders
        .iter()
        .map(|_| Constraint::Length(3))
        .chain([Constraint::Min(1)])
        .collect();

    let chunks = Layout::default()
        .direction(Direction::Vertical)
        .margin(1)
        .constraints(constraints)
        .split(frame.area());

    for (i, slider) in app.sliders.iter().enumerate() {
        let active = i == app.selected;
        let color  = if active { Color::Cyan } else { Color::Gray };
        let style  = if active {
            Style::default().fg(Color::Cyan).add_modifier(Modifier::BOLD)
        } else {
            Style::default().fg(Color::White)
        };

        let label = if slider.unit.is_empty() {
            format!("{:.2}", slider.value)
        } else {
            format!("{:.2} {}", slider.value, slider.unit)
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

    std::thread::spawn(move || {
        enable_raw_mode().unwrap();

        let mut stdout = io::stdout();
        execute!(stdout, EnterAlternateScreen).unwrap();

        let backend = CrosstermBackend::new(stdout);
        let mut terminal = Terminal::new(backend).unwrap();

        let mut app = App::new();

        loop {
            terminal.draw(|f| draw(f, &app)).unwrap();

            if event::poll(Duration::from_millis(16)).unwrap() {
                if let Event::Key(key) = event::read().unwrap() {
                    let fast = key.modifiers.contains(KeyModifiers::SHIFT);
                    let sel  = app.selected;

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
                        KeyCode::Char('q') | KeyCode::Esc => {
                            disable_raw_mode().unwrap();
                            execute!(
                                terminal.backend_mut(),
                                LeaveAlternateScreen,
                            ).unwrap();
                            terminal.show_cursor().unwrap();
                            std::process::exit(0);
                        }
                        _ => {}
                    }
                }
            }
        }
    });

    // Audio config
    let config = AudioConfig {
        input_channels: 2,
        duration: None,
        ..AudioConfig::default()
    };

    let dsp = MyDsp::new(Arc::clone(&params));

    run_audio(dsp, &config)?;
    Ok(())
}
