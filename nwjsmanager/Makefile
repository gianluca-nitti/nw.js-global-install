APPNAME=nwjsmanager

SRC=*.c
SRC_WINONLY=res/gui_compiled_win.c res/resources.obj win-only/junzip.c win-only/unzip.c win-only/IsWow64.c
SRC_LINUXONLY=res/gui_compiled_linux.c linux-only/untar.c
GUIS=./res/gui/*.led

CC_LINUX=gcc
CC_WIN=i686-w64-mingw32-gcc
WINDRES=i686-w64-mingw32-windres
STRIP_LINUX=strip
STRIP_WIN=i686-w64-mingw32-strip

CFLAGS=--std=gnu99 -Os -fdata-sections -ffunction-sections -Wall -Werror -Wl,--gc-sections

GTK_LIBS=-lgtk-x11-2.0 -lgdk-x11-2.0 -lgdk_pixbuf-2.0 -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -lglib-2.0 -lXext -lX11 -lm

INCS_WIN32=-Ilib/win32/iup/include -Ilib/src/jsmn-src -Ilib/src/semver.c-src -DCURL_STATICLIB -Ilib/win32/curl/include -Ilib/win32/zlib/include
INCS_LINUX32=-Ilib/linux32/iup/include -Ilib/src/jsmn-src -Ilib/src/semver.c-src -DCURL_STATICLIB -Ilib/linux32/curl/include -Ilib/linux32/zlib/include
INCS_LINUX64=-Ilib/linux64/iup/include -Ilib/src/jsmn-src -Ilib/src/semver.c-src -DCURL_STATICLIB -Ilib/linux64/curl/include -Ilib/linux64/zlib/include

LIBS_WIN32=lib/win32/iup/libiup.a -lgdi32 -lcomdlg32 -lcomctl32 -luuid -loleaut32 -lole32 lib/win32/libjsmn.a lib/win32/libsemver.a -DCURL_STATICLIB lib/win32/curl/lib/libcurl.a -lws2_32 lib/win32/zlib/lib/libz.a
LIBS_LINUX32=lib/linux32/iup/libiup.a $(GTK_LIBS) lib/linux32/libjsmn.a lib/linux32/libsemver.a -DCURL_STATICLIB lib/linux32/curl/lib/libcurl.a lib/linux32/zlib/lib/libz.a
LIBS_LINUX64=lib/linux64/iup/libiup.a $(GTK_LIBS) lib/linux64/libjsmn.a lib/linux64/libsemver.a -DCURL_STATICLIB lib/linux64/curl/lib/libcurl.a lib/linux64/zlib/lib/libz.a

all: win32 linux32 linux64 debug

.PHONY: dir ledc win32 linux32 linux64 clean test

dir:
	mkdir -p build

ledc:
	./tools/ledc -o res/gui_compiled_win.c $(GUIS)
	./tools/im_copy res/icon.ico res/icon.led LED
	patch res/icon.led res/led_patcher.patch
	./tools/ledc -o res/gui_compiled_linux.c $(GUIS) res/icon.led

win32: dir ledc
	rm -rf build/$@
	mkdir -p build/$@
	$(WINDRES) res/res.rc -O coff -o res/resources.obj
	$(CC_WIN) $(SRC_WINONLY) $(SRC) $(INCS_WIN32) $(LIBS_WIN32) $(CFLAGS) -mno-ms-bitfields -Wl,-subsystem,windows -o build/$@/$(APPNAME).exe
	$(STRIP_WIN) build/$@/$(APPNAME).exe

linux32: dir ledc
	rm -rf build/$@
	mkdir -p build/$@
	$(CC_LINUX) -m32 $(SRC_LINUXONLY) $(SRC) $(INCS_LINUX32) $(LIBS_LINUX32) $(CFLAGS) -o build/$@/$(APPNAME)
	$(STRIP_LINUX) build/$@/$(APPNAME)

linux64: dir ledc
	rm -rf build/$@
	mkdir -p build/$@
	$(CC_LINUX) $(SRC_LINUXONLY) $(SRC) $(INCS_LINUX64) $(LIBS_LINUX64) $(CFLAGS) -o build/$@/$(APPNAME)
	$(STRIP_LINUX) build/$@/$(APPNAME)

debug: dir ledc
	rm -rf build/$@
	mkdir -p build/$@
	$(CC_LINUX) $(SRC_LINUXONLY) $(SRC) $(INCS_LINUX64) $(LIBS_LINUX64) -std=gnu99 -O0 -g -Wall -Werror -o build/$@/$(APPNAME)

clean:
	rm -rf build/
	rm -f res/gui_compiled_win.c res/gui_compiled_linux.c res/icon.led

test:
	cd test && $(MAKE)
