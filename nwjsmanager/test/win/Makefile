CC=i686-w64-mingw32-gcc
CFLAGS=--std=gnu99 -mno-ms-bitfields
ZLIB=-I../../lib/win32/zlib/include ../../lib/win32/zlib/lib/libz.a

all: testZip.exe

testZip.exe: testZip.c
	$(CC) $< ../../win-only/unzip.c ../../win-only/junzip.c ../../extractArchive.c ../../strUtil.c $(ZLIB) $(CFLAGS) -o $@

clean:
	rm -rf testZip.exe
