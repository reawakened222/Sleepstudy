
make_test:
	gcc  -I../include  -O2 -D_REENTRANT -D_GNU_SOURCE -Wall -Wno-unused -fno-strict-aliasing  -DBASE_THREADSAFE CuTest.c AllTests.c testWithSleeps.c -o test -Wl,--start-group  -lpthread -lm -lrt -lcrypt -luuid  -DCZMQ_BUILD_DRAFT_API=1 -DZMQ_BUILD_DRAFT_API=1 -I/usr/local/include -L/usr/local/lib -lczmq -lzmq -Wl,--end-group -L. -L. -L. -lpthread -lm -lrt -lcrypt -luuid

build_and_test: make_test
	./test