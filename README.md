# Brainfuck Compiler*

This repo is made up of two C programs and a basic brainfuck program. 

Compiling `bfCompiler.c` grants a program that allows you to translate brainfuck programs into assembly programs, and then tries to use YASM to compile them into actual executable programs. This at least works on Linux, given you have YASM installed (`sudo apt install yasm` or equivalent)

Compiling `bfInterpreter.c` grants a program that allows you to step through a brainfuck program iteratively, viewing the program and the output, similar to something like GDB.

`helloWorld.bf` is a brainfuck program that just prints hello world. Visit [brainfuck.org](https://brainfuck.org/) for more information on brainfuck, and more sample programs to compile or test.

*\*compiles into assembly, and invokes a third-party compiler to go the rest of the way to executable*
