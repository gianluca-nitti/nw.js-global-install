#!/usr/bin/env node

var childProcess = require('child_process');
var fs = require('fs');
var path = require('path');

var lazyjson = require('lazy-json');
var cli = require('cli');
var tinytemplate = require('tiny-template');

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
	fs.createReadStream(src).pipe(fs.createWriteStream(dest));
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

	function getConfValue(key, def){
		if(conf[key] === undefined)
			if(def === undefined){
				fail('couldn\'t find configuration entry "' + key + '". Please add it in nw-global.json.');	
			}else
				return def
		else
			return conf[key];
	}

	var outDir = getConfValue('outDir', 'dist');
	mkdir(outDir);
	mkdir(path.join(outDir, "intermediate"));
	var name = conf['name'];
	if(name === undefined)
		name = packageJson['name'];
	if(name === undefined){
		fail('failed to determine the name of the application (not defined neither in package.json nor in nw-global.json).');
	}
	var guiName = getConfValue('gui-name');
	var license = getConfValue('license');
	if(!(fs.existsSync(license) && fs.statSync(license).isFile()))
		fail('failed to open the license file at "' + license + '". Please check nw-global.json.');
	var template_win = fs.readFileSync('../../install/setup-win.nsi', 'utf8'); //TODO: move install into cli
	var script_win = tinytemplate.compile(template_win, {'APPNAME': name, 'APPNAMEGUI': guiName, 'LICENSE': license});
	var nsiPath = path.join(outDir, "intermediate/setup-win.nsi");
	fs.writeFileSync(nsiPath, script_win);
	childProcess.execSync('makensis ' + nsiPath);
}
