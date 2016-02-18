#include <stdio.h>
#include <stdlib.h>

typedef struct{
	char *name;
	int (*function)(void);
}test;

test* tests;
int n = 0;

void test_init(int testCount){
	tests = malloc(testCount * sizeof(test));
}

void test_add(char* name, int (*function)(void)){
	tests[n].name = name;
	tests[n].function = function;
	n++;
}

int test_run(){
	int passed = 0;
	for(int i = 0; i < n; i++){
		printf("Running test: \"%s\"... ", tests[i].name);
		if(tests[i].function()){
			puts("PASSED.");
			passed++;
		}else
			puts("FAILED.");
	}
	int failed = n - passed;
	printf("%d/%d (%d%%) tests passed, %d/%d (%d%%) failed.\n", passed, n, (passed*100/n), failed, n, (failed*100/n));
	free(tests);
	return failed;
}
