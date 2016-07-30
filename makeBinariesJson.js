#!/usr/bin/env node

var crypto = require('crypto');
var fs = require('fs');
var path = require('path');

var plat_suffix = {'win32': '.exe', 'linux32': '-linux32', 'linux64': '-linux64'};

var version = "1.0.0";
var out = {};

['applauncher', 'nwjsmanager'].forEach(function(name){
	['win32', 'linux32', 'linux64'].forEach(function(platform){
		var shasum = crypto.createHash('sha256');
		shasum.update(fs.readFileSync(path.join(__dirname, name, 'build', platform, name + (platform === 'win32' ? '.exe' : ''))));
		out[name + plat_suffix[platform]] = {'path': (name === 'applauncher' ? 'applauncher/' : '') + 'v' + version, 'checksum': shasum.digest('hex')};
	});
});

fs.writeFileSync(path.join(__dirname, '/binaries.json'), JSON.stringify(out, null, '\t'));
