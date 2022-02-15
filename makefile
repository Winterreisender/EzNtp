# For Windows & Mingw/GCC

RC_FILES = ./assets/main_gui.rc
RC_OUTPUT = ./build/main_gui.res

SOURCE_FILES = ./src/main_gui.cpp $(RC_OUTPUT)
OUTPUT = ./build/main_gui.exe

CC = g++

COMMON_BUILD_FLAGS = -Wall -fexec-charset=UTF-8 -finput-charset=UTF-8 
DEBUG_BUILD_FLAGS = -g
RELEASE_BUILD_FLAGS = -O2 -static -static-libgcc -static-libstdc++ #-lwinpthread #静态链接Mingw的库便于分发,否则应当附带dll

#PACKAGES
FLTK = -I./packages/fltk/include -I./packages/fltk/include/FL/images -mwindows -DWIN32 -DUSE_OPENGL32 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -mwindows ./packages/fltk/lib/libfltk.a  -lole32 -luuid -lcomctl32 #-ldl
SOCKET =  -lwsock32
PACKAGE_FLAGS = $(FLTK) $(SOCKET)


debug : $(SOURCE_FILES)
	$(CC) $(DEBUG_BUILD_FLAGS) $(SOURCE_FILES) $(COMMON_BUILD_FLAGS) $(PACKAGE_FLAGS) -o $(OUTPUT) && $(OUTPUT)

run: $(OUTPUT)
	$(OUTPUT)

release: $(SOURCE_FILES)
	$(CC) $(RELEASE_BUILD_FLAGS) $(SOURCE_FILES) $(COMMON_BUILD_FLAGS) $(PACKAGE_FLAGS) -o $(OUTPUT)

clean: ./build/*
	@rm ./build/* -f && echo cleaned.

compile_rc:
	windres -i $(RC_FILES) --input-format=rc -o $(RC_OUTPUT) -O coff

# 配置32位Mingw过于麻烦,且需要重新编译FLTK,故放弃
compile_rc_32:
	windres -i $(RC_FILES) --input-format=rc -o $(RC_OUTPUT) -F pe-i386 -O coff

dumpbin:
	dumpbin /DEPENDENTS $(OUTPUT)

