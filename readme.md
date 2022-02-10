## EzNtp

简单的NTP手动同步软件，使用C++和FLTK编写。

编译环境: Mingw GCC 11.2.0 Windows 10 x64

## 命令行版

    g++ ./main.cpp -o main.exe -lwsock32

## FLTK版

自行更换FLTK地址

    g++ -I/c/some/fltk/include -I/c/some/fltk/include/FL/images -mwindows -DWIN32 -DUSE_OPENGL32 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -o 'main' './main.cpp' -mwindows /c/some/fltk/lib/libfltk.a -ldl -lole32 -luuid -lcomctl32 -lwsock32


## 许可

AGPL-3.0


