#! /bin/bash -e
#This script will download libjsmn (JSON parser) and compile it for the target platforms.

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
SRCDIR=$DIR/../nwjsmanager/lib/src
mkdir -p $SRCDIR
cd $SRCDIR
echo "Downloading libjsmn source code..."
rm -rf jsmn-src
git clone https://github.com/zserge/jsmn.git jsmn-src
cd jsmn-src
echo "Compiling libjsmn for linux64..."
make
mkdir -p ../../linux64
cp libjsmn.a ../../linux64/libjsmn.a
make clean
echo "Compiling libjsmn for linux32..."
CFLAGS=-m32 make
mkdir -p ../../linux32
cp libjsmn.a ../../linux32/libjsmn.a
make clean
echo "Compiling libjsmn for win32..."
CC=i686-w64-mingw32-gcc AR=i686-w64-mingw32-ar make
mkdir -p ../../win32
cp libjsmn.a ../../win32/libjsmn.a
make clean
