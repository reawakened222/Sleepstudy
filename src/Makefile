PROJECT_INCL_PATH=-I../include
LIB_FLAGS=-Wl,--start-group -lpthread -lm -lrt -lcrypt -luuid  -DCZMQ_BUILD_DRAFT_API=1 -DZMQ_BUILD_DRAFT_API=1 -I/usr/local/include -L/usr/local/lib -lczmq -lzmq -Wl,--end-group -L. -L. -L. -lpthread -lm -lrt -lcrypt -luuid
CC_FLAGS=-O2 -D_REENTRANT -D_GNU_SOURCE -Wall -Wno-unused -fno-strict-aliasing  -DBASE_THREADSAFE -I.
build_server:
	gcc $(PROJECT_INCL_PATH) $(LIB_FLAGS) $(CC_FLAGS) mathFuncs.c stringHelperFuncs.c mathServer.c -o mathServer

