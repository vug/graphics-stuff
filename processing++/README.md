# Processing++

Original Processing is Java. There are also JS and Python versions. I'll make a very simplified C++ version, because, why not.

build ColorSpace via

```
cd dependencies\ColorSpace
mkdir bld
cl /c /EHsc src\ColorSpace.cpp src\Comparison.cpp src\Conversion.cpp
lib ColorSpace.obj Comparison.obj Conversion.obj /out:bld\ColorSpace.lib
```

Build `minifb` and `blend2d` via CMake.

After building dependencies, following is build/compile and link the project that depends on minifb for window creating and pixel buffer rendering.

```shell
cl /W4 /EHsc main.cpp processing.cpp sketch1.cpp /I../dependencies/minifb/include ../dependencies/minifb/build/Release/minifb.lib User32.lib OpenGL32.lib Gdi32.lib Winmm.lib /I../dependencies/blend2d/src ../dependencies/blend2d/build/Release/blend2d.lib /MD
```

After introduction of ColorSpace


```shell
cl /W4 /EHsc main.cpp processing.cpp sketch1.cpp ^
  User32.lib OpenGL32.lib Gdi32.lib Winmm.lib ^
  /I../dependencies/minifb/include ../dependencies/minifb/build/Release/minifb.lib ^
  /I../dependencies/blend2d/src ../dependencies/blend2d/build/Release/blend2d.lib ^
  /I../dependencies/ColorSpace/src ../dependencies/ColorSpace/bld/ColorSpace.lib ^
  /MD
```

Or use the batch file

```
build_sketch.bat references\rectMode_02.cpp
```

which generates `rectMode_02.exe`.
