all: test snd_test rcv_test can_test

CFLAGS= -O2 -g -Wall
LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
DEFS = 

INCS = -I../tinythreadpp/source/ -I../libcandbc/ -I../libcandbc/model/
LIBS = -lpthread

test: test.o intercom.o tinythread.o
	g++ $(LDFLAGS) $+ -o $@ $(LIBS)

snd_test: snd_test.o intercom.o
	g++ $(LDFLAGS) $+ -o $@ $(LIBS)

rcv_test: rcv_test.o intercom.o
	g++ $(LDFLAGS) $+ -o $@ $(LIBS)

CAN_TEST_OBJS= \
	dbc.tab.o \
	dbc.yy.o \
	dbcModel.o \
	dbcReader.o \
	dbcWriter.o \
	hashtable.o \
	hashtable_itr.o \
	messageHash.o \
	signalFormat.o \
	busAssignment.o \
	messageDecoder.o \
	measurement.o \
	ascReader.o \
	can_test.o \
	intercom.o

can_test: $(CAN_TEST_OBJS)
	g++ $(LDFLAGS) $+ -o $@ $(LIBS)

%.o: %.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: %.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../tinythreadpp/source/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../tinythreadpp/source/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.tab.c %.tab.h: ../libcandbc/model/%.y
	bison -d $<

%.yy.c: ../libcandbc/model/%.l %.tab.h
	flex -o $@ $<

%.o: ../libcandbc/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../libcandbc/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../libcandbc/model/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../libcandbc/model/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../libcandbc/hashtable/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../libcandbc/hashtable/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../libcandbc/ascreader/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../libcandbc/ascreader/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

clean:
	rm -fv test snd_test rcv_test can_test *.o *~ *.tab.c *.tab.h *.yy.c
