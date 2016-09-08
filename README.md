# nw.js-global-install [![Build Status](https://travis-ci.org/gianluca-nitti/nw.js-global-install.svg?branch=master)](https://travis-ci.org/gianluca-nitti/nw.js-global-install)
The aim of this project is to provide an easy and fast way for users to launch [nw.js](https://github.com/nwjs/nw.js) based applications.
It consists of a small executable (*nwjsmanager*) for Windows and Linux which will manage a cache of one or more nw.js versions in a common directory on the user's machine. These nw.js runtime files are shared between multiple applications.
The project defines a new way for developers to package their nw.js apps, without bundling the nw binaries with the application files; when a user installs an application packaged in this format, the installer (NSIS-based on Windows, or .deb/.rpm package on Linux) looks for *nwjsmanager* and extracts it if it's not already present on the computer, and launches it to ensure a suitable nw.js version is installed; if not, it downloads the latest compatible version. When the user launches the application, *nwjsmanager* checks if there is a nw.js version compatible with the application in the cache folder; if a suitable version is found, the application will be launched using it, otherwise the latest compatible version will be downloaded from http://dl.nwjs.io/ and put in the cache. This will avoid the 40MB+ nw.js download when the user installs another nw.js based application, speeding up the download&install process and saving hard drive space.

A tiny native executable (*applauncher*) is packed with the application: the developer can rename it accordingly to the application's name (and maybe add an icon on Windows), and it's purpose is to look for *nwjsmanager* and launch it with the correct arguments if it's found, or show a proper error message asking the user to reinstall the application if it's not (a copy of *nwjsmanager*, which is around 1MB, is shipped with applications and is installed if it isn't already).

## Resources for application developers
This repository also contains a command line tool that automates the process of building packages and installers for your nw.js application. See it's README [here](https://github.com/gianluca-nitti/nw.js-global-install/blob/master/cli/README.md).
To start packaging applications with this system-wide nwjs installation method, [get the cli tool from npm](https://www.npmjs.com/package/nw-global-build).

## Specifying a nw.js version filter
When *nwjsmanager* needs to download an nw.js version, the latest one compatible with the application is chosen. To specify which nw.js versions are compatible with your application, you need to add this section to your `package.json`:
```
"nwjsmanager":{
	"nwjs-version-filter": ">=0.15.1" //Example
}
```
The filter string is made of a [semver](http://semver.org/) operator and a version. See [here](https://github.com/h2non/semver.c#api) for a list of supported operators.

## Flowchart ([bitmap version](http://i.imgur.com/9yT4De1.png?1))
![flowchart](https://cdn.rawgit.com/gianluca-nitti/nw.js-global-install/master/Flowchart.svg)

## Other resources
* There are currently a couple of issues with packages generated with the cli tool: https://github.com/gianluca-nitti/nw.js-global-install/issues.
* [Repository with example application](https://github.com/gianluca-nitti/nw-image-viewer)
* [Thread on the nw.js discussion group](https://groups.google.com/forum/#!topic/nwjs-general/xv_GKgLgwkU)
* [Post on my blog](http://gntheprogrammer.blogspot.com/2016/08/nwjs-system-wide-installation.html)

## Libraries involved
The tools in this repository are made possible thanks to the following projects. Both *applauncher* and *nwjsmanager* are compiled with [gcc](https://gcc.gnu.org/) (natively on Linux and cross-compiled to Windows with [MingW](http://www.mingw.org/)).

* *applauncher* links to:
	* (Linux version only) [GTK+](http://www.gtk.org/), LGPL licensed, and related libraries (GDK, Pango, Glib (LGPL), Xlib (MIT))
* *nwjsmanager* links to:
	* [curl](https://curl.haxx.se/) ([MIT-like license](https://curl.haxx.se/docs/copyright.html))
	* [IUP](http://webserver2.tecgraf.puc-rio.br/iup/) ([MIT](http://webserver2.tecgraf.puc-rio.br/iup/en/copyright.html)), user interface library that uses the native Win32 API on Windows and GTK+ on Linux (which is dinamically linked with nwjsmanager too).
	* [jsmn](http://zserge.com/jsmn.html) ([MIT](https://github.com/zserge/jsmn/blob/master/LICENSE))
	* [semver.c](https://github.com/h2non/semver.c) ([MIT](https://github.com/h2non/semver.c/blob/master/LICENSE))
	* [zlib](http://www.zlib.net/) ([zlib license](http://www.zlib.net/zlib_license.html))
	and uses parts of code from the following projects:
	* (Windows version only) [junzip](https://github.com/jokkebk/JUnzip) ([public domain/unlicense](https://github.com/jokkebk/JUnzip/blob/master/LICENSE))
	* (Windows version only) Part of the [IsWow64Process function example](https://msdn.microsoft.com/it-it/library/windows/desktop/ms684139(v=vs.85).aspx), [MS-PL licensed](https://msdn.microsoft.com/en-us/cc300389.aspx) is included in the *nwjsmanager/win-only/IsWow64.c* file
	* (Linux version only) [untar.c](https://github.com/libarchive/libarchive/blob/master/contrib/untar.c) (public domain, as stated in the source file)
* the cli tool uses the following node modules:
	* [fs-extra](https://github.com/jprichardson/node-fs-extra) ([MIT](https://github.com/jprichardson/node-fs-extra/blob/master/LICENSE))
	* [ignore](https://github.com/kaelzhang/node-ignore) ([MIT](https://github.com/kaelzhang/node-ignore/blob/master/LICENSE-MIT))
	* [lazy-json](https://github.com/egoist/lazy-json) ([MIT](https://github.com/egoist/lazy-json/blob/master/LICENSE))
	* [log](https://github.com/tj/log.js) ([MIT](https://github.com/tj/log.js#license))
	* [recursive-readdir-sync](https://github.com/battlejj/recursive-readdir-sync) ([MIT](https://github.com/battlejj/recursive-readdir-sync/blob/master/LICENSE))
	* [semver](https://github.com/npm/node-semver) ([ISC](https://github.com/npm/node-semver/blob/master/LICENSE))
	* [swig](https://github.com/paularmstrong/swig) ([license](https://github.com/paularmstrong/swig/blob/master/LICENSE))
	* [sync-request](https://github.com/ForbesLindesay/sync-request) ([MIT](https://github.com/ForbesLindesay/sync-request/blob/master/LICENSE))
	* [to-ico-sync](https://github.com/gianluca-nitti/to-ico-sync), fork of [to-ico](https://github.com/kevva/to-ico) ([MIT](https://github.com/kevva/to-ico/blob/master/license))