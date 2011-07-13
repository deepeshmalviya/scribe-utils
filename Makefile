CURRENT_DIR = $(shell pwd)
LBITS=$(shell getconf LONG_BIT)

all: clean gen build

gen:
	make -f $(CURRENT_DIR)/gen.mk

build:
	make -f $(CURRENT_DIR)/build$(LBITS).mk

install:
	make -f $(CURRENT_DIR)/build$(LBITS).mk install

uninstall: 
	make -f $(CURRENT_DIR)/build$(LBITS).mk uninstall

clean:
	make -f $(CURRENT_DIR)/gen.mk clean
	make -f $(CURRENT_DIR)/build$(LBITS).mk clean

.PHONY: all clean force


