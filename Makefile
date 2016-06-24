all: applauncher nwjsmanager cli
.PHONY: applauncher nwjsmanager cli clean
applauncher:
	cd applauncher && $(MAKE)
nwjsmanager:
	cd nwjsmanager && $(MAKE) test
	cd nwjsmanager && $(MAKE)
cli:
	cd cli && npm install && jshint index.js
clean:
	cd applauncher && $(MAKE) clean
	cd nwjsmanager && $(MAKE) clean
