SHELL=/bin/bash
SRC=../textFile.c ../appList.c ../jsonFile.c ../packageJsonFile.c ../indexJsonFile.c ../paths.c ../strUtil.c ../nwjsBinaryCache.c ../download.c ../linux-only/untar.c ../extractArchive.c ../update.c
INCS=-I../lib/src/jsmn-src -I../lib/src/semver.c-src -DCURL_STATICLIB -I../lib/linux64/curl/include -I../lib/linux64/zlib/include
LIBS=../lib/linux64/libjsmn.a ../lib/linux64/libsemver.a ../lib/linux64/curl/lib/libcurl.a ../lib/linux64/zlib/lib/libz.a

.PHONY: all run clean

all: run

test: clean
	gcc --std=gnu99 -O0 -Wall -Werror main.c test.c $(SRC) $(INCS) $(LIBS) -g -o $@

run: test
	mkdir -p test-data chroot/usr/lib/nwjs/bin/{invalid-name,v0.12.1,v,vinvalid-version,v0.13.0} #Set up chroot environment to simulate some locally installed nw.js versions
	printf "app1\napp2\napp3\napp4\n" > chroot/usr/lib/nwjs/appList #Simulate a list off installed applications
	@echo "Sudo is required to run chroot-based tests. If you don't want to give sudo permissions, you can build without tests with 'cd nwjsmanager && make' from the root of the project."
	sudo ./$< #Run test

clean:
	rm -rf test chroot
