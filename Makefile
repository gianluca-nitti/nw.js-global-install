all: applauncher nwjsmanager
.PHONY: applauncher nwjsmanager clean
applauncher:
	cd applauncher && $(MAKE)
nwjsmanager:
	cd nwjsmanager && $(MAKE)
clean:
	cd applauncher && $(MAKE) clean
	cd nwjsmanager && $(MAKE) clean
