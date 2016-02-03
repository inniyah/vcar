LIBRARY=libCanDbc

all: static
static: $(LIBRARY).a
shared: $(LIBRARY).so

MAJOR=0
MINOR=0

SOURCES = \
	dbc.tab.c \
	dbc.yy.c \
	dbcModel.c \
	dbcReader.c \
	dbcWriter.c \
	hashtable.c \
	hashtable_itr.c \
	messageHash.c \
	signalFormat.c \
	busAssignment.c \
	messageDecoder.c \
	ascReader.c \
	showDbc.c

SHARED_OBJS = $(SOURCES:.c=.shared.o)
STATIC_OBJS = $(SOURCES:.c=.static.o)

CFLAGS=-O2 -g -Wall 
STATIC_CFLAGS= -O2 -g -Wall $(CFLAGS)
SHARED_CFLAGS= $(STATIC_CFLAGS) -fPIC

LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
LIBS=$(PKG_CONFIG_LIBS)
INCS=
DEFS=

$(LIBRARY).so.$(MAJOR).$(MINOR): $(SHARED_OBJS)
	g++ $(LDFLAGS) $(EXTRA_LDFLAGS) -shared \
		-Wl,-soname,$(LIBRARY).so.$(MAJOR) \
		-o $(LIBRARY).so.$(MAJOR).$(MINOR) \
		$+ -o $@ $(LIBS)

$(LIBRARY).so: $(LIBRARY).so.$(MAJOR).$(MINOR)
	rm -f $@.$(MAJOR)
	ln -s $@.$(MAJOR).$(MINOR) $@.$(MAJOR)
	rm -f $@
	ln -s $@.$(MAJOR) $@

$(LIBRARY).a: $(STATIC_OBJS)
	ar cr $@ $+

%.so : %.o
	g++ $(LDFLAGS) $(LIBS) -shared $^ -o $@

%.shared.o: %.cpp
	g++ -o $@ -c $+ $(SHARED_CFLAGS)

%.shared.o: %.c
	gcc -o $@ -c $+ $(SHARED_CFLAGS)

%.shared.o: model/%.cpp
	g++ -o $@ -c $+ $(SHARED_CFLAGS)

%.shared.o: model/%.c
	gcc -o $@ -c $+ $(SHARED_CFLAGS)

%.shared.o: hashtable/%.cpp
	g++ -o $@ -c $+ $(SHARED_CFLAGS)

%.shared.o: hashtable/%.c
	gcc -o $@ -c $+ $(SHARED_CFLAGS)

%.static.o: %.cpp
	g++ -o $@ $(DEFS) $(INCS) -c $+ $(STATIC_CFLAGS)

%.static.o: %.c
	gcc -o $@ $(DEFS) $(INCS) -c $+ $(STATIC_CFLAGS)

%.static.o: model/%.cpp
	g++ -o $@ $(DEFS) $(INCS) -c $+ $(STATIC_CFLAGS)

%.static.o: model/%.c
	gcc -o $@ $(DEFS) $(INCS) -c $+ $(STATIC_CFLAGS)

%.static.o: hashtable/%.cpp
	g++ -o $@ $(DEFS) $(INCS) -c $+ $(STATIC_CFLAGS)

%.static.o: hashtable/%.c
	gcc -o $@ $(DEFS) $(INCS) -c $+ $(STATIC_CFLAGS)

%.tab.c %.tab.h: model/%.y
	bison -d $<

%.yy.c: model/%.l %.tab.h
	flex -o $@ $<

clean:
	rm -f $(SHARED_OBJS)
	rm -f $(STATIC_OBJS)
	rm -f *.tab.c *.tab.h *.yy.c *.so *.so* *.a *~
