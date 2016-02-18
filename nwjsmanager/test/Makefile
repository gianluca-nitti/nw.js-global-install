SRC=../jsonFile.c
INCS=-I../lib/src/jsmn-src
LIBS=../lib/linux64/libjsmn.a

.PHONY: all run clean

all: run

test: clean
	gcc --std=gnu99 main.c test.c $(SRC) $(INCS) $(LIBS) -g -o $@

run: test
	./$<

clean:
	rm -f test
