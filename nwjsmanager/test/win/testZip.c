#include <stdio.h>
#include "../../extractArchive.h"

int main(int argc, char **argv){
	return extractArchive("archive.zip", "test-data/archive_out");
}
