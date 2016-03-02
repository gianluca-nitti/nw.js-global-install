#include <stdlib.h>
#include <stdio.h>
#include <iup.h>
#include "jsonFile.h"
#include "packageJsonFile.h"
#include "strUtil.h"

int _argc;
char** _argv;
packageJsonFile_t* app;

static void freeGlobals(){
	packageJson_file_free(app);
	free(app);
}

//Shows an error popup with the specified text.
static int showError(char *msg){
	IupOpen(&_argc, &_argv);
	char *title = "nwjsmanager";
	if(app && app->name)
		title = app->name;
	IupMessage(title, msg);
	IupClose();
	freeGlobals();
	return EXIT_FAILURE;
}

//Determines the most recent installed nw.js version compatible with the application and launches it.
static int launch(){
	//TODO
	return -1;
}

int main(int argc, char **argv){
	_argc = argc; _argv = argv;
	for(int i = 0; i < argc; i++)
		printf("[nwjsmanager][DEBUG] argv[%d]: %s\n", i, argv[i]);
	if(argc < 2)
		return showError("No application specified. Please specify on the command line the path to the directory containing the application you want to run.");
	char *packageJsonPath = string_concat(argv[1], "/package.json");
	app = malloc(sizeof(packageJsonFile_t));
	if(packageJson_file_parse(packageJsonPath, app) != JSON_SUCCESS){
		free(packageJsonPath);
		return showError("Failed to open or parse the application's package.json file. Please reinstall the application.");
	}
	free(packageJsonPath);
	return launch(); //TODO
}
