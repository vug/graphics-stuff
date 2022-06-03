# Builds

```cmd
cl /std:c++20 /W4 /external:I"../dependencies" /external:W0 /I"../dependencies/glfw/include" ../dependencies/glfw/build/src/Release/glfw3.lib /I"../dependencies/glad/include" ../dependencies/glad/glad.lib /I"../dependencies/glm" ../dependencies/glm/build/glm/Release/glm_static.lib Opengl32.lib User32.lib Gdi32.lib Shell32.lib /MD /EHsc icosahedron.cpp
```
