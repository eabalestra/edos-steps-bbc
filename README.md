# EDOS: A simple educational operating system from scratch

This project contains documentation and code for building a minimalist, very
simple operating system (OS) from scratch. It was initially developed by Prof.
Marcelo Arroyo for the operating systems course at Río Cuarto National
University, Argentina.

We were using [xv6](https://pdos.csail.mit.edu/6.1810/2024/xv6.html) in the
past. Xv6 is a great resource for OS teaching. After we got some experience with
xv6, we decide to build an OS from scratch in simple steps.

The main goal is to get a simple OS kernel with a basic set of system calls.
Students can extend it with a UNIX like API and applications, similar to xv6.

EDOS can be used as a set of hands-on laboratory or homework projects to
develop a minimal (but almost complete) OS from scratch.

This project contain the EDOS development in steps. Teachers can use it as a
incremental development of an OS from booting to running user applications in
simple steps.

EDOS run on a RISC-V 32-bit architecture. Should be easy to migrate to another
platform.

In each step directory there is a `README.md` file with goals, some concepts and
implementation details. Also, some exercises for students are proposed.

This project is under development (May, 2025). Until now, developed steps are
from 0 (boot) until 7 (memory management).

The main goal is to keep it simple and small with code focusing on readability
than performance. We'l try to maintain it well documented for easy hacking.

Contributions and comments are welcome.

## Requirements

1. A cross-compiler running in the host platform for RISC-V. We use the GNU
   build chain. With a few changes in `Makefile` build files it should compiled
   and linked with _Clang/LLVM_.
2. [QEMU](https://www.qemu.org/) for running the OS in a RISC-V emulated
   machine.

Most GNU-Linux distributions provide the necessary packages.

### Debian/Ubuntu based systems

```
sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
```

### Arch Linux:

```
$ sudo pacman -S riscv64-linux-gnu-binutils riscv64-linux-gnu-gcc riscv64-linux-gnu-gdb qemu-emulators-full
```

### MacOS

```
$ xcode-select --install
```

Then install RISCV compiler toolchain with [Homebrew](https://brew.sh/)

```
$ brew tap riscv/riscv
$ brew install riscv-tools
$ brew install qemu
```

Maybe you should set `$PATH` environment variable in your shell init script.

```
PATH=$PATH:/usr/local/opt/riscv-gnu-toolchain/bin
```

### MS-Windows

Install packages in an Ubuntu system installed on WSL 2 (Windows Subsystem for
Linux, version 2).

### Running in a Linux virtual machine

Install some linux distribution on some virtualization platform like
[VirtualBox](https://www.virtualbox.org/). Then install the required packages
following the above instructions.

## Resources

1. _RISCV programming book_: Prof. Edson Borin.
   https://riscv-programming.org/book/riscv-book.html
2. _XV6, a simple Unix-like teaching operating system_.
   https://pdos.csail.mit.edu/6.1810/2024/xv6.html
3. Daniel Magnum RISC-V blog: https://danielmangum.com/categories/risc-v-bytes/
4. _Writing an Operating System in 1000 lines_.
   https://github.com/nuta/operating-system-in-1000-lines
5. _Operating Systems UNRC Course Notes (in Spanish)_. Marcelo Arroyo.
   https://marceloarroyo.gitlab.io/cursos/SO/index.html
6. _Mini RISCV OS_: https://github.com/cccriscv/mini-riscv-os

## Authors

- **Arroyo Marcelo** - Professor. ([@marceloarroyo](https://marceloarroyo.gitlab.io))
- **Balestra Edgar Agustin** ([@eabalestra](https://github.com/eabalestra))
- **Buil Delfina** ([@delfib](https://github.com/delfib))
- **Compagnucci Bruno José Ignacio** ([@JoseCompag](https://github.com/JoseCompag))
