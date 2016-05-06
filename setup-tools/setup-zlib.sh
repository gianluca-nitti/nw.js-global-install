#! /bin/bash -e
#This script will download the zlib source from the official repository and compile it for the target platforms.

VERSION=v1.2.8
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DEST="$DIR/../nwjsmanager/lib"

SRCDIR=$DIR/../nwjsmanager/lib/src
mkdir -p $SRCDIR
cd $SRCDIR
if [ ! -f ./zlib-src/Makefile ]; then
	echo "Downloading zlib source code..."
	git clone https://github.com/madler/zlib.git zlib-src
fi
cd zlib-src
git checkout tags/$VERSION

echo "Building for linux64..."
./configure --static --prefix=$DEST/linux64/zlib
make install
make clean

echo "Building for linux32..."
export CC="gcc -m32"
./configure --static --prefix=$DEST/linux32/zlib
make install
make clean
unset CC

echo "Building for win32..."
patch win32/Makefile.gcc $DIR/zlibMakefile.patch
INSTALLDIR=$DEST/win32/zlib
mkdir -p $INSTALLDIR
BINARY_PATH=$INSTALLDIR/bin INCLUDE_PATH=$INSTALLDIR/include LIBRARY_PATH=$INSTALLDIR/lib make -f win32/Makefile.gcc install
