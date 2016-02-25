#! /bin/bash -e
#This script will download semver.c (C semantic versioning implementation) and compile it for the target platforms.

VERSION=0.1.4
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SRCDIR=$DIR/../nwjsmanager/lib/src
mkdir -p $SRCDIR
cd $SRCDIR
echo "Downloading semver.c source code..."
rm -rf semver.c-src
git clone https://github.com/h2non/semver.c semver.c-src
cd semver.c-src
git checkout tags/$VERSION
echo "Running semver.c tests..."
make test
echo "Copyng semver.c alternative Makefile (to build it as a static library)..."
rm Makefile
cp $DIR/Makefile-semver Makefile
echo "Compiling semver.c for linux64..."
make
mkdir -p ../../linux64
cp libsemver.a ../../linux64/libsemver.a
make clean
echo "Compiling semver.c for linux32..."
CFLAGS=-m32 make
mkdir -p ../../linux32
cp libsemver.a ../../linux32/libsemver.a
make clean
echo "Compiling semver.c for win32..."
CC=i686-w64-mingw32-gcc AR=i686-w64-mingw32-ar make
mkdir -p ../../win32
cp libsemver.a ../../win32/libsemver.a
make clean
