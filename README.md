# nw.js-global-install [![Build Status](https://travis-ci.org/gianluca-nitti/nw.js-global-install.svg?branch=master)](https://travis-ci.org/gianluca-nitti/nw.js-global-install)
The aim of this project is to provide a fast way for users to launch [nw.js](https://github.com/nwjs/nw.js) based applications.
It consists of a small executable (*nwjsmanager*) for Windows and Linux which will manage a cache of one or more nw.js versions in a common directory on the users' machines. These nw.js runime files are shared between multiple applications.
The project will define a new way for developers to package their nw.js apps, without bundling the nw binaries with the application files; when a user installs an application packaged in this format, the installer (NSIS or InnoSetup on Windows, or .deb/.rpm package on Linux) will look for *nwjsmanager* and extract it if it's not already present on the computer. Then when the user launches the application, *nwjsmanager* will check if there is a nw.js version compatible with the application in the cache folder; if a suitable version is found, the application will be launched using it, otherwise the latest compatible version will be downloaded from http://dl.nwjs.io/ and put in the cache. This will avoid the 40MB+ nw.js download when the user installs another nw.js based application, speeding up the download&install process and saving hard drive space.

A tiny native executable (*applauncher*) is packed with the application: the developer can rename it accordingly to the application's name (and maybe add an icon on Windows), and it's purpose is to look for *nwjsmanager* and launch it with the correct arguments if it's found, or show a proper error message asking the user to reinstall the application if it's not (a copy of *nwjsmanager*, which is around 1MB, is shipped with applications and is installed if it isn't already).

## Flowchart ([bitmap version](http://i.imgur.com/9yT4De1.png?1))
![flowchart](https://rawgit.com/gianluca-nitti/nw.js-global-install/master/path/Flowchart.svg)