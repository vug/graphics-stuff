# Blend2D

https://blend2d.com/

* `cd blend2d`
* `mkdir build`
* `cd build`
* `cmake .. -DBLEND2D_STATIC=true`
* `cmake --build . --config Release`
* example project compilation: `cl /W4 /EHsc main.cpp /I../dependencies/blend2d/src ../dependencies/blend2d/build/Release/blend2d.lib /MD`