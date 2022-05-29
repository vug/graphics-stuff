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
cl /W4 /EHsc cube.cpp /I"../dependencies/OpenMesh/src" ../dependencies/OpenMesh/build/Build/lib/OpenMeshCore.lib /MD /D_USE_MATH_DEFINES
```
