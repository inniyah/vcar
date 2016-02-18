PROGRAM=ecu

all: $(PROGRAM)

HEADERS = \
	os.h \
	os_cfg.h \
	sys_cfg.h \
	ISwc.h \
	Swc.h \
	can01.dbc.h

SOURCES = \
	tinythreadpp/tinythread.cpp \
	intercom.cpp \
	sys_os.cpp \
	sys_main.cpp\
	sys_com.cpp \
	sys_can.cpp \
	sys_pwm.cpp \
	TaskHandlers.cpp \
	IsrHandlers.cpp \
	SwcBackLight.cpp \
	SwcIntLight.cpp \
	rte.cpp

OBJS_DIR=./
OBJS = $(addprefix $(OBJS_DIR)/,$(addsuffix .o,$(basename $(notdir $(SOURCES)))))

PKG_CONFIG=
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PKG_CONFIG) 2>/dev/null`
PKG_CONFIG_LIBS=`pkg-config --libs $(PKG_CONFIG) 2>/dev/null`

CFLAGS= -O2 -g -D_FORTIFY_SOURCE=2 -Wall -Wno-multichar
CPPFLAGS= -std=c++11 $(CFLAGS)
DEFS= 
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

