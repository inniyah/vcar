PROGRAM=panel

all: ../libcandbc/libCanDbc.a $(PROGRAM)

OBJS = \
	intercom.o \
	tinythread.o \
	AbstractSvgBox.o \
	CarSvgBox.o \
	CarState.o \
	DialSvgBox.o \
	OilWaterSvgBox.o \
	panel.o \
	panel_cb.o \
	main.o

PKG_CONFIG=cairo librsvg-2.0
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PKG_CONFIG) 2>/dev/null` `fltk-config --cflags`
PKG_CONFIG_LIBS=`pkg-config --libs $(PKG_CONFIG) 2>/dev/null` `fltk-config --ldflags --libs`

CFLAGS= -O2 -g -Wall
DEFS= 
INCS=  -I../tinythreadpp/source/ -I../intercom/  -I../libcandbc/ -I../libcandbc/model/

LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
LIBS= -L../libcandbc -lCanDbc -lpthread

$(PROGRAM): $(OBJS)
	g++ $(LDFLAGS) $+ -o $@ $(LIBS) $(PKG_CONFIG_LIBS)

../libcandbc/libCanDbc.a:
	$(MAKE) -C ../libcandbc/ static

%.o: %.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: %.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.h %.cpp: %.fl
	fluid -c $<

%.o: ../tinythreadpp/source/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../tinythreadpp/source/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../intercom/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.o: ../intercom/%.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS)

%.tab.c %.tab.h: %.y
	bison -d $<

%.yy.c: %.l %.tab.h
	flex -o $@ $<

clean:
	rm -fv $(OBJS)
	rm -fv $(PROGRAM)
	rm -fv $(addsuffix .h,$(basename $(shell find . -maxdepth 1 -name "*.fl")))
	rm -fv $(addsuffix .cpp,$(basename $(shell find . -maxdepth 1 -name "*.fl")))
	rm -fv svgtest.svg pngtest.png epstest.eps pdftest.pdf cstest.cs
	rm -fv *.o *.a *~
