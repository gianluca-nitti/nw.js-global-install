#!/usr/bin/env node

var cli = require("cli");
var fs = require("fs");
var path = require("path");
var lazyjson = require("lazy-json");

cli.parse(null, ["init", "build"]);

if(cli.command == "init"){
	fs.createReadStream(path.join(__dirname, "default-config.json")).pipe(fs.createWriteStream("nw-global.json"));
	console.log('nw-global.json created. Edit it to configure the build process and build with "nw-global-build build".');
}else if(cli.command == "build"){
	var conf = lazyjson.requireJSON("./nw-global.json");
	console.log(conf);
	//TODO
}
