PROGRAM=test

all: $(PROGRAM)

HEADERS = \
	os/os.h \
	can01.dbc.h

OBJS = \
	tinythread.o \
	intercom.o \
	main.o \
	can.o \
	com.o \
	os.o \
	task_handlers.o \
	isr_handlers.o

PKG_CONFIG=
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PKG_CONFIG) 2>/dev/null`
PKG_CONFIG_LIBS=`pkg-config --libs $(PKG_CONFIG) 2>/dev/null`

CFLAGS= -O2 -g -Wall
DEFS=
INCS=-I../tinythreadpp/source/ -I../intercom/

LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
LIBS=-levent -lpthread

$(PROGRAM): $(OBJS)
	g++ $(LDFLAGS) $+ -o $@ $(LIBS) $(PKG_CONFIG_LIBS)

../tools/dbcgen/dbcgen:
	$(MAKE) -C ../tools/dbcgen/

%.o: %.cpp $(HEADERS)
	g++ -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: %.c $(HEADERS)
	gcc -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: os/%.cpp $(HEADERS)
	g++ -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: os/%.c $(HEADERS)
	gcc -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: ../tinythreadpp/source/%.cpp
	g++ -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: ../tinythreadpp/source/%.c
	gcc -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: ../intercom/%.cpp
	g++ -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS)

%.o: ../intercom/%.c
	gcc -o $@ -c $< $(DEFS) $(INCS) $(CFLAGS)

%.dbc.h: ../panel/dbc/%.dbc ../tools/dbcgen/dbcgen
	../tools/dbcgen/dbcgen -o $@ -e Car $<

clean:
	rm -f $(OBJS)
	rm -f $(PROGRAM)
	rm -f *.o *.a *.dbc.h *~
