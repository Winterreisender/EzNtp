SOURCE_FILES = ./src/main_gui.cpp

CC = g++

COMMON_BUILD_FLAGS = -Wall -fexec-charset=UTF-8 -finput-charset=UTF-8
DEBUG_BUILD_FLAGS = -g
RELEASE_BUILD_FLAGS = -O2

#PACKAGES
FLTK = -I./packages/fltk/include -I./packages/fltk/include/FL/images -mwindows -DWIN32 -DUSE_OPENGL32 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -mwindows ./packages/fltk/lib/libfltk.a -ldl -lole32 -luuid -lcomctl32
SOCKET =  -lwsock32
PACKAGE_FLAGS = $(FLTK) $(SOCKET)

OUT_PUT = ./build/main_gui.exe

all : $(SOURCE_FILES)
	$(CC) $(SOURCE_FILES) $(COMMON_BUILD_FLAGS) $(PACKAGE_FLAGS) -o $(OUT_PUT) && $(OUT_PUT)

run: $(OUT_PUT)
	$(OUT_PUT)

build: $(SOURCE_FILES)
	$(CC) $(DEBUG_BUILD_FLAGS)   $(SOURCE_FILES) $(COMMON_BUILD_FLAGS) $(PACKAGE_FLAGS) -o $(OUT_PUT)

release: $(SOURCE_FILES)
	$(CC) $(RELEASE_BUILD_FLAGS) $(SOURCE_FILES) $(COMMON_BUILD_FLAGS) $(PACKAGE_FLAGS) -o $(OUT_PUT)

clean: ./build/*
	@rm ./build/* -f && echo cleaned.