# 2D Drawing on Windows using GDI+

## drawing_a_line.cpp

* [Drawing a Line \- Win32 apps \| Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-drawing-a-line-use)
* libraries to link
  * [c\+\+ \- MFC Static Linking Unresolved External Symbol \- Stack Overflow](https://stackoverflow.com/questions/20694408/mfc-static-linking-unresolved-external-symbol) For each missing symbol "search its name, find MSDN entry, see the `.lib` file required".
  * [GetStockObject function \(wingdi\.h\) \- Win32 apps \| Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-getstockobject?redirectedfrom=MSDN) -> `Gdi32.lib`
  * [GetMessageA function \(winuser\.h\) \- Win32 apps \| Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmessagea) -> `User32.lib`
* `stdafx.h`
  * [c\+\+ \- \(\(Code::Blocks\)\) \- fatal error: stdafx\.h: No such file or directory \- Stack Overflow](https://stackoverflow.com/questions/38470207/codeblocks-fatal-error-stdafx-h-no-such-file-or-directory)
  * Looks like it's only used by Visual Studio. Comment it out in other development environments
* Linking against libraries
  * [CL Invokes the Linker \| Microsoft Docs](https://docs.microsoft.com/en-us/cpp/build/reference/cl-invokes-the-linker?view=msvc-170)
  * `cl /W4 /EHsc drawing_a_line.cpp gdi32.lib User32.lib`
