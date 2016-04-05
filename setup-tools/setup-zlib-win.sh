#! /bin/bash -e
#This script will download the zlib source from the official repository and compile it for the win32 target (Linux binaries are already installed).

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DEST="$DIR/../nwjsmanager/lib"

SRCDIR=$DIR/../nwjsmanager/lib/src
mkdir -p $SRCDIR
cd $SRCDIR
if [ ! -f ./zlib-src/Makefile ]; then
	echo "Downloading curl source code..."
	git clone https://github.com/madler/zlib.git zlib-src
fi

cd zlib-src
#TODO: checkout version tag
patch win32/Makefile.gcc $DIR/zlibMakefile.patch
INSTALLDIR=$DIR/../nwjsmanager/lib/win32/zlib
mkdir -p $INSTALLDIR
BINARY_PATH=$INSTALLDIR/bin INCLUDE_PATH=$INSTALLDIR/include LIBRARY_PATH=$INSTALLDIR/lib make -f win32/Makefile.gcc install
