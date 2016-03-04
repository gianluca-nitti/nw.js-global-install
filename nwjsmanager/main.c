#include <stdlib.h>
#include <stdio.h>
#include <iup.h>
#include <unistd.h>
#include "jsonFile.h"
#include "nwjsBinaryCache.h"
#include "packageJsonFile.h"
#include "paths.h"
#include "strUtil.h"

int _argc;
char** _argv;
packageJsonFile_t* app;
semver_t latestNwVersion;

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
	semver_t* launchVersion = NULL;
	semverList_t installedVersions = nwjs_binary_cache_get_versions();
	for(int i = 0; i < installedVersions.count; i++)
		if(packageJson_file_is_nw_version_OK(app, installedVersions.items[i], latestNwVersion) && (!launchVersion || semver_gt(installedVersions.items[i], *launchVersion)))
			launchVersion = &installedVersions.items[i];
		else
			printf("[nwjsmanager][DEBUG] Version %d.%d.%d is not compatible.\n", installedVersions.items[i].major, installedVersions.items[i].minor, installedVersions.items[i].patch);
	if(!launchVersion){
		showError("There isn't any nw.js version compatible with the application installed."); //TODO: download it
		semverList_free(&installedVersions);
		return 1;
	}else{
		printf("[nwjsmanager][DEBUG] Launching %s with nw.js v%d.%d.%d.\n", app->name, launchVersion->major, launchVersion->minor, launchVersion->patch);
		char binDirName[256];
		sprintf(binDirName, "v%d.%d.%d/", launchVersion->major, launchVersion->minor, launchVersion->patch);
		char *binPath = string_concat(string_concat(path_get_nwjs_cache(), binDirName), NWJS_BIN_NAME);
		printf("[nwjsmanager][DEBUG] Nw.js binary path: %s.\n", binPath);
		execv(binPath, _argv /*+ sizeof(char*)*/); //TODO: manage errors
		return 0;
	}
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
	semver_parse("0.13.0", &latestNwVersion); //TODO: remove hardcoded value and read it form the index.json file
	return launch(); //TODO
}
