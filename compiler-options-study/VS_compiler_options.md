# Compiler Options

## TL;DR

some options to give VS compiler/linker when building small projects from command-line.

## Introduction

My experience with C and C++ was always been using gcc. On Linux (Ubuntu) you can install it using a single command, and on Windows I've used Mingw (or Cygwin). I guess I was aiming for portability and development environment homogeneity across platforms. Now that I'm using Visual Studio at work, this time, while studying C++ and random graphics programming topics while on paternity leave, I decided to use and learn compiling and building stuff using VS on command line.

## Command Prompt for VS Compiler

First of all, just using the standard "Command Prompt" or "PowerShell" won't work because one needs to have VS compiler on PATH and also should have some environment stuff set up. Easiest way to get a functioning command-line environment in which we can compile and link is to use the "command tools prompt". I type "x64" in search bar and "x64 Native Tools Command Prompt for VS 2022" shows up. This runs a BAT file when opening and prepares the environment in which we can call `cl.exe` from anywhere.

> **********************************************************************
> **Visual Studio 2022 Developer Command Prompt v17.0.6
>** Copyright (c) 2021 Microsoft Corporation
> **********************************************************************
> [vcvarsall.bat] Environment initialized for: 'x64'

## Single CPP file

Single CPP files without any external inclusion or libraries:

```cmd
cl single_file.cpp
```

outputs:

```txt
Microsoft (R) C/C++ Optimizing Compiler Version 19.30.30709 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

first.cpp
Microsoft (R) Incremental Linker Version 14.30.30709.0
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:first.exe
first.obj
```

this combines both compiling of `single_file.obj` (via "C/C++ Optimizing Compiler Version 19.30.30709 for x64") and linking to `single_file.exe` (via "Incremental Linker Version 14.30.30709.0"). Both of them are placed in the same folder as the CPP file. Also note the `/out:first.exe`. The output executable name is automatically set to the CPP filename.

One weird looking option that showed up in my StackOverflow queries was `/EHsc`, where EH stands for exception handling. [/EH \(Exception handling model\) \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model?view=msvc-170) says

> "The default exception unwinding code doesn't destroy automatic C++ objects outside of `try` blocks that go out of scope because of an exception. Resource leaks and undefined behavior may result when a C++ exception is thrown."

and recommends to use `/EHsc` which brings the "Standard C++ exception handling". Since I'm a noob I don't have an opinion on whether to use Exceptions or not. I'll just keep this option in every build command from now on, and move on with my life.

```cmd
cl /EHsc single_file.cpp
```

## Multiple CPP files

Say our project is made of more than one CPP files, because we are being good engineers and splitting our code into logical components. We have one `main.cpp` file where the `main()` function leaves, and other `moduleN.cpp` files that have various classes and functions.

```cmd
cl /EHsc main.cpp module1.cpp
```

observe linker output:

```txt
/out:main.exe
main.obj
module1.obj
```

It gives the name of the first CPP file to the executable. For example if we had put `module1.cpp` first, `cl /EHSC main.cpp module1.cpp`, we'd get `module1.exe`.

`main.exe` is not much descriptive. In order to explicitly name the output file one needs to give its name to the linker as follows

```cmd
cl /EHsc main.cpp module1.cpp /link /out:program1.exe
```

See [/OUT \(Output File Name\) \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/out-output-file-name?view=msvc-170). See that in order to provide the `/out` option through `cl` we first give `/link` option to forward the `/out` options to the linker.

It's a stylistic choice to whether the name the CPP file that has the `main()` function after the project name, or just call it `main.cpp` and provide the output name explicitly. For projects with one or a few files I usually to the former.

See [CL Invokes the Linker \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/cl-invokes-the-linker?view=msvc-170)

## Using C++20 features

C++ evolves in time and every 3 years new features are added by the committee. In order to use most recent features we need to tell the compiler which version of C++ we want to use.

For a list of all features introduced in C++20 please see [C\+\+20 \- cppreference\.com](https://en.cppreference.com/w/cpp/20). As an example one of those features is the `contains()` method of `std::map`. (As a Python dev I found it very surprising that this method didn't exist in previous versions!) See [std::map<Key,T,Compare,Allocator>::contains \- cppreference\.com](https://en.cppreference.com/w/cpp/container/map/contains).

```cpp
// cpp20.cpp
std::map<int, char> example = {{1, 'a'}, {2, 'b'}};
std::cout << example.contains(1) << " " << example.contains(3) << std::endl;
```

Compiling above file via `cl /EHsc cpp20.cpp` will fail with following error message: `cpp20.cpp(7): error C2039: 'contains': is not a member of 'std::map<int,char,std::less<int>,std::allocator<std::pair<const int,char>>>'`.

To enable c++20 features use `/std` option. Now, following build command does not throw any errors.

```cmd
cl /std:c++20 /EHsc cpp20.cpp
```

[/std \(Specify Language Standard Version\) \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/std-specify-language-standard-version?view=msvc-170) Looks like the default is `/std:c++14`.
There is also a `/std:c++latest` option to always use the latest version. But I prefer to set the version explicitly.

**********************************************************************

## Compiler Warnings

See [/w, /W0, /W1, /W2, /W3, /W4, /w1, /w2, /w3, /w4, /Wall, /wd, /we, /wo, /Wv, /WX \(Warning level\) \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level?view=msvc-170)

There is also this topic of conformance, non-conformance. See [/permissive\- \(Standards conformance\) \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/permissive-standards-conformance?view=msvc-170). However, looks like when c++20 is chosen, this option is automatically added.

## Compiling a library

* ColorSpace
* ImGui

```cmd
cl /W4 /EHsc main.cpp /I../dependencies/blend2d/src ../dependencies/blend2d/build/Release/blend2d.lib /MD
```

Most libraries come with CMakeLists.txt

* blend2d
* minifb

## Project depending on above libraries

processing++

```cmd
cl /W4 /EHsc main.cpp processing.cpp sketch1.cpp /I../dependencies/minifb/include ../dependencies/minifb/build/Release/minifb.lib User32.lib OpenGL32.lib Gdi32.lib Winmm.lib /I../dependencies/blend2d/src ../dependencies/blend2d/build/Release/blend2d.lib /MD
```

```cmd
cl /W4 /EHsc main.cpp processing.cpp sketch1.cpp ^
  User32.lib OpenGL32.lib Gdi32.lib Winmm.lib ^
  /I../dependencies/minifb/include ../dependencies/minifb/build/Release/minifb.lib ^
  /I../dependencies/blend2d/src ../dependencies/blend2d/build/Release/blend2d.lib ^
  /I../dependencies/ColorSpace/src ../dependencies/ColorSpace/bld/ColorSpace.lib ^
  /MD
```

At this point it's better to have a `build.bat` file with above command in it.

It'll be even better to have a `Makefile` and more better to have a `CMakeLists.txt` but the goal here is to learn the VS compiler options.

## Compiler warnings only for our code, not for dependencies

## Creating our own library

instead of an executable

```cmd
cl /c /EHsc src\ColorSpace.cpp src\Comparison.cpp src\Conversion.cpp
lib ColorSpace.obj Comparison.obj Conversion.obj /out:bld\ColorSpace.lib
```

## Debug symbols

`/Zi`

## Optimization

`/O2`

## References

* All compiler options [Compiler Options Listed by Category \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-170)
* All linker options: [MSVC Linker options \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/linker-options?view=msvc-170)
