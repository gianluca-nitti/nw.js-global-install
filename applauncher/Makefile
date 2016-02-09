CFLAGS= -Os -fdata-sections -ffunction-sections -Wall -Werror -Wl,--gc-sections

all: win32 linux32 linux64

.PHONY: dir win32 linux32 linux64 clean

dir:
	mkdir -p build/win32 build/linux32 build/linux64

win32: dir
	i686-w64-mingw32-gcc main.c $(CFLAGS) -o build/win32/applauncher.exe
	i686-w64-mingw32-strip build/win32/applauncher.exe

linux32: dir
	gcc -m32 main.c $(CFLAGS) -o build/linux32/applauncher
	strip build/linux32/applauncher

linux64: dir
	gcc main.c $(CFLAGS) -o build/linux64/applauncher
	strip build/linux64/applauncher

clean:
	rm -rf build/
