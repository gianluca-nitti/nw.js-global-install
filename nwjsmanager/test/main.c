#include <stdio.h>
#include "test.h"

int pass(){
	return 1;
}

int main(int argc, char **argv){
	test_init(1);
	test_add("Example test", pass);
	return test_run();
}
