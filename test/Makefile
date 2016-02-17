PROGRAM=ecu

all: $(PROGRAM)

HEADERS = \
	os.h \
	os_cfg.h \
	sys_cfg.h \
	can01.dbc.h

SYS_OBJS= \
	tinythreadpp/tinythread.o \
	intercom.o \
	sys_os.o \
	sys_main.o

OBJS = \
	$(SYS_OBJS) \
	TaskHandlers.o \
	IsrHandlers.o \
	rte.o

PKG_CONFIG=
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PKG_CONFIG) 2>/dev/null`
PKG_CONFIG_LIBS=`pkg-config --libs $(PKG_CONFIG) 2>/dev/null`

CFLAGS= -O2 -g -D_FORTIFY_SOURCE=2 -Wall -Wno-multichar
CPPFLAGS= -std=c++11 $(CFLAGS)
DEFS= -DDEBUG
INCS= -Itinythreadpp/ -I../intercom/

LDFLAGS= -Wl,-z,defs -Wl,-z,relro -Wl,--as-needed -Wl,--no-undefined
LIBS= -levent -lpthread

$(PROGRAM): $(HEADERS) $(OBJS)
	g++ $(LDFLAGS) $+ -o $@ $(LIBS) $(PKG_CONFIG_LIBS)

../tools/dbcgen/dbcgen:
	$(MAKE) -C ../tools/dbcgen/

.cpp.o: $(HEADERS)
	g++ -o $@ $(DEFS) $(INCS) $(CPPFLAGS) $(PKG_CONFIG_CFLAGS) -c $<

.c.o: $(HEADERS)
	gcc -o $@ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS) -c $<

%.o: ../intercom/%.cpp
	g++ -o $@ $(DEFS) $(CPPFLAGS) -c $<

%.o: ../intercom/%.c
	gcc -o $@ $(DEFS) $(CFLAGS) -c $<

%.dbc.h: ../panel/dbc/%.dbc ../tools/dbcgen/dbcgen
	../tools/dbcgen/dbcgen -o $@ -e Car $<

clean:
	rm -f $(OBJS)
	rm -f $(PROGRAM)
	rm -f *.o *.a *.dbc.h *~ core

