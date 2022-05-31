# Dependencies

One way to add them is to use Git submodules. See: <https://training.github.com/downloads/submodule-vs-subtree-cheat-sheet/>
Example:

```cmd
git submodule add https://github.com/blend2d/blend2d
```

Submodules can be cloned with the repo via

```cmd
git clone --recurse-submodules https://github.com/vug/graphics-stuff.git
```

or pulled later via

```cmd
git submodule update --init --recursive
```

## Blend2D

<https://blend2d.com/>

* `cd blend2d`
* `mkdir build`
* `cd build`
* `cmake .. -DBLEND2D_STATIC=true`
* `cmake --build . --config Release`
* example project compilation: `cl /W4 /EHsc main.cpp /I../dependencies/blend2d/src ../dependencies/blend2d/build/Release/blend2d.lib /MD`

## OpenMesh

[OpenMesh \- Computer Graphics and Multimedia](https://www.graphics.rwth-aachen.de/software/openmesh/)

Bring repo as dependency and get sub-dependencies

```cmd
cd dependencies
git submodule add https://gitlab.vci.rwth-aachen.de:9000/OpenMesh/OpenMesh.git
cd ..    # (?)
git submodule update --init --recursive
```

```cmd
cd dependencies\OpenMesh\
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_APPS=OFF
```

* Apps can be downloaded via binary distribution, didn't want to bring QT as a dependency just to have those apps.
* Also, probably `CMAKE_BUILD_TYPE=Release` is not needed
* Debug build `cmake --build .`
  * generates `OpenMeshCored.lib` and `OpenMeshToolsd.lib`
* Release build `cmake --build . --config Release`
  * generates `OpenMeshCore.lib` and `OpenMeshTools.lib` which are significantly smaller

Example compilation

```cmd
cl /W4 /external:I"../dependencies" /external:W0 /I"../dependencies/OpenMesh/src" ../dependencies/OpenMesh/build/Build/lib/OpenMeshCore.lib /MD /D_USE_MATH_DEFINES /EHsc cube.cpp
```

# Raylib

* https://www.raylib.com/
  * https://www.raylib.com/examples.html
* https://github.com/raysan5/raylib
  * https://github.com/raysan5/raylib/releases binaries can be downloaded from here
* https://github.com/raysan5/raylib/wiki#development-platforms
  * https://github.com/raysan5/raylib/wiki/Working-on-Windows

```
cd dependencies
git submodule add https://github.com/raysan5/raylib
cd raylib
mkdir build
cd build
cmake .. -L
cmake .. -DOPENGL_VERSION=4.3 
cmake --build . --config Release
cmake --build . --config Debug
```

default OpenGL Version was 3.3. See `CMakeOptions.txt`: `enum_option(OPENGL_VERSION "OFF;4.3;3.3;2.1;1.1;ES 2.0" "Force a specific OpenGL Version?")`

* `raylib.lib` file is in `dependencies\raylib\build\raylib\Release`
* examples are in `dependencies\raylib\build\examples\Release`

Example compilation

```cmd
cl /W4 /external:I"../dependencies" /external:W0 /I"../dependencies/raylib/build/raylib/include" ../dependencies/raylib/build/raylib/Release/raylib.lib User32.lib gdi32.lib Shell32.lib Winmm.lib /MD /EHsc raylib.cpp
```

`/Zi` for debug symbols
