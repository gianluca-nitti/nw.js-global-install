all: applauncher nwjsmanager indexJson cli
.PHONY: applauncher nwjsmanager cli indexJson clean
applauncher:
	cd applauncher && $(MAKE)
nwjsmanager:
	cd nwjsmanager && $(MAKE) test
	cd nwjsmanager && $(MAKE)
cli:
	cd cli && npm install && jshint index.js
indexJson: nwjsmanager
	jsonlint index.json
	./nwjsmanager/test/checkUrls.js
clean:
	cd applauncher && $(MAKE) clean
	cd nwjsmanager && $(MAKE) clean
