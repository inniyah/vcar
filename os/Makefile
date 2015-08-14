PROGRAM=test

all: $(PROGRAM)

OBJS = \
	tinythread.o \
	main.o \
	tasks.o

PKG_CONFIG=
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PKG_CONFIG) 2>/dev/null`
PKG_CONFIG_LIBS=`pkg-config --libs $(PKG_CONFIG) 2>/dev/null`

CFLAGS= -O2 -g -Wall
DEFS=
INCS=-ITinyThread++-1.1/source/

LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
LIBS=-lpthread

$(PROGRAM): $(OBJS)
	g++ $(LDFLAGS) $+ -o $@ $(LIBS) $(PKG_CONFIG_LIBS)

%.o: %.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: %.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: ../tinythreadpp/source/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: ../tinythreadpp/source/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

clean:
	rm -f $(OBJS)
	rm -f $(PROGRAM)
	rm -f *.o *.a *~
