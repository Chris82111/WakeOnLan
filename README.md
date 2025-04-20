# WakeOnLan

A simple program for sending a magic packet/Wake-On-LAN (WOL) packet to a network card of a computer to wake up the PC.

The used module can be used in C and C++ under Windows and Linux.

## Using

After compiling, the program can be used with various parameters.
Use `.exe` at the end under Windows or without anything under Linux.

The broadcast address of the network or the IP address of the end device should always be used.

```bat
WakeOnLan.exe <-i <"192.168.178.255">> <-m <"FF:FF:FF:FF:FF:FF">> [-m {60000}] [-h] [-s]
```

## Parameter description

| Switch | Description          | Optional |
|:-------|:---------------------|:--------:|
| -i     | Sets the IP address  |          |
| -m     | Sets the MAC address |          |
| -p     | Sets the port        |    x     |
| -h     | Shows this help      |    x     |
| -s     | Mute output          |    x     |

## Compile for Linux

```bash
gcc -Wall -Wextra -O3 -o WakeOnLan-linux-x86-64 WakeOnLan.c wake_on_lan.c && strip WakeOnLan-linux-x86-64
```

For Linux, [`musl`](https://www.musl-libc.org/how.html) can be used to create a portable version:

```bash
musl-gcc -static -Wall -Wextra -O3 -o WakeOnLan-linux-x86-64-portable WakeOnLan.c wake_on_lan.c && strip WakeOnLan-linux-x86-64-portable
```

## Compile for Windows

```bat
cmd /c "x86_64-w64-mingw32-gcc -Wall -Wextra -O3 -o WakeOnLan-windows-x86-64.exe WakeOnLan.c wake_on_lan.c -lws2_32 && strip WakeOnLan-windows-x86-64.exe & exit"
```
