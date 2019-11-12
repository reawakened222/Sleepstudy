INCL=-I../include 
CFLAGS=$(INCL) -O2 -D_REENTRANT -D_GNU_SOURCE -Wall -Wno-unused -fno-strict-aliasing  -DBASE_THREADSAFE
LDFLAGS=-Wl,--start-group  -lpthread -lm -lrt -lcrypt -luuid  -DCZMQ_BUILD_DRAFT_API=1 -DZMQ_BUILD_DRAFT_API=1 -I/usr/local/include -L/usr/local/lib -lczmq -lzmq -Wl,--end-group -L. -L. -L. -lpthread -lm -lrt -lcrypt -luuid
TARGET_BUILD_DIR=./exec
phony: build_test
$(TARGET_BUILD_DIR): 
	mkdir -p $(TARGET_BUILD_DIR)
build_test: $(TARGET_BUILD_DIR)
	gcc $(CFLAGS) CuTest.c AllTests.c testServerStartup_CuTest.c -o $(TARGET_BUILD_DIR)/test $(LDFLAGS)