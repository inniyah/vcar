all: test snd_test rcv_test

CFLAGS= -O2 -g -Wall
LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
DEFS = 

INCS = -I../tinythreadpp/source/
LIBS = -lpthread

test: test.o intercom.o tinythread.o
	g++ $(LDFLAGS) $+ -o $@ $(LIBS)

snd_test: snd_test.o intercom.o
	g++ $(LDFLAGS) $+ -o $@ $(LIBS)

rcv_test: rcv_test.o intercom.o
	g++ $(LDFLAGS) $+ -o $@ $(LIBS)

%.o: %.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: %.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../tinythreadpp/source/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../tinythreadpp/source/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

clean:
	rm -fv sender receiver *.o *~
