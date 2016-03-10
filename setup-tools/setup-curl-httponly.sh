#! /bin/bash -e
#This script will download the curl source from the official repository and compile an http-only version of libcur for the target platforms.

VERSION=curl-7_46_0
TYPE="--disable-shared --enable-static"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DEST="$DIR/../nwjsmanager/lib"

FLAGS="--disable-debug --disable-rtsp --disable-smtp --disable-gopher --disable-pop3 --disable-imap --disable-ftp --disable-file --disable-ldsp --disable-telnet --disable-dict --disable-tftp --disable-ares --disable-cookies --disable-crypto-auth --disable-ipv6 --disable-manual --disable-proxy --disable-unix-sokets --disable-ldap --disable-ldaps --disable-verbose --disable-versioned-symbols --enable-hidden-symbols --without-libidn --without-librtmp --without-ssl --without-zlib"

SRCDIR=$DIR/../nwjsmanager/lib/src
mkdir -p $SRCDIR
cd $SRCDIR
if [ ! -f ./curl-src/buildconf ]; then
	echo "Downloading curl source code..."
	git clone https://github.com/curl/curl.git curl-src
fi
cd curl-src
git checkout tags/$VERSION
if [ ! -f ./configure ]; then
	./buildconf
fi

echo "Building for linux64..."
./configure --prefix=$DEST/linux64/curl $TYPE $FLAGS
make -C $SRCDIR/curl-src/lib install
make -C $SRCDIR/curl-src/include install

echo "Building for linux32..."
./configure --prefix=$DEST/linux32/curl $TYPE $FLAGS CFLAGS=-m32
make -C $SRCDIR/curl-src/lib install
make -C $SRCDIR/curl-src/include install

echo "Building for win32..."
./configure --prefix=$DEST/win32/curl --host=i686-w64-mingw32 $TYPE $FLAGS
make -C $SRCDIR/curl-src/lib install
make -C $SRCDIR/curl-src/include install
