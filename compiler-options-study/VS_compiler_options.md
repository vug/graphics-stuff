# Compiler Options

## TL;DR

* Some options to give VS compiler/linker when building small projects from command-line. 
* Projects of various size (single file, multi-file, multi-dependencies)
* C Runtime library.
* Which LIB files to provide to the linker.
* Compiler warnings.
* Various C++ specifications (c++17, c++20 etc). 
* Creating LIB files.

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

## Compiler Warnings

One way compilers help with learning the language and its best practices is to rely on it to throw warnings to detect issues at compile time. These can be about simple stuff such as a declared but unused variable [Compiler Warning (level 3) C4101](https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-3-c4101?view=msvc-170) to about issues that are legal C++ but can cause problems at runtime such as "uninitialized local variable 'name' used" [Compiler Warning (level 1 and level 4) C4700](https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-and-level-4-c4700?view=msvc-170) Examples from documentation:

```cpp
int main() {
  int i;   // C4101 (i never used later)

  int s, t, u, v;   // Danger, uninitialized variables
  s = t + u + v;    // C4700: t, u, v used before initialization
}
```

See [Compiler warnings C4000 - C5999](https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warnings-c4000-c5999?view=msvc-170) for an index of all warnings or [Compiler Warnings by compiler version](https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warnings-by-compiler-version?view=msvc-170) for having all in the same page with their names and categorized by VS version.

I find these warnings mostly very useful because they help me reduce the number of bugs. Some of them are annoying, such as the ones that make you use `static_cast` instead of implicit casting: "conversion from 'type_1' to 'type_2' requires a narrowing conversion" [C4838](https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4838?view=msvc-170) and "'context' : truncation from 'type1' to 'type2'" [C4305](https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4305?view=msvc-170). You might "know what you are doing" by assigning a `float` value into an `int`, but also might not. :-) Usually an issue when need to narrow down `size_t` into `int` because I'm being a good citizen and using `size_t` for vector indices, but the library I'm using is old and still uses `int`s everywhere... But I see their point and obey them.

No that these are just warnings, not errors. Errors prevent succesful compilation. See the index of errors here [Compiler errors C2000 - C3999, C7000 - C7999](https://docs.microsoft.com/en-us/cpp/error-messages/compiler-errors-1/compiler-errors-c2000-c3999?view=msvc-170). Whereas one can turn off warnings and suffer the consequences.

The warnings have levels. More essential ones have lower level (1) and less insequential ones have higher level (4). `/W0` means turning off warnings. I prefer `/W4`. See [/w, /W0, /W1, /W2, /W3, /W4, /w1, /w2, /w3, /w4, /Wall, /wd, /we, /wo, /Wv, /WX \(Warning level\) \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level?view=msvc-170) "For a new project, it may be best to use /W4 in all compilations. This option helps ensure the fewest possible hard-to-find code defects."

There is an even more verbose warning option: `/Wall`. I assume they are not included at level-4 for a reason and not use this option. See [Compiler warnings that are off by default \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/preprocessor/compiler-warnings-that-are-off-by-default?view=msvc-170)

There is this "all-in" way about compiler warnings, which is `/WX`. This turns any warning into an error. If I'm working on a serios project I'll turn this on because I don't want a code that causes compiler warnings to leak into production. `/WX` will make pushing of the commit with bad code be stopped by CI systems. However, if I'm coding for fun or to study, I'd like to see the resulted executable before fixing the warning.

```cmd
cl /std:c++20 /EHsc /W4 program.cpp module1.cpp
# optionals: /WX /link /out:program1.exe
```

There is also this topic of conformance, non-conformance. See [/permissive\- \(Standards conformance\) \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/permissive-standards-conformance?view=msvc-170). However, looks like when c++20 is chosen, this option is automatically added.

## Compiling a library

When working on an actual project we rarely write everything ourselves, usually we rely on other libraries/code which we bring into our project as dependencies.

### Header-only libraries

Some of them are "header-only" libraries. Which means they don't have any CPP file. They are made of just H files. You usually include one H file in your CPP files that require their functionality and you are good to go. 

At first this seems great because of practicality: just download them into your project and start including headers. But recently I learned and realized that this increases the compilation times. The logic provided by a header-only library is copy-pasted in every CPP file that uses it and the same code is recompiled again and again. Actually even standard library is bloated with lots of code. (For now please see [GitHub - stdfwd: Forward declarations for C++ standard library](https://github.com/olegpublicprofile/stdfwd) and [Magnum - Forward-declaring STL container types](https://blog.magnum.graphics/backstage/forward-declaring-stl-container-types/) One day I might write about forward declarations for faster compile times.)

Let's use [stb](https://github.com/nothings/stb) as an example. It's a collection of single header libraries that does various stuff such as image saving ([stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)), recursive `#include` support for GLSL ([stb_include.h](https://github.com/nothings/stb/blob/master/stb_include.h)) etc.

First option is to bring header file into the project's source code:

```cmd
project/image_program.cpp
project/stb_image.h
```

where

```cpp
// image_program.cpp
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned char *data = stbi_load(...
```

This case does not require any changes in our build command.

However, projects that rely on multiple dependencies and made of multiple files might have a more organized structure as follows

```cmd
project/src/image_program.cpp
project/src/module1.cpp
project/dependencies/stb/stb_image.h
project/dependencies/other/...
```

Now, the single-header library file is not in the same folder as the CPP file that uses it. `#include "stb_image.h"` will throw an error about not being able to that file. We have to tell the compiler where to look at to find the header file.

```cmd
# from project/src/
cl /std:c++20 /EHsc /W4 image_program.cpp module1.cpp /I"..\dependencies\stb"

# from project/
cl /std:c++20 /EHsc /W4 src/image_program.cpp src/module1.cpp /I"dependencies\stb"
```

### Libraries without build support

i.e. libraries that are only made of header and cpp files. We can choose to compile them along with our CPP files each time we build our project by adding their CPP files into our build command. However, the compiler will do the exact same calculation at each build command and spend time unnecessarily on library files, even though nothing has changed.

We can "cache" the compilation output of the library by first only compiling/linking the library itself into a non-executable binary file and link our project against it.

There are two types of library linking that requires two types of build outputs: dynamic and static. The content of a statically linked library (a LIB file) will be included in the final project executable (increasing its size), but it's easier to use. The content of a dynamically linked library (on Windows a DLL file) is loaded at run-time from its file (final executable size is smaller) but it's harder to use. Hopefully once I'll study and learn hot-reloading code for plug-in, visual effect development, I'll write about storing logic in DLL files and loading/reloading them at runtime.

Let's go over two libraries, one small one more substantial.

First, <https://github.com/berendeanicolae/ColorSpace>. The project comes with a Visual Studio solution, but because we are studying command-line options we'll ignore that.

The src folder has 3 pairs of header and CPP files: ColorSpace, Comparison, Conversion. Declarations in header files to be included in our project, and definitions (logic) in CPP files that we want to store in a LIB file.

```cmd
cd project/dependencies/ColorSpace
mkdir build
cl /c /EHsc src\ColorSpace.cpp src\Comparison.cpp src\Conversion.cpp
# OR
cl /c /EHsc src\*.cpp /Fobuild\
lib build/*.obj /out:build\ColorSpace.lib
```

Usually `build/` directory is added to `.gitignore` anything happening there is hidden from version control to prevent cluttering it with binary, temporary files. We'll store temporary OBJ files, and generated LIB file there.

`/c` means that we are not going to link anything, just will compile OBJ files. See [/c (Compile Without Linking)](https://docs.microsoft.com/en-us/cpp/build/reference/c-compile-without-linking?view=msvc-170).

We also did the trick of using `*.cpp` instead of naming each CPP file to save few keystrokes! and used `/Fo` (output file - obj) option to set the folder into which OBJ files are placed. See [/Fo (Object File Name)](https://docs.microsoft.com/en-us/cpp/build/reference/fo-object-file-name?view=msvc-170)

Also note that we didn't use `/W4` because we don't care about the compiler warnings caused by the dependency. That's not our reponsibility.

`lib` combines `obj` files into a single LIB file. As usual `/out` option to linker is used to determine the final LIB file location and name.

See [Overview of LIB](https://docs.microsoft.com/en-us/cpp/build/reference/overview-of-lib?view=msvc-170) and [Running LIB](https://docs.microsoft.com/en-us/cpp/build/reference/running-lib?view=msvc-170).

Now when building our project that depends on ColorSpace we just provide the LIB file and the header file and ColorSpace is not needed to be compiled.

```cmd
cl /std:c++20 /EHsc /W4 colorspace_program.cpp module1.cpp /I..\dependencies\ColorSpace\src ../dependencies/ColorSpace/build/ColorSpace.lib

cl /std:c++20 /EHsc /W4 colorspace_program.cpp module1.cpp ColorSpace.lib /I..\dependencies\ColorSpace\src /link /LIBPATH:..\dependencies\ColorSpace\build
```

Again `/I` is to tell where the header files are. To "include" the LIB file we have two options. A) we add the LIB file into our build command by its full path, B) treat it similar to a header file, tell the linker where to look for and just add it's name next to CPP, OBJ files. I found method A) more succint.


Let's do a more substantial and complex library: ImGui at <https://github.com/ocornut/imgui>. It's an "immediate mode" UI library for C++ that's used a lot in rendering projects. We are going to use it's OpenGL backend for rendering and GLFW backend for windowing, which are extra dependencies that'll complicate the build process. Just assume that we have glfw under dependencies and it was build using a tool such as CMake with default settings therefore we have its headers and LIB file ready.

By the way, a trick to bring the library into your project's version control is to use `git submodule` command.


```cmd
cd project/dependencies
git submodule add https://github.com/ocornut/imgui.git

# OR if a specific branch is needed

git submodule add --branch docking https://github.com/ocornut/imgui.git
```

Now, whoever clones your project from GitHub also will clone the dependency libraries with it and your repo is not responsible of the dependency repo's files.

```cmd
cd project/dependencies/imgui
mkdir build
cl /c /EHsc /I. /I../glfw/include imgui.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp imgui_demo.cpp backends/imgui_impl_glfw.cpp backends/imgui_impl_opengl3.cpp /MD /Fobuild\

cd build
lib *.obj /out:imgui.lib
```

Again we use `/c` to just compile OBJ files. We include all necessary CPP files from ImGui. We choose `.\build\` as the output directory. 

3 extra dependency specific tweaks: 
1. Included current directory. `/I.` because some CPP files are not in the same folder as the rest of the source files. `backends/imgui_impl_glfw.cpp` includes `imgui.h` via `#include "imgui.h"` but that header is in the parent folder.
2. that we rely on another dependency, GLFW, whose headers we include.
3. we included `/MD` option which'll be explained very soon

Let's build our project `imgui_test.cpp` that uses ImGui. Figuring out exact build command will take some process. Start with the following which includes GLFW headers and lib (assume we have it)

```cmd
cl /std:c++20 /W4 /EHsc ^
    /I"../dependencies/imgui" /I"../dependencies/imgui/backends" ../dependencies/imgui/build/imgui.lib ^
    /I"../dependencies/glfw/include" ../dependencies/glfw/build/src/Release/glfw3.lib ^
    imgui_test.cpp
```

We get millions of errors, first one being: `LINK : warning LNK4098: defaultlib 'MSVCRT' conflicts with use of other libs; use /NODEFAULTLIB:library`. See [LNK4098](https://docs.microsoft.com/en-us/cpp/error-messages/tool-errors/linker-tools-warning-lnk4098?view=msvc-170). This is related to the previously mentioned `/MD` option which is related to the runtime libraries. See [/MD, /MT, /LD (Use Run-Time Library)](https://docs.microsoft.com/en-us/cpp/build/reference/md-mt-ld-use-run-time-library?view=msvc-170).

Please refer to [C runtime (CRT) and C++ Standard Library (STL) .lib files](https://docs.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=msvc-170) about runtime libraries. AFAIU, CRT has the implementations of lower-level functionalities required by STL plus some other stuff the operating system provides. "The C runtime Library (CRT) is the part of the C++ Standard Library that incorporates the ISO C standard library." See [Microsoft C runtime library (CRT) reference](https://docs.microsoft.com/en-us/cpp/c-runtime-library/c-run-time-library-reference?view=msvc-170) for stuff that are provided with CRT. As an example, `new`, `delete` operators, `malloc()`, `free()` functions are implemented in CRT [Memory allocation](https://docs.microsoft.com/en-us/cpp/c-runtime-library/memory-allocation?view=msvc-170)

`/MT` uses static version of the runtime library whereas `/MD` uses the dynamic version. (they also have `d` suffix to use the debug versions, e.g. `/MDd`). VS documentations recommends to use `/MD` all the time so I won't rebel against that. 

The warning above was due to the fact that different components included in the final link, i.e. glfw, imgui, and our code, have different options used for the choice of runtime library. The suggestion to use `/NODEFAULTLIB:library` is to prevent linking any library (including runtime library) automatically.

However, if everything uses `/MD` then every component uses the same version DLL provided by operating system and there won't be inconsistencies among them. So, let's add `/MD` into our build command.

```cmd
cl /std:c++20 /W4 /EHsc ^
  /I"../dependencies/imgui" /I"../dependencies/imgui/backends" ../dependencies/imgui/build/imgui.lib ^
  /I"../dependencies/glfw/include" ../dependencies/glfw/build/src/Release/glfw3.lib ^
  /MD ^
  imgui_test.cpp
```

Suddenly number of warnings/errors dropped from million to 20! An example warning that was gone was: `LINK : warning LNK4286: symbol 'strncmp' defined in 'libucrt.lib(strncmp.obj)' is imported by 'glfw3.lib(context.obj)'` This should make more sense now. Documentation says `/MT` "Statically links the native CRT startup into your code." which is `libcmt.lib`. When not using `/MD` default `/MT` was used, but probably GLFW was build with `/MD` option and included the `strncmp` definition already.

With `/MD` the first error we get is `imgui_test.obj : error LNK2019: unresolved external symbol __imp_glClear referenced in function main`. See [LNK2019](https://docs.microsoft.com/en-us/cpp/error-messages/tool-errors/linker-tools-error-lnk2019?view=msvc-170). This means, we are using a function, which is probably declared in one of the header files, but no OBJ file provided its definition/implementation.

The procedure to fix these LNK2019 errors is to google the name of the missing symbol, and find the library that implemented that symbol in MS documents. :-)

But this first one is a bit cryptic. You can tell that `glClear` is an OpenGL API function, and just add OpenGL32.lib as a library to our build command: `/MD OpenGL32.lib ^`. That is the OpenGL 1.1 library provided in Windows. (You might ask, what? How is that v1.1? Aren't we using 4.6? But that's a topic for another blog post. Just know that we are using OpenGL 1.1 to load OpenGL 4.6 functions later at runtime.)


Now we have 17 errors. All errors from `imgui_test.obj` are gone, all of which were related to OpenGL API calls. Next error is `glfw3.lib(win32_window.obj) : error LNK2019: unresolved external symbol __imp_CreateBitmap referenced in function createIcon`. Google `CreateBitmap` which brings us to [CreateBitmap function (wingdi.h)](https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createbitmap) Scroll down and see "Library	`Gdi32.lib`". OK, now we know which other LIB to add: `/MD OpenGL32.lib Gdi32.lib ^`

Nice, 4 errors were left. First one is `glfw3.lib(win32_window.obj) : error LNK2019: unresolved external symbol __imp_DragQueryFileW referenced in function windowProc`. We know the drill. Google `DragQueryFileW`. Get to [DragQueryFileW function (shellapi.h)](https://docs.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-dragqueryfilew). See that it uses `	Shell32.lib`. Build command has following line now: `/MD OpenGL32.lib Gdi32.lib Shell32.lib ^`

Neat! We don't have any errors anymore and it generated the functioning executable.

```cmd
cl /std:c++20 /W4 /EHsc ^
  /I"../dependencies/imgui" /I"../dependencies/imgui/backends" ../dependencies/imgui/build/imgui.lib ^
  /I"../dependencies/glfw/include" ../dependencies/glfw/build/src/Release/glfw3.lib ^
  /MD OpenGL32.lib Gdi32.lib Shell32.lib ^
  imgui_test.cpp
```

This was mouthful but we build a project that has complex dependencies, and we have a methodology to find necessary the LIB files provided by Windows.


### Libraries with some building support such as CMake

Above we assumed that we have GLFW library as dependency, but we didn't mention how it was built. I'm not going to dive into details of CMake but just going to provide commands to demonstrate building a dependency that has CMake support.

First add GLFW as a submodule dependency

```cmd
cd project/dependencies
git submodule add https://github.com/glfw/glfw.git
```

then create a build folder under glfw

```cmd
cd glfw
mkdir build
cd build
```

optionally list configuration options for this project

```cmd
cmake .. -L
```

for example following options are printed with `-L` call

```txt
BUILD_SHARED_LIBS:BOOL=OFF
CMAKE_CONFIGURATION_TYPES:STRING=Debug;Release;MinSizeRel;RelWithDebInfo
CMAKE_INSTALL_PREFIX:PATH=C:/Program Files (x86)/GLFW
GLFW_BUILD_DOCS:BOOL=ON
GLFW_BUILD_EXAMPLES:BOOL=ON
GLFW_BUILD_TESTS:BOOL=ON
GLFW_BUILD_WIN32:BOOL=ON
GLFW_INSTALL:BOOL=ON
GLFW_LIBRARY_TYPE:STRING=
GLFW_USE_HYBRID_HPG:BOOL=OFF
USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=ON
```

Generate the build system with our choice of configuration settings. For example we don't need examples, documentation, tests. And we don't want to install anything.

```cmd
cmake .. -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=Off -DGLFW_BUILD_TESTS=OFF -DGLFW_INSTALL=OFF
```

This creates a Visual Studio solution for us (because that's the default setting for me). Though we don't need to open the IDE, we can let the CMake to build the project from command-line via following

```cmd
cmake --build . --config Release
cmake --build . --config Debug
```

First one builds the optimized Release library binaries, and the latter build the unoptimized Debug version that has debugging symbols.

If everything goes well we should see the LIB files in

```txt
project/dependencies/glfw/build/src/Release/glfw3.lib
project/dependencies/glfw/build/src/Debug/glfw3.lib
```

Use the version that fits the purpose.


## Compiler Warnings Reprise - No warnings from dependencies

Sometimes the author of a library we use does not care about the compiler warnings. This can become annoying if the warning is thrown due to a code in its header files. Then we'll see the warning each time we build.

It's possible to tell the compiler "this is my code and these are external code. I want detailed warnings about my code but no warnings about those external code". `/external:I` tells the location of external code and `/external:W?` tells the level of warnings from external code.

In my project structure all external logic is under `dependencies/` folder there fore I use following line in my build command.

```cmd
    /external:I"../dependencies" /external:W0
```

## Optimization and Debug Symbols

Remember CMake provides Release and Debug build options. (I think standard VS solutions provide the same too.) We can provide optimization options from command line too. See [/O options \(Optimize code\) \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/o-options-optimize-code?view=msvc-170)

Basically if you want an optimized Release version use `/O2` option. See [/O1, /O2 (Minimize Size, Maximize Speed)](https://docs.microsoft.com/en-us/cpp/build/reference/o1-o2-minimize-size-maximize-speed?view=msvc-170).

The default `/O` options is `/Od` which is the debugging option. See [/Od (Disable (Debug))](https://docs.microsoft.com/en-us/cpp/build/reference/od-disable-debug?view=msvc-170)

Don't be lazy use optimization options because they really make a difference in code execution speed! (It produces shorter assembily code that'll be ran in fewer CPU cycles.)

Other useful option related debugging is about generating debug information such as list of symbols. See [/DEBUG (Generate Debug Info)](https://docs.microsoft.com/en-us/cpp/build/reference/debug-generate-debug-info?view=msvc-170).

[/Z7, /Zi, /ZI (Debug Information Format)](https://docs.microsoft.com/en-us/cpp/build/reference/z7-zi-zi-debug-information-format?view=msvc-170) says that if we use "`/Zi` option [it] produces a separate PDB file that contains all the symbolic debugging information for use with the debugger". (I might write about a VS Code setup with debugging option for C++ later.)

So, either use `/O2` or `/Zi`.

## Summary

Assuming all dependencies are under `dependencies/` folder we have the following build command structure. 

* Start with `cl /std:c++20 /W4 /external:I../dependencies /external:W0 /EHsc /MD`
* If release add `/O2` if debug add `/Zi` options
* Then for each dependency add necessary inclusion folders via `/I` for its headers and their LIB file. (Either using full path or using filename only with `/LIBPATH:` providing folder to libs) Also bring release vs. debug verions of dependencies based on our choice.
* If necessary add LIBs that comes with Windows
* Add CPP files from our project.

At this point it's better to have a `build.bat` file with our build command in it.


```bat
// build_release.bat
cl /std:c++20 /W4 /external:I../dependencies /external:W0 /EHsc /MD /O2 /I../dependencies/imgui /I../dependencies/imgui/backends ../dependencies/imgui/build/imgui.lib /I../dependencies/glfw/include ../dependencies/glfw/build/src/Release/glfw3.lib OpenGL32.lib Gdi32.lib Shell32.lib imgui_test.cpp module1.cpp
```

```bat
// build_debug.bat
cl /std:c++20 /W4 /external:I../dependencies /external:W0 /EHsc /MD /Zi /I../dependencies/imgui /I../dependencies/imgui/backends ../dependencies/imgui/build/imgui.lib /I../dependencies/glfw/include ../dependencies/glfw/build/src/Debug/glfw3.lib OpenGL32.lib Gdi32.lib Shell32.lib imgui_test.cpp module1.cpp
```

They can even output temp files and final executable into different folders based on the taste. :-)

And remember we can build library dependencies themselves via

```cmd
cl /c /EHsc src\*.cpp /Fobuild\ 
lib build/*.obj /out:build\MyLibrary.lib
```

and optionally use `/Zi` or `/O2` at compilation step.

## Conclusion

It'll be even better to have a `Makefile` and more better to have a `CMakeLists.txt` but my goal with this post was to learn the VS compiler options. I might write about a practical CMake based project structure later.

One does not need to deal with any of these and just use Visual Studio IDE and apply settings there. 

However, I believe, it's always good to know what's happening under the hood. And learning concepts apply to different platforms and compilers too. I should learn the gcc and clang counterparts of all the options mentioned in this post. 

And sometimes, when projects become very big (thousands of dependencies, hunderds of software developers working on the same codebase) it might not be practical to rely on the IDE and some advanced build tool (bazel, buck, ninja, conan etc.) needed and we've to provide options via config files.

So, a one time investment on understanding building projects purely from command-line options provides concepts to utilize for long time!

## References

* [Compiler Options \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options?view=msvc-170)
* All compiler options [Compiler Options Listed by Category \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-170)
* All linker options: [MSVC Linker options \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/linker-options?view=msvc-170)
* [Use the Microsoft C\+\+ toolset from the command line \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170)
* [Compiler Command-Line Syntax \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/compiler-command-line-syntax?view=msvc-170)
* [Order of CL Options \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/order-of-cl-options?view=msvc-170) "Options can appear anywhere on the CL command line, except for the /link option, which must occur last."
