# Dependencies

One way to add them is to use Git submodules. See: https://training.github.com/downloads/submodule-vs-subtree-cheat-sheet/
Example:

```
git submodule add https://github.com/blend2d/blend2d
```

Submodules can be cloned with the repo via 

```
git clone --recurse-submodules https://github.com/vug/graphics-stuff.git
```

or pulled later via

```
git submodule update --init --recursive
```

## Blend2D

https://blend2d.com/

* `cd blend2d`
* `mkdir build`
* `cd build`
* `cmake .. -DBLEND2D_STATIC=true`
* `cmake --build . --config Release`
* example project compilation: `cl /W4 /EHsc main.cpp /I../dependencies/blend2d/src ../dependencies/blend2d/build/Release/blend2d.lib /MD`