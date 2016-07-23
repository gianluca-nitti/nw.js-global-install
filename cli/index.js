#!/usr/bin/env node

var childProcess = require('child_process');
var fs = require('fs-extra');
var path = require('path');
var os = require('os');
var crypto = require('crypto');

var lazyjson = require('lazy-json');
var cli = require('cli');
var swig = require('swig');
var request = require('sync-request');
var recursiveReadSync = require('recursive-readdir-sync');
var ignore = require('ignore');
var Log = require('log');

var log = new Log('notice');

var tmpRoot = '';
var binaryIndex = null;

function fail(msg){
	log.error(msg);
	if(tmpRoot !== '')
		fs.removeSync(tmpRoot);
	process.exit(1);
}

function mkdir(dirname){
	if(!(fs.existsSync(dirname) && fs.statSync(dirname).isDirectory()))
		try{
			fs.mkdirSync(dirname);
		}catch(ex){
			fail('Failed to create directory "' + dirname + '".');
		}
}

function cp(src, dest){
	log.info('Copying ' + src + ' to ' + dest);
	try{
		fs.copySync(src, dest);
	}catch(ex){
		log.warning(ex.message);
		fail('Failed to copy ' + src + ' to ' + dest);
	}
}

function attemptGetBinary(name, secondAttempt){
	log.info('Retrieving binary ' + name);
	if(binaryIndex === null)
		binaryIndex = JSON.parse(request('GET', 'http://gntheprogrammer.users.sourceforge.net/nw.js-global-install/binaries.json').getBody());
	var shasum = crypto.createHash('sha256');
	var binPath = path.join(__dirname, '/bin/', name);
	if(fs.existsSync(binPath))
		if(fs.statSync(binPath).isFile()){
			shasum.update(fs.readFileSync(binPath));
			var realSum = shasum.digest('hex');
			var expectedSum = binaryIndex[name].checksum;
			log.info('Expected checksum is ' + expectedSum);
			log.info('Checksum of ' + name + ' is ' + realSum);
			if(realSum === expectedSum){
				log.notice('Hashes match. Using cached version of ' + name + ' at ' + binPath);
				return binPath;
			}else if(secondAttempt){
				fail('Hashes still doesn\'t match. Please try to update nw-global-build; if the error persists, report the issue at https://github.com/gianluca-nitti/nw.js-global-install/issues .');
			}else{
				log.notice('Hash of ' + binPath + ' doesn\'t match with the hash from binaries.json. Attempting to download latest version of ' + name + '.');
				fs.removeSync(binPath);
				return attemptGetBinary(name, true);
			}
		}else
			fail('Failed to retrieve ' + name + ': "' + path + '" is not a file. Please move. rename or delete it to allow this program to download the required binary.');
	mkdir(path.join(__dirname, '/bin'));
	try{
		log.notice('Downloading ' + name + '...');
		fs.writeFileSync(binPath, request('GET', 'http://gntheprogrammer.users.sourceforge.net/nw.js-global-install/' + binaryIndex[name].path + '/' + name).getBody());
		return attemptGetBinary(name, secondAttempt);
	}catch(ex){
		fail('Failed to download the "' + name + '" binary:\n' + ex.message);
	}
}

function getBinary(name){
	return attemptGetBinary(name, false);
}

function exec(cmd, args){
	var fullCmd = cmd + ' ' + args;
	log.info('Running ' + fullCmd + '...');
	var result;
	try{
		result = childProcess.execSync(fullCmd, {maxBuffer: 1048576}); //1MB of max stdout buffer
	}catch(ex){
		log.warning(ex.message);
		fail(cmd + ' failed. Please ensure you have the required build tools installed.');
	}
	result.toString().split('\n').forEach(function(s){
		log.debug('[stdout] ' + s);
	});
	log.info(cmd + ' exited.');
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
		fail("Failed to open or parse package.json. The current working directory isn't the root of a nw.js application, or the package.json file is not a valid JSON file.");
	}
	try{
		conf = lazyjson.requireJSON('./nw-global.json');
	}catch(ex){
		fail('Failed to open or parse nw-global.json. Please run "nw-global-build init" to create a default nw-global.json in the current directory, then customize it with your build settings and run "nw-global-build build" again.');
	}

	var getConfValue = function(key, def){
		if(conf[key] === undefined)
			if(def === undefined){
				fail('Couldn\'t find configuration entry "' + key + '". Please add it in nw-global.json.');
			}else{
				log.warning('Couldn\'t find configuration entry "' + key + '", using default value "' + def + '". Add the desidered value in nw-global.json to override.');
				return def;
			}
		else
			return conf[key];
	};

	log = new Log(getConfValue('log-level', 'notice'));
	var outDir = getConfValue('out-dir', 'dist');
	mkdir(outDir);
	mkdir(path.join(outDir, 'intermediate'));
	var appName = packageJson.name;
	if(appName === undefined)
		fail('Failed to determine the name of the application from package.json.');
	log.info('Application name is ' + appName);
	var appVersion = packageJson.version;
	if(appVersion === undefined)
		fail('Failed to determine the version of the application from package.json.');
	log.info('Application version is ' + appVersion);
	var guiName = getConfValue('gui-name');
	log.info('Application user-friendly name is ' + guiName);
	var appDescr = getConfValue('description', guiName);
	log.info('Application description is "' + appDescr + '"');
	var license = getConfValue('license');
	if(!(fs.existsSync(license) && fs.statSync(license).isFile()))
		fail('Failed to open the license file at "' + license + '". Please check nw-global.json.');

	log.info('Generating list of application\'s files...');
	var appFiles = recursiveReadSync('.');
	var totalFiles = appFiles.length;
	log.info('Working directory recursively contains ' + totalFiles + ' files.');
	var appFilesFilter = ignore().add(getConfValue('ignore')).add(['/' + outDir + '/', '/nw-global.json']);
	appFiles = appFilesFilter.filter(appFiles);
	log.notice((totalFiles - appFiles.length) + ' files are being ignored.');

	var iconPath = getConfValue('icon', path.join(__dirname, '/install/default-icon.png')); //TODO: implement conversion

	var buildWindows = function(){
		log.notice('Begin building installer for Windows.');
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
		var template_win = swig.compileFile(path.join(__dirname, '/install/setup-win.nsi'));
		var applauncher_win = path.join(outDir, 'intermediate/' + appName + '.exe');
		cp(getBinary('applauncher.exe'), applauncher_win);
		//TODO: set icon
		var script_win = template_win({appName: appName, guiName: guiName, appVersion: appVersion, nwjsmanager: getBinary('nwjsmanager.exe'), license: license, appDirs: appDirsWin});
		var nsiPath = path.join(outDir, "intermediate/setup-win.nsi");
		fs.writeFileSync(nsiPath, script_win);
		exec('makensis', nsiPath);
		log.notice('Succesfully built Windows installer.');
	};

	var buildLinux = function(){
		if(process.platform == 'win32')
			log.warning('Running on windows. You probably won\'t be able to build linux packages (build tool is fpm; see https://github.com/jordansissel/fpm/issues/505 for more information.)');
		log.notice('Setting up temporary directory environment for building Linux packages.');
		tmpRoot = fs.mkdtempSync(os.tmpDir() + '/' + appName);
		log.info('Temporary directory is ' + tmpRoot);
		var tmpAppDir = tmpRoot + '/opt/' + appName;
		fs.mkdirsSync(tmpAppDir);
		log.info('Copying tree to temporary directory without the ignored files.');
		appFiles.forEach(function(item){
			fs.copySync(item, tmpAppDir + '/' + item);
		});
		var addBinary = function(binName, binPath){
			fs.copySync(getBinary(binName), binPath);
			fs.chmodSync(binPath, 0755);
		};
		log.info('Generating .desktop file...');
		var desktopFile = swig.compileFile(path.join(__dirname, '/install/desktopFile.desktop'))({name: appName, guiName: guiName, description: appDescr, categories: getConfValue('categories', '')}); //Add .desktop files to show application in menus
		fs.mkdirsSync(path.join(tmpRoot, '/usr/share/applications'));
		var desktopFilePath = path.join(tmpRoot, '/usr/share/applications/' + appName + '.desktop');
		fs.writeFileSync(desktopFilePath, desktopFile);
		fs.chmodSync(desktopFilePath, 0644);
		log.info('Adding icon...');
		cp(iconPath, path.join(tmpAppDir, '/icon.png')); //Add icon
		var installScripts = ' --template-scripts --after-install ' + path.join(__dirname, '/install/after-install-linux.sh') + ' --after-remove ' + path.join(__dirname, '/install/after-uninstall-linux.sh');
		var pkgMetadata = ' -n ' + appName + ' -v ' + appVersion + ' -m "' + getConfValue('maintainer-name', 'A package maintainer') + ' <' + getConfValue('maintainer-email', 'packagemaintainer@example.com') + '>"' + ' --description "' + appDescr + '"';
		var buildPkg = function(format, arch){
			var realArch = '';
			if(arch == 32)
				realArch = 'i386';
			else if(arch == 64)
				realArch = 'x86_64';
			log.notice('Building Linux package - ' + format + '-' + realArch);
			log.info('Adding applauncher binaries...');
			addBinary('applauncher-linux' + arch, tmpAppDir + '/' + appName);
			addBinary('nwjsmanager-linux' + arch, tmpAppDir + '/nwjsmanager-install');
			exec('fpm', '-f -s dir -t ' + format + ' -p ' + path.join(outDir, appName + '-linux-' + realArch + '.' + format) + ' -C ' + tmpRoot + ' -a ' + realArch + ' --directories=/opt/' + appName + installScripts + pkgMetadata);
			log.notice('Successfully built ' + format + '-' + realArch + ' package.');
		};
		buildPkg('deb', 32);
		buildPkg('deb', 64);
		buildPkg('rpm', 32);
		buildPkg('rpm', 64);
		fs.removeSync(tmpRoot);
	};

	buildWindows();
	buildLinux();
}
