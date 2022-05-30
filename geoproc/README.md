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

## `vertex_split` study

Observed that `vl` and `vr` needs to be neighbors of `v`, the vertex that is being splited. 

Before split

![before](split_original.png)

After split

![after](split_splitted.png)