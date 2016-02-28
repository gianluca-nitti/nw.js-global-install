SHELL=/bin/bash
SRC=../jsonFile.c ../packageJsonFile.c ../indexJsonFile.c ../paths.c ../nwjsBinaryCache.c
INCS=-I../lib/src/jsmn-src -I../lib/src/semver.c-src
LIBS=../lib/linux64/libjsmn.a ../lib/linux64/libsemver.a

.PHONY: all run clean

all: run

test: clean
	gcc --std=gnu99 -Wall -Werror main.c test.c $(SRC) $(INCS) $(LIBS) -g -o $@

run: test
	mkdir -p chroot/var/cache/nwjs/bin/{invalid-name,v0.12.1,v,vinvalid-version,v0.13.0} #Set up chroot environment to simulate some locally installed nw.js versions
	@echo "Sudo is required to run chroot-based tests. If you don't want to give sudo permissions, you can build without tests with 'cd nwjsmanager && make' from the root of the project."
	sudo ./$< #Run test

clean:
	rm -rf test chroot
