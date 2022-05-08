# No dependency bitmap generation

## Build and run instructions for Visual Studio + ImageMagick

Not using a build system such as CMake because there are no dependencies

* Run "x64 Native Tools Command Prompt for VS 2022"
* Go to this folder
  * `cd graphics-stuff\bitmaps`
* Compile object file and link it to executable
  * `cl /W4 /EHsc colors.cpp`
* Save output into a PPM file
  * `colors.exe > colors.ppm`
* Make sure [ImageMagick](https://imagemagick.org/script/index.php) is installed
* Convert PPM to PNG
  * `magick colors.ppm -compress none color.png`

## Reference

Idea from

* [mrozycki/bitmaps](https://github.com/mrozycki/bitmaps)
* Presentation: [Bitmaps\! Or "Images and Animations Without Linking External Libraries" \- Mariusz Różycki C\+\+ on Sea \- YouTube](https://www.youtube.com/watch?v=2OZR9_appAA)
