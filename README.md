# MoltarOS

## Summary

MoltarOS is a 'toy' operating system developed purely for academic purposes and interests. It is a 'toy' in that it never is intended to compete against other well established operating systems, such as FreeBSD, Linux, Windows, or Mac OS X, but rather with the intent that I may learn how they (potentially) work internally. The project by itself is a very ambitious one, but not unachievable. This project was inspired by the [ToaruOS](https://github.com/klange/toaruos), which was developed by a single undergraduate student, which made it clear that it certainly was possible. The operating system will begin implementation by following [OSDev Tutorials](http://wiki.osdev.org/Tutorials), and later potentially adding my own support for other things based on where my interests lead me.

## Progress

The below depicts an early "schedule" or rather a path I will be taking in terms to seeing this project to relative completion (or at least to the point I am content with it). The below progress may or may not be implemented in the master branch, but if it is checked, it will be in the development branch. As well, just because it is checked does not mean it is "finished" per-say, it means that it is working well enough, and most likely will be enhanced in the future.

- [x] "Hello World" Kernel
- [x] Interrupts (IRQ + ISR)
- [x] VGA Display Driver
- [x] Keyboard Driver
- [ ] Interactive Shell
- [ ] Memory Management (Physical + Virtual)
- [ ] File System
- [ ] Multitasking and Scheduling
- [ ] Networking
- [ ] Process Creation and Managements
- [ ] ELF Binary Support
- [ ] Graphical User Interfaces

#Progress Update

## Version .001a

Implemented the GDT, IDT (and IRQs and ISRs), reprogrammed the PIC's (Master and Slave), and created an interrupt handler for the PIT. Unfortunately, right now it uses the PIT to call the RTC rather than handling RTc separately, but that will come next, kind of busy right now.

![Screenshot](/os_progress.PNG)

## Version .001b

Implemented the keyboard driver, but it won't be able to communicate with other components (such as an interactive shell) until memory management is implemented... perhaps, not even until processes and scheduling is implemented. That is the next thing I will be releasing. Currently, when you execute
the OS, it will have a blank screen, until you use the keyboard. When executing the keyboard, it will display what key has been pressed and what has been released... but only one at a time unfortunaately, no multi-key press events are supported.

![Screenshot](/kbd_input.PNG)