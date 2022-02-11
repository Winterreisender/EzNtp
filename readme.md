## EzNtp

十分简单的Windows手动校时软件，使用C++,socket,Win32 API,NTP协议和FLTK编写。

![截图](screenshot.png)

编译环境: Mingw GCC 11.2.0 Windows 10 x64

## 命令行版

    g++ ./main_cli.cpp -o main.exe -lwsock32

## FLTK版

#### 使用mingw32-make

将编译好的fltk放到`./packages/fltk/`文件夹中

执行:

```bash
mingw32-make
```

#### 手动编译

以FLTK库的位置在`/c/some/fltk`为例:

    g++ -I/c/some/fltk/include -I/c/some/fltk/include/FL/images -mwindows -DWIN32 -DUSE_OPENGL32 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -o 'main' './main_gui.cpp' -mwindows /c/some/fltk/lib/libfltk.a -ldl -lole32 -luuid -lcomctl32 -lwsock32

## 许可

AGPL-3.0

![GNU AGPL Logo](https://www.gnu.org/graphics/agplv3-155x51.png)
