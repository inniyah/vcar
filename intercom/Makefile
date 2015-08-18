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

../libcandbc/libCanDbc.a:
	$(MAKE) -C ../libcandbc/ static

CAN_TEST_OBJS= \
	can_test.o \
	intercom.o

can_test: $(CAN_TEST_OBJS)
	g++ $(LDFLAGS) $+ -o $@ -L../libcandbc -lCanDbc $(LIBS)

%.o: %.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: %.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../tinythreadpp/source/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../tinythreadpp/source/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.tab.c %.tab.h: %.y
	bison -d $<

%.yy.c: %.l %.tab.h
	flex -o $@ $<

clean:
	rm -fv test snd_test rcv_test can_test *.o *~ *.tab.c *.tab.h *.yy.c
