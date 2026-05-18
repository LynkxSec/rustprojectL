# mydsp-rust-examples
Audio Rust project using mydsp-rust crate and jack

The mydsp-jack crate  is developed with the aim of helping to set up a [5th year course option](https://5tc-rust-cc1d61.gitlabpages.inria.fr/) 
on introduction to  Rust 
([@INSA Lyon - Département Télécommunications, Services et Usages](https://telecom.insa-lyon.fr/) 
&& [@Emeraude Research Team](https://team.inria.fr/emeraude/)).


# How to use 
show examples of using the 'mydsp-rust' crate with jack
The current [`mydsp-rust-example` crate](https://github.com/inria-emeraude/mydsp-rust-example) provides example of how to use mydsp-rust crate with various audio back end: 'jack' (available on Linux and Mac-OS through 'pipewire') or 'juce' (available on windows platforms)

For the application using jack (e.g. 'bypass-jack'), the ``pw-jack`` command should be used to run: 

```pw-jack cargo run```

the general scheme of audio application Rust program is to create a 'mydsp' crate that implement the 'run' method. This 'run' method will dynamically build the audio pipeline connecting the different audio components used from 'mydsp-rust'.

Because of Rust ownership rules, each parameter controlling the audio components cannot by accessed directly from the 'run' method. A tunnel has be set up for each parameter that is supposed to vary

TODO: a work on the 'process_callback- function and 'ClosureProcessHandler' 