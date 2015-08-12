PROGRAM=vcar
DATADIR=./data

all: $(PROGRAM)

#SOURCES = $(shell find . -name "*.c") $(shell find . -name "*.cpp")

SOURCES = \
	src/common/dynamicobject.cpp \
	src/common/soundenginealsa.cpp \
	src/common/vectortext.cpp \
	src/common/ogl.cpp \
	src/common/staticworldobject.cpp \
	src/carobject.cpp \
	src/controllerkey.cpp \
	src/controllerpad.cpp \
	src/main.cpp

OBJS_DIR=obj

OBJS = $(addprefix $(OBJS_DIR)/,$(addsuffix .o,$(basename $(notdir $(SOURCES)))))

PKG_CONFIG=gl glu ode alsa
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PKG_CONFIG)`
PKG_CONFIG_LIBS=`pkg-config --libs $(PKG_CONFIG)`

CFLAGS= -O2 -g -Wall
LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
DEFS = -DDATADIR=\"$(DATADIR)\"

INCS = -Isrc/common/ -Isrc/ 
LIBS = -lplibsg -lplibssg -lplibssgaux -lplibul -lglut -L.

test:
	echo $(OBJS)

$(PROGRAM): $(OBJS)
	g++ $(LDFLAGS) $+ -o $@ $(LIBS) $(PKG_CONFIG_LIBS)

obj/%.o: src/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

obj/%.o: src/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

obj/%.o: src/common/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

obj/%.o: src/common/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

clean:
	rm -fv $(OBJS)
	rm -fv $(PROGRAM)
	rm -fv `find . -name "*.so"`
	rm -fv `find . -name "*.so*"`
	rm -fv `find . -name "*.a"`
	rm -fv `find . -name "*~"`
