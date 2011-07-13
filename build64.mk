CURRENT_DIR=$(shell pwd)
SRC_DIR = $(CURRENT_DIR)/src
BUILD_DIR = $(CURRENT_DIR)/build
LIBRARY=libscribewrapper.so.1.0 
EXEC_STDIN=scribe_stdin 
EXEC_TAIL=scribe_tail

THRIFT_INCLUDES = -I/usr/include/thrift -I/usr/include/thrift/fb303

CXX = g++
CXX_SHARED_CFLAGS = -g -shared -W1,-soname,libscribewrapper.so.1
CXX_EXEC_CFLAGS = -Wall -g
CXX_EXEC_LDFLAGS = -lboost_program_options -lboost_iostreams -lthrift -lfb303 -lscribe -lscribewrapper
CXX_INCLUDES = $(THRIFT_INCLUDES) -I$(CURRENT_DIR)
CXX_LIB_CFLAGS = -Wall -g -fPIC -pipe -m64
LIB_DIR = /usr/lib64

all: clean init $(LIBRARY) lib_install $(EXEC_STDIN) $(EXEC_TAIL)

init: 
	mkdir $(BUILD_DIR)

$(SRC_DIR)/scribe_wrapper.o: $(SRC_DIR)/scribe_wrapper.cpp
	$(CXX) $(CXX_INCLUDES) $(CXX_LIB_CFLAGS) -o $@ -c $<

$(LIBRARY): $(SRC_DIR)/scribe_wrapper.o
	$(CXX) $(CXX_SHARED_CFLAGS) $< -o $(BUILD_DIR)/$@ 

$(EXEC_STDIN): $(SRC_DIR)/scribe_stdin.cpp
	$(CXX) $(CXX_INCLUDES) $(CXX_EXEC_LDFLAGS) $< -o $(BUILD_DIR)/$@

$(EXEC_TAIL): $(SRC_DIR)/scribe_tail.cpp
	$(CXX) $(CXX_INCLUDES) $(CXX_EXEC_LDFLAGS) $< -o $(BUILD_DIR)/$@

lib_install:
	cp $(BUILD_DIR)/$(LIBRARY) $(LIB_DIR)/$(LIBRARY)
	ln -sf $(LIB_DIR)/$(LIBRARY) $(LIB_DIR)/libscribewrapper.so
	ln -sf $(LIB_DIR)/$(LIBRARY) $(LIB_DIR)/libscribewrapper.so.1

install: 
	install -m 755 $(BUILD_DIR)/$(EXEC_STDIN) /usr/bin
	install -m 755 $(BUILD_DIR)/$(EXEC_TAIL) /usr/bin

uninstall: 
	rm -f /usr/bin/$(EXEC_STDIN)
	rm -f /usr/bin/$(EXEC_TAIL) 
	rm -f $(LIB_DIR)/libscribewrapper.*

clean: force
	$(shell find . -name '*.o' -exec rm -f {} \;)
	rm -rf $(BUILD_DIR)

.PHONY: all force
