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

## Raylib

* <https://www.raylib.com/>
  * <https://www.raylib.com/examples.html>
* <https://github.com/raysan5/raylib>
  * <https://github.com/raysan5/raylib/releases> binaries can be downloaded from here
* <https://github.com/raysan5/raylib/wiki#development-platforms>
  * <https://github.com/raysan5/raylib/wiki/Working-on-Windows>

```cmd
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

## GLFW

* <https://www.glfw.org/>

```cmd
cd dependencies
git submodule add https://github.com/glfw/glfw.git
cd glfw
mkdir build
cd build
cmake .. -L
cmake .. -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=Off -DGLFW_BUILD_TESTS=OFF -DGLFW_INSTALL=OFF
cmake --build . --config Release
cmake --build . --config Debug
```

options

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

outputs

* glfw.vcxproj -> C:\Users\veliu\Documents\repos\graphics-stuff\dependencies\glfw\build\src\Release\glfw3.lib
* glfw.vcxproj -> C:\Users\veliu\Documents\repos\graphics-stuff\dependencies\glfw\build\src\Debug\glfw3.lib

```cmd
cl /std:c++20 /W4 /external:I"../dependencies" /external:W0 /I"../dependencies/glfw/include" ../dependencies/glfw/build/src/Release/glfw3.lib Opengl32.lib User32.lib Gdi32.lib Shell32.lib /MD /EHsc glfw_01.cpp
```

`/Zi` for debug symbols

## Glad

v1

* <https://glad.dav1d.de/>
* gl: Version 4.6
* no extensions
* profile: core
* Generate a loader: checked (tutorial says `--no-loader` though)
* permalink: <https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D4.6&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=core&loader=on>

v2

* <https://gen.glad.sh/>
* gl: Version 4.6
* profile: core
* no other options (could try `loader` and `header only`)
* permalink: <https://gen.glad.sh/#generator=c&api=gl%3D4.6&profile=gl%3Dcore%2Cgles1%3Dcommon>

Could have tried "v2 but submodule dependency", but not necessarily needed.

* download glad.zip
* extract it into `dependencies/glad`
* `cl /std:c++20 /W4 /I"include" /c /EHsc src\glad.c /MD`
* `lib glad.obj /out:glad.lib`
* `del glad.obj`

```cmd
cl /std:c++20 /W4 /external:I"../dependencies" /external:W0 /I"../dependencies/glfw/include" ../dependencies/glfw/build/src/Release/glfw3.lib /I"../dependencies/glad/include" ../dependencies/glad/glad.lib Opengl32.lib User32.lib Gdi32.lib Shell32.lib /MD /EHsc glfw_02.cpp
```

## GLM

* <https://github.com/g-truc/glm>

```cmd
git submodule add https://github.com/g-truc/glm.git
cmake .. -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF -DGLM_TEST_ENABLE=OFF -DBUILD_STATIC_LIBS=ON
```

options

```txt
BUILD_SHARED_LIBS:BOOL=ON
BUILD_STATIC_LIBS:BOOL=ON
BUILD_TESTING:BOOL=ON
CMAKE_CONFIGURATION_TYPES:STRING=Debug;Release;MinSizeRel;RelWithDebInfo
CMAKE_INSTALL_PREFIX:PATH=C:/Program Files (x86)/glm
GLM_DISABLE_AUTO_DETECTION:BOOL=OFF
GLM_QUIET:BOOL=OFF
GLM_TEST_ENABLE:BOOL=ON
GLM_TEST_ENABLE_CXX_11:BOOL=OFF
GLM_TEST_ENABLE_CXX_14:BOOL=OFF
GLM_TEST_ENABLE_CXX_17:BOOL=OFF
GLM_TEST_ENABLE_CXX_20:BOOL=OFF
GLM_TEST_ENABLE_CXX_98:BOOL=OFF
GLM_TEST_ENABLE_FAST_MATH:BOOL=OFF
GLM_TEST_ENABLE_LANG_EXTENSIONS:BOOL=OFF
GLM_TEST_ENABLE_SIMD_AVX:BOOL=OFF
GLM_TEST_ENABLE_SIMD_AVX2:BOOL=OFF
GLM_TEST_ENABLE_SIMD_SSE2:BOOL=OFF
GLM_TEST_ENABLE_SIMD_SSE3:BOOL=OFF
GLM_TEST_ENABLE_SIMD_SSE4_1:BOOL=OFF
GLM_TEST_ENABLE_SIMD_SSE4_2:BOOL=OFF
GLM_TEST_ENABLE_SIMD_SSSE3:BOOL=OFF
GLM_TEST_FORCE_PURE:BOOL=OFF
```

```cmd
cl /std:c++20 /W4 /external:I"../dependencies" /external:W0 /I"../dependencies/glm" ../dependencies/glm/build/glm/Release/glm_static.lib /EHsc glm_test.cpp
```

## ImGui

* <https://github.com/ocornut/imgui>
* docking branch: <https://github.com/ocornut/imgui/tree/docking>

```cmd
git submodule add --branch docking https://github.com/ocornut/imgui.git
cd imgui
mkdir build
cl /std:c++20 /c /EHsc /I"." /I"../glfw/include" imgui.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp imgui_demo.cpp backends/imgui_impl_glfw.cpp backends/imgui_impl_opengl3.cpp /MD /Fo.\build\
cd build
lib *.obj /out:imgui.lib
del *.obj
```

```cmd
cl /std:c++20 /W4 /external:I"../dependencies" /external:W0 /I"../dependencies/imgui" /I"../dependencies/imgui/backends" ../dependencies/imgui/build/imgui.lib /I"../dependencies/glfw/include" ../dependencies/glfw/build/src/Release/glfw3.lib Opengl32.lib Gdi32.lib Shell32.lib /MD /EHsc imgui_test.cpp
```
