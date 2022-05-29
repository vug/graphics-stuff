# Geometry Processing

Using OpenMesh

```cmd
cl /std:c++20 /W4 /external:I"../dependencies" /external:W0 /I"../dependencies/OpenMesh/src" ../dependencies/OpenMesh/build/Build/lib/OpenMeshCore.lib /MD /D_USE_MATH_DEFINES /EHsc cube.cpp
```

or

```cmd
cl /std:c++20 /W4 /external:I"../dependencies" /external:W0 ^
  /I"../dependencies/OpenMesh/src" ../dependencies/OpenMesh/build/Build/lib/OpenMeshCore.lib /MD ^
  /D_USE_MATH_DEFINES ^
  /EHsc cube.cpp
```