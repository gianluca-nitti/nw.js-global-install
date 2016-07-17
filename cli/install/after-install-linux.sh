#! /bin/bash
if [ ! -f /usr/bin/nwjsmanager ]; then
	echo Installing nw.js version manager to /usr/bin/nwjsmanager...
	cp /opt/<%= name %>/nwjsmanager-install /usr/bin/nwjsmanager
fi
rm -f /opt/<%= name %>/nwjsmanager-install
echo Adding '<%= name %>' to nwjsmanager application list...
/usr/bin/nwjsmanager /opt/<%= name %> --nwjsmanager-prepare
/usr/bin/nwjsmanager --install <%= name %> 
echo Done.
