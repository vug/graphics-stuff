# Processing++

Original Processing is Java. There are also JS and Python versions. I'll make a very simplified C++ version, because, why not.

After building `minifb` dependency, following is build/compile and link the project that depends on minifb for window creating and pixel buffer rendering.

```
cl /W4 /EHsc main.cpp /I../dependencies/minifb/include ../dependencies/minifb/build/Release/minifb.lib User32.lib OpenGL32.lib Gdi32.lib Winmm.lib /MD
```