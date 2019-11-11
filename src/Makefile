PROJECT_INCL_PATH=-I../include -I.
TARGET_BUILD_DIR=../bin
LIB_FLAGS=-Wl,--start-group -lpthread -lm -lrt -lcrypt -luuid  -DCZMQ_BUILD_DRAFT_API=1 -DZMQ_BUILD_DRAFT_API=1 -I/usr/local/include -L/usr/local/lib -lczmq -lzmq -Wl,--end-group -L. -L. -L. -lpthread -lm -lrt -lcrypt -luuid
CC_FLAGS=-O2 -D_REENTRANT -D_GNU_SOURCE -Wall -Wno-unused -fno-strict-aliasing  -DBASE_THREADSAFE

CFLAGS=$(PROJECT_INCL_PATH) $(CC_FLAGS)
OBJECTS = mathFuncs.o stringHelperFuncs.o

mathFuncs.o:
	gcc -c $(PROJECT_INCL_PATH) mathFuncs.c 
	
stringHelperFuncs.o:
	gcc -c $(PROJECT_INCL_PATH) stringHelperFuncs.c

mathServer.o: $(OBJECTS)
	gcc -c $(CFLAGS) $(OBJECTS) mathServer.c

build_server: $(OBJECTS) mathServer.o 
	gcc $(CFLAGS) $(OBJECTS) mathServer.o -o $(TARGET_BUILD_DIR)/mathServer $(LIB_FLAGS)