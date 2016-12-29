# MoltarOS

## Summary

MoltarOS is a 'toy' operating system developed purely for academic purposes and interests. It is a 'toy' in that it never is intended to compete against other well established operating systems, such as FreeBSD, Linux, Windows, or Mac OS X, but rather with the intent that I may learn how they (potentially) work internally. The project by itself is a very ambitious one, but not unachievable. This project was inspired by the [ToaruOS](https://github.com/klange/toaruos), which was developed by a single undergraduate student, which made it clear that it certainly was possible. The operating system will begin implementation by following [OSDev Tutorials](http://wiki.osdev.org/Tutorials), and later potentially adding my own support for other things based on where my interests lead me.

## Progress

The below depicts an early "schedule" or rather a path I will be taking in terms to seeing this project to relative completion (or at least to the point I am content with it). The below progress may or may not be implemented in the master branch, but if it is checked, it will be in the development branch. As well, just because it is checked does not mean it is "finished" per-say, it means that it is working well enough, and most likely will be enhanced in the future.

- [x] "Hello World" Kernel
- [x] Interrupts (IRQ + ISR)
- [x] VGA Display Driver
- [x] Keyboard Driver
- [x] Memory Management (Physical + Virtual)
- [x] Higher Half Kernel
- [ ] File System
- [ ] Process Creation and Managements
- [ ] Multitasking and Scheduling
- [ ] Networking
- [ ] ELF Binary Support
- [ ] Interactive Shell
- [ ] Graphical User Interfaces

#Progress Update & Changelog

## Version .002

It is FINALLY here! I have implemented not only memory management (paging and a heap allocator), but even converted to a higher-half kernel approach, which was also easier, surprisingly, than a normal identity-mapped system. I've also fixed up the tests and their output format to better portray the significance of the initialization of the kernel thus far. I am very satisfied with what I have done, but unfortunately, I have to attend to another project for the time being.

![Screenshot](/all_tests_and_heap.PNG)

## Version .001c

Began implementation of memory management, but not finished yet. Paging SHOULD be implemented very soon (and most code has been written), as well the heap is also mainly written using Pancakes' Bitmap Heap implementation, and paired with the identity paging technique, development of all other parts of the OS should proceed as expected. Today, I also managed to make use of the multiboot info struct that GRUB
gives us that was pushed on the stack in 'kernel_init', and now it can detect the amount of physical memory (RAM) that the machine (or virtual machine) has to offer. This is crucial to finished memory management.

As well, there has been a huge restructure in terms of the hierarchy and logical structure of the operating system. For example, the folders 'mm' and 'drivers' now also have their own respective folders in the
include folder, I.E 'drivers/vga.c' will have it's header in 'include/drivers/vga.h'. While in terms of usability, it is not too much of an update (yet, memory management will be in version .002), there has been
a significant amount of work and should be pushed to master.

Lastly, I also added a nice logger macro, `KLOG`, and panic macro, `KPANIC`.

![Screenshot](/ram_and_kbd.PNG)

## Version .001b

Implemented the keyboard driver, but it won't be able to communicate with other components (such as an interactive shell) until memory management is implemented... perhaps, not even until processes and scheduling is implemented. That is the next thing I will be releasing. Currently, when you execute
the OS, it will have a blank screen, until you use the keyboard. When executing the keyboard, it will display what key has been pressed and what has been released... but only one at a time unfortunaately, no multi-key press events are supported.

![Screenshot](/kbd_input.PNG)

## Version .001a

Implemented the GDT, IDT (and IRQs and ISRs), reprogrammed the PIC's (Master and Slave), and created an interrupt handler for the PIT. Unfortunately, right now it uses the PIT to call the RTC rather than handling RTc separately, but that will come next, kind of busy right now.

![Screenshot](/os_progress.PNG)
