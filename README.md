# MoltarOS

MoltarOS will be a simple and minimal implementation of an Operating System, developed initially using [OSDev Tutorials](http://wiki.osdev.org/Tutorials), and later with me implementing things on my own.

MoltarOS is not intended to be a commercial or competitive operating system, it is merely an attempt at a more academic minimal but OS, with the goal to teach myself, the sole developer, the ins and outs of how an Operating System works. The features I plan to implement are...

1) EXT2 Filesystem

2) Basic Keyboard driver

3) Basic interactive shell

4) Simplistic yet effective CPU scheduling and multitasking

5) Multithreading primitives and message-passing 

6) Basic Network Stack, enough to implement BSD sockets

7) Memory allocation and implementation of a heap

8) ELF support

These will, without a doubt, take a very long time, and I may not ever finish them, however if and when I do, another notable goal would be to implement support for X windows server for a basic GUI.

#Progress Update

## Version .001a

Implemented the GDT, IDT (and IRQs and ISRs), reprogrammed the PIC's (Master and Slave), and created an interrupt handler for the PIT. Unfortunately, right now it uses the PIT to call the RTC rather than handling RTc separately, but that will come next, kind of busy right now.

![Screenshot](/os_progress.PNG)