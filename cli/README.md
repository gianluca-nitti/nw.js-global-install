# nw-global-build
This is the command-line tool to automate the build of installer (for Windows) and packages (for Linux) of [nw.js](http://nwjs.io) applications that use a system-wide installation of the nw runtime files. For more information about this packaging method for nw.js apps, please see the [README file at the root of the nw.js-global-install repository](https://github.com/gianluca-nitti/nw.js-global-install/blob/master/README.md).

## Features
This tool can build Windows installers based on [NSIS](http://nsis.sourceforge.net) and .deb and .rpm packages for Linux in i386 and x86_64 architectures.

## Requirements
This tool is written in Javascript so you obviously need node and npm.
To build a Windows installer you need `makensis`. On Windows, you can get it by installing NSIS and then adding it's install directory (default is *C:\Program Files (x86)\NSIS*) to your PATH environment variable. On Linux, you can install the *nsis* package from your distribution's repositories (for example `sudo apt-get install nsis` on Ubuntu and Debian-like distros).
To build Linux packages you need [`fpm`](https://github.com/jordansissel/fpm), a tool written in Ruby that allows to build various types of packages. You can actually [build Linux packages only on a Linux machine or maybe with Cygwin](https://github.com/jordansissel/fpm/issues/505). To install `fpm`, follow the instructions [here](https://github.com/jordansissel/fpm/blob/master/README.md#system-packages) (basically, you need to install Ruby with C headers and then install the *fpm* gem). For RPM packages *fpm* also needs *rpmbuild*; if you are on a distro which isn't based on RPM packages, you probably need to install *rpm* (for example `sudo apt-get install rpm` on Ubuntu and similar).

## Usage
* Step 0: make sure you have installed the dependencies for the package types you want to build (see the previous section).
* Step 1: install this tool with `npm install -g nw-global-build` (you may need `sudo` depending on your node.js installation).
* Step 2: navigate (`cd`) to the root directory of the nw.js application you want to package. The root is where the `package.json` is located. Make sure you have added the *nwjsmanager* section with the version filter in `package.json` as explained [here](https://github.com/gianluca-nitti/nw.js-global-install/blob/master/README.md#specifying-a-nw.js-version-filter).
* Step 3: run `nw-global-build init`. This will put a `nw-global.json` file in the directory, which you can (and should) edit to customize the build process.
* Step 4: open `nw-global.json` with your editor. Read the comments to understand what each field means, and edit the fields according to your needs.
* Step 5: run `nw-global-build update`. If you had to use `sudo` at Step 1, you'll need it here too. This builds a cache of the binaries required during the build process (*nwjsmanager* and *applauncher* for the various platforms) in a subdirectory of the `nw-global-build` npm module (since it's installed globally, this is the reason you may need ```sudo```).
* Step 6: run `nw-global-build build` to actually build the packages. This will build the types listed in the ```default-output``` field of `nw-global.json`, but can be overridden by listing the package types you want to be built on the command line. For example, `nw-global-build build win deb64` builds a Windows installer and an x86_64 package for Debian-like Linux distributions.

Repeat Step 6 every time you want to rebuild the packages. If you get errors saying that binary hashes doesn't match, it means that *nwjsmanager* and/or *applauncher* have been updated, but they can't be downloaded to the cache directory due to missing permissions. To solve the issue, rebuild the binary cache with `sudo nw-global-build update`.
