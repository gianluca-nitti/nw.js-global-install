SRC=../jsonFile.c ../packageJsonFile.c ../indexJsonFile.c
INCS=-I../lib/src/jsmn-src -I../lib/src/semver.c-src
LIBS=../lib/linux64/libjsmn.a ../lib/linux64/libsemver.a

.PHONY: all run clean

all: run

test: clean
	gcc --std=gnu99 main.c test.c $(SRC) $(INCS) $(LIBS) -g -o $@

run: test
	./$<

clean:
	rm -f test
