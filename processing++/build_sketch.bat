@echo off
cl /W4 /EHsc main.cpp processing.cpp %1 /I../dependencies/minifb/include ../dependencies/minifb/build/Release/minifb.lib User32.lib OpenGL32.lib Gdi32.lib Winmm.lib /I../dependencies/blend2d/src ../dependencies/blend2d/build/Release/blend2d.lib /I../dependencies/ColorSpace/src ../dependencies/ColorSpace/bld/ColorSpace.lib /MD /link /out:%~n1.exe
echo output: %~n1.exe