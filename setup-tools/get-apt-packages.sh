#! /bin/bash -e
# This script is written for distributions based on .deb packages, and it requires the apt-get package manager.
# Run this script to install the cross-compilers for Windows and the compiler libraries to compile for x86 linux from x86_64 linux.
# This script needs to be run as superuser (use sudo ./get-packages.sh)

dpkg --add-architecture i386
sed -i -e 's/deb http/deb [arch=amd64] http/' "/etc/apt/sources.list.d/google-chrome.list" #To avoid Travis-CI failure due to Google Chrome not available for i386 (credit: https://www.reddit.com/r/chrome/comments/48oje6/linux_how_to_fix_failed_to_fetch/d0lbqbt)
apt-get update
apt-get -y install gdb              # For test debugging (analysis of core dumps)
apt-get install -y atool            # Tool to automate extraction of tar and zip archives, required by the IUP download script.
apt-get install -y mingw-w64        # To cross compile for Windows
apt-get install -y libc6-dev-i386   # To cross compile for Linux x86 from linux x86_64.
apt-get install -y libgtk2.0-dev    # Gtk dev library.
apt-get install -y libgtk2.0-0:i386 # Required for Gtk cross compilation.
apt-get install -y autoconf         # Required to build curl.
apt-get install -y automake         # Required to build curl.
apt-get install -y libtool          # Required to build curl.

#These links are necessary for the cross-compilation from x86_64 Linux to x86 Linux to link the code with the GTK driver and it's dependencies (credit: http://stackoverflow.com/a/20845987)
ln -sf /lib/i386-linux-gnu/libglib-2.0.so.0 /usr/lib32/libglib-2.0.so
ln -sf /usr/lib/i386-linux-gnu/libgtk-x11-2.0.so.0 /usr/lib32/libgtk-x11-2.0.so
ln -sf /usr/lib/i386-linux-gnu/libgdk-x11-2.0.so.0 /usr/lib32/libgdk-x11-2.0.so
ln -sf /usr/lib/i386-linux-gnu/libatk-1.0.so.0 /usr/lib32/libatk-1.0.so
ln -sf /usr/lib/i386-linux-gnu/libpangox-1.0.so.0 /usr/lib32/libpangox-1.0.so
ln -sf /usr/lib/i386-linux-gnu/libpango-1.0.so.0 /usr/lib32/libpango-1.0.so
ln -sf /usr/lib/i386-linux-gnu/libgmodule-2.0.so.0 /usr/lib32/libgmodule-2.0.so
ln -sf /usr/lib/i386-linux-gnu/libgobject-2.0.so.0 /usr/lib32/libgobject-2.0.so
ln -sf /usr/lib/i386-linux-gnu/libgdk_pixbuf-2.0.so.0 /usr/lib32/libgdk_pixbuf-2.0.so
ln -sf /usr/lib/i386-linux-gnu/libXext.so.6 /usr/lib32/libXext.so
ln -sf /usr/lib/i386-linux-gnu/libX11.so.6 /usr/lib32/libX11.so
