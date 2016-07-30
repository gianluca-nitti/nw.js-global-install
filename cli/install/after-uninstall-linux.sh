#! /bin/bash
echo Removing '<%= name %>' from nwjsmanager application list...
/usr/bin/nwjsmanager --uninstall <%= name %>
if [ $? -eq 1 ]; then
	echo There aren\'t other nw.js-based applications installed\; removing nwjsmanager and cached nw.js binaries.
	rm -rf /usr/lib/nwjs
	rm /usr/bin/nwjsmanager
fi
echo Done.
exit 0
