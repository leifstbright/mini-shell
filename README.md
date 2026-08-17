# Mini Shell

A simple Unix-like shell implemented in C that replicates core behaviors of the
Linux/Bash shell.

## Overview
This project implements fundamental shell functionality, including command
parsing, process creation, and execution using system calls.

It provides hands-on experience with operating system concepts such as processes,
forking, and command execution.

## Features
- Command parsing and execution
- Process creation using fork/exec
- Basic shell behavior replication

## Technologies
- C
- Linux / Unix system calls
- Process management

## Requirements

- GCC
- readline development library

Install readline if needed:
```bash
sudo apt install libreadline-dev
```

## Build

```bash
make
```

This compiles `ssi.c` into an executable called `ssi`.

## Run

```bash
./ssi
```

## Clean

Remove the compiled binary and object files:

```bash
make clean
```

