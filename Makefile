all: nwjsmanager
.PHONY: nwjsmanager clean
nwjsmanager:
	cd nwjsmanager && $(MAKE)
clean:
	cd nwjsmanager && $(MAKE) clean
