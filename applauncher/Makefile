CFLAGS=--std=gnu99 -Os -fdata-sections -ffunction-sections -Wall -Werror -Wl,--gc-sections
GTKFLAGS=$(shell pkg-config --cflags gtk+-2.0) -lgtk-x11-2.0 -lgdk-x11-2.0 -lgdk_pixbuf-2.0 -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -lglib-2.0 -lXext -lX11 -lm

all: win32 linux32 linux64

.PHONY: dir win32 linux32 linux64 clean

dir:
	mkdir -p build/win32 build/linux32 build/linux64

win32: dir
	i686-w64-mingw32-windres res-win.rc -O coff -o resources.obj
	i686-w64-mingw32-gcc main.c resources.obj $(CFLAGS) -mwindows -o build/win32/applauncher.exe
	i686-w64-mingw32-strip build/win32/applauncher.exe

linux32: dir
	gcc -m32 main.c $(CFLAGS) $(GTKFLAGS) -o build/linux32/applauncher
	strip build/linux32/applauncher

linux64: dir
	gcc main.c $(CFLAGS) $(GTKFLAGS) -o build/linux64/applauncher
	strip build/linux64/applauncher

clean:
	rm -rf build/
