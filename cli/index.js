#!/usr/bin/env node

var childProcess = require('child_process');
var fs = require('fs-extra');
var path = require('path');

var lazyjson = require('lazy-json');
var cli = require('cli');
var swig = require('swig');
var request = require('sync-request');
var recursiveReadSync = require('recursive-readdir-sync');
var ignore = require('ignore');

function fail(msg){
	console.error('ERROR: ' + msg);
	process.exit(1);
}

function mkdir(dirname){
	if(!(fs.existsSync(dirname) && fs.statSync(dirname).isDirectory()))
		try{
			fs.mkdirSync(dirname);
		}catch(ex){
			fail('failed to create directory "' + dirname + '".');
		}
}

function cp(src, dest){
	try{
		fs.copySync(src, dest);
	}catch(ex){
		fail(ex.message);
	}
}

cli.parse(null, ['init', 'build']);

if(cli.command === 'init'){
	cp(path.join(__dirname, 'default-config.json'), 'nw-global.json');
	console.log('nw-global.json created. Edit it to configure the build process and build with "nw-global-build build".');
}else if(cli.command === "build"){
	var conf, packageJson;
	try{
		packageJson = lazyjson.requireJSON('./package.json');
	}catch(ex){
		fail("failed to open or parse package.json. The current working directory isn't the root of a nw.js application, or the package.json file is not a valid JSON file.");
	}
	try{
		conf = lazyjson.requireJSON('./nw-global.json');
	}catch(ex){
		fail('failed to open or parse nw-global.json. Please run "nw-global-build init" to create a default nw-global.json in the current directory, then customize it with your build settings and run "nw-global-build build" again.');
	}

	var getConfValue = function(key, def){
		if(conf[key] === undefined)
			if(def === undefined){
				fail('couldn\'t find configuration entry "' + key + '". Please add it in nw-global.json.');	
			}else
				return def;
		else
			return conf[key];
	};

	var outDir = getConfValue('out-dir', 'dist');
	mkdir(outDir);
	mkdir(path.join(outDir, 'intermediate'));
	var appName = conf.name;
	if(appName === undefined)
		appName = packageJson.name;
	if(appName === undefined)
		fail('failed to determine the name of the application (not defined neither in package.json nor in nw-global.json).');
	var appVersion = packageJson.version;
	if(appVersion === undefined)
		fail('failed to determine the version of the application from package.json.');
	var guiName = getConfValue('gui-name');
	var license = getConfValue('license');
	if(!(fs.existsSync(license) && fs.statSync(license).isFile()))
		fail('failed to open the license file at "' + license + '". Please check nw-global.json.');

	var appFiles = recursiveReadSync('.');
	var appFilesFilter = ignore().add(getConfValue('ignore')).add(['/' + outDir + '/', '/nw-global.json']);
	appFiles = appFilesFilter.filter(appFiles);
	var appDirsWin = [];
	appFiles.forEach(function(fileItem){
		var foundDir = -1;
		var fileWindowsPath = fileItem.replace(/\//g, '\\');
		var dirPath = path.dirname(fileItem);
		appDirsWin.forEach(function(dirItem, dirIndex){
			if(dirItem.name === dirPath)
				foundDir = dirIndex;
		});
		if(foundDir == -1)
			appDirsWin.push({name: dirPath, files: [fileWindowsPath]});
		else
			appDirsWin[foundDir].files.push(fileWindowsPath);
	});
	appDirsWin.forEach(function(dirItem){ //Add directories that doesn't contain files (necessary for the uninstaller cleanup).
		var dirParent = path.dirname(dirItem.name);
		if(dirParent !== '.'){
			if(appDirsWin.findIndex(function(item){return item.name === dirParent;}) === -1)
				appDirsWin.push({name: dirParent, files: []});
		}
	});
	appDirsWin.forEach(function(dirItem){
		dirItem.name = dirItem.name.replace(/\//g, '\\'); //Use Windows backslashes.
	});
	appDirsWin.sort(function(a,b){return b.name.localeCompare(a.name);}); //Sort directories by name to delete the inner ones first during unistall.

	var getBinary = function(name){
		var binPath = path.join(__dirname, '/bin/', name);
		if(fs.existsSync(binPath))
			if(fs.statSync(binPath).isFile())
				return binPath; //TODO: check version
			else
				fail('failed to retrieve ' + name + ': "' + path + '" is not a file. Please move. rename or delete it to allow this program to download the required binary.');
		mkdir(path.join(__dirname, '/bin'));
		try{
			var binaryIndex = JSON.parse(request('GET', 'http://gntheprogrammer.users.sourceforge.net/nw.js-global-install/binaries.json').getBody());
			fs.writeFileSync(binPath, request('GET', 'http://gntheprogrammer.users.sourceforge.net/nw.js-global-install/' + binaryIndex[name].path + '/' + name).getBody());
			//TODO: set permissions
			return binPath;
		}catch(ex){
			fail('failed to download the "' + name + '" binary:\n' + ex.message);
		}
	};

	var template_win = swig.compileFile(path.join(__dirname + '/install/setup-win.nsi'));
	var applauncher_win = path.join(outDir, 'intermediate/' + appName + '.exe');
	cp(getBinary('applauncher.exe'), applauncher_win);
	//TODO: set icon
	var script_win = template_win({appName: appName, guiName: guiName, appVersion: appVersion, nwjsmanager: getBinary('nwjsmanager.exe'), license: license, appDirs: appDirsWin});
	var nsiPath = path.join(outDir, "intermediate/setup-win.nsi");
	fs.writeFileSync(nsiPath, script_win);
	childProcess.execSync('makensis ' + nsiPath);
}
