#!/usr/bin/env node

//This script is used to check that all the URLs in the list (urlList file) generated by the testing engine refer to existing files on dl.nwjs.io.

var fs = require('fs');
var path = require('path');
var http = require('http');

var urls = fs.readFileSync(path.join(__dirname, '/urlList'), 'utf8').split('\n');
if(urls[urls.length - 1] === '')
	urls.pop(); //Remove the last item, that is an empty line

urls.forEach(function(url){
	if(url.lastIndexOf('http://dl.nwjs.io/', 0) !== 0){ //Check if url starts with http://dl.nwjs.io/
		console.error(url + ' points to the wrong domain.');
		process.exit(1);
	}
	var urlPath = url.slice(17); //Remove the http://dl.nwjs.io
	http.request({method:'HEAD', host: 'dl.nwjs.io', path: urlPath}, function(res){
		console.log(res.statusCode + ' ' + url);
		if(res.statusCode != 200){
			console.error('Request to ' + url + ' returned status code ' + res.statusCode + '.');
			process.exit(1);
		}
	}).end();
});
