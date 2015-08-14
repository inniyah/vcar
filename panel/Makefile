PROGRAM=panel

all: $(PROGRAM)

OBJS = \
	CarSvgBox.o \
	panel.o \
	panel_cb.o \
	main.o

PKG_CONFIG=cairo librsvg-2.0
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PKG_CONFIG) 2>/dev/null` `fltk-config --cflags`
PKG_CONFIG_LIBS=`pkg-config --libs $(PKG_CONFIG) 2>/dev/null` `fltk-config --ldflags --libs`

CFLAGS= -O2 -g -Wall
DEFS= 
INCS= 

LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
LIBS=

$(PROGRAM): $(OBJS)
	g++ $(LDFLAGS) $+ -o $@ $(LIBS) $(PKG_CONFIG_LIBS)

%.o: %.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: %.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.h %.cpp: %.fl
	fluid -c $<

clean:
	rm -f $(OBJS)
	rm -f $(PROGRAM)
	rm -f *.o *.a *~
