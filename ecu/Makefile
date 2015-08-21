PROGRAM=test

all: $(PROGRAM)

OBJS = \
	tinythread.o \
	intercom.o \
	main.o \
	can.o \
	task_handlers.o \
	isr_handlers.o

PKG_CONFIG=
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PKG_CONFIG) 2>/dev/null`
PKG_CONFIG_LIBS=`pkg-config --libs $(PKG_CONFIG) 2>/dev/null`

CFLAGS= -O2 -g -Wall
DEFS=
INCS=-I../tinythreadpp/source/ -I../intercom/

LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
LIBS=-lpthread

$(PROGRAM): $(OBJS)
	g++ $(LDFLAGS) $+ -o $@ $(LIBS) $(PKG_CONFIG_LIBS)

%.o: %.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: %.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: os/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: os/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: ../tinythreadpp/source/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: ../tinythreadpp/source/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: ../intercom/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../intercom/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

clean:
	rm -f $(OBJS)
	rm -f $(PROGRAM)
	rm -f *.o *.a *~
