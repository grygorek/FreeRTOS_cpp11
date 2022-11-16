# Example Program to use GCC FreeRTOS C++ library with QEMU

## QEMU

QEMU can run ARM code in virtual machine. In this way the library 
can be tested without dependency on hardware.

I have tested with this version


```console
$ qemu-system-arm --version
QEMU emulator version 6.1.0 (v6.1.0-11882-g7deea770bf-dirty)
Copyright (c) 2003-2021 Fabrice Bellard and the QEMU Project developers
```

## Platform Decision

There is quiet good selection of platforms supported by QEMU.
I picked `ARM Versatile Express for Cortex-A9` due to:

Pros:

`+` Enough memory to run entire library test  
`+` Easy to port FreeRTOS to it

Cons:

`-` Nothing of the importance

## Execution

Build the demo project with this commands:

```console
$ cmake ../FreeRTOS_cpp11 -G "Eclipse CDT4 - Unix Makefiles" -Darmca9=1
$ cmake --build .
```

To run the program execute this command:

```console
$ qemu-system-arm -M vexpress-a9 -m 128M -nographic -kernel test_ca9.elf
```

The test is run in a loop printing to a console:

```console
$ qemu-system-arm -M vexpress-a9 -m 128M -nographic -kernel test_ca9.elf
ARM CA9 - start test
Run...OK
Run...OK
```

# Run in GDB mode

It is possible to debug the application with gdb using the following command: 

```console
$ qemu-system-arm -M vexpress-a9 -m 128M -nographic -kernel test_ca9.elf -s -S
```

QEMU window will open but will halt the program. The window will be blank. 
Look for QEMU documentation how to use it with gdb.

Do not forget to compile the application with -g -O0 flags in the cmake script.