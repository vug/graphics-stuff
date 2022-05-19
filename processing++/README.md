# Processing++

Original Processing is Java. There are also JS and Python versions. I'll make a very simplified C++ version, because, why not.

After building `minifb` and `blend2d` dependencies, following is build/compile and link the project that depends on minifb for window creating and pixel buffer rendering.

```shell
cl /W4 /EHsc main.cpp processing.cpp sketch1.cpp /I../dependencies/minifb/include ../dependencies/minifb/build/Release/minifb.lib User32.lib OpenGL32.lib Gdi32.lib Winmm.lib /I../dependencies/blend2d/src ../dependencies/blend2d/build/Release/blend2d.lib /MD
```

Or use the batch file

```
build_sketch.bat references\rectMode_02.cpp
```

which generates `rectMode_02.exe`.
