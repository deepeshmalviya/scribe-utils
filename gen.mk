CURRENT_DIR=$(shell pwd)
GEN_DIR = gen-cpp

THRIFT_DIR=$(CURRENT_DIR)/if
THRIFT=/usr/bin/thrift -r --gen cpp:pure_enums -o $(CURRENT_DIR) $(1)

all: 	clean
	mkdir -p $(CURRENT_DIR)/$(GEN_DIR)
	# Thrift
	$(THRIFT) $(THRIFT_DIR)/scribe.thrift
	find $(GEN_DIR) -name "*.skeleton.*" -exec rm -f {} \;

clean:
	rm -rf $(GEN_DIR)

.PHONY: all force
