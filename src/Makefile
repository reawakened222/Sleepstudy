PROJECT_INCL_PATH=-I../include -I.
TARGET_BUILD_DIR=../bin
LIB_FLAGS=-Wl,--start-group -lpthread -lm -lrt -lcrypt -luuid  -DCZMQ_BUILD_DRAFT_API=1 -DZMQ_BUILD_DRAFT_API=1 -I/usr/local/include -L/usr/local/lib -lczmq -lzmq -Wl,--end-group -L. -L. -L. -lpthread -lm -lrt -lcrypt -luuid
CC_FLAGS=-D_REENTRANT -D_GNU_SOURCE -Wall -Wno-unused -fno-strict-aliasing  -DBASE_THREADSAFE

CFLAGS=$(PROJECT_INCL_PATH) $(CC_FLAGS)
OBJECTS = mathFuncs.o stringHelperFuncs.o

mathFuncs.o: mathFuncs.c
	gcc -c $(PROJECT_INCL_PATH) mathFuncs.c 
	
stringHelperFuncs.o: stringHelperFuncs.c
	gcc -c $(PROJECT_INCL_PATH) stringHelperFuncs.c

mathServer.o: $(OBJECTS) mathServer.c
	gcc -c $(CFLAGS) mathServer.c

$(TARGET_BUILD_DIR): 
	@echo "Creating folder $(TARGET_BUILD_DIR)"
	mkdir -p $(@) 
build_server: $(OBJECTS) mathServer.o $(TARGET_BUILD_DIR)
	gcc -O2 $(CFLAGS) $(OBJECTS) mathServer.o -o $(TARGET_BUILD_DIR)/mathServer $(LIB_FLAGS)

build_and_start: build_server
	$(TARGET_BUILD_DIR)/mathServer &


build_debug: $(OBJECTS) mathServer.o $(TARGET_BUILD_DIR)
	gcc -g -O0 $(CFLAGS) $(OBJECTS) mathServer.o -o $(TARGET_BUILD_DIR)/mathServer_DBG $(LIB_FLAGS)

clean:
	rm -f *.log
	cd $(TARGET_BUILD_DIR) && rm -f *
