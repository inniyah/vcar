PROGRAM=argvparser_tests

all: $(PROGRAM)

OBJS_DIR=.
SOURCES = argvparser.cpp argvparser_tests.cpp
OBJS = $(addprefix $(OBJS_DIR)/,$(addsuffix .o,$(basename $(notdir $(SOURCES)))))

CPPSTD= -std=c++11
CFLAGS= -O2 -g -Wall
LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
DEFINES= 
INCLUDES= 
LIBS= 

$(PROGRAM): $(OBJS)
	g++ $(LDFLAGS) $(EXTRA_LDFLAGS) $+ -o $@ $(LIBS)

%.o: %.cpp
	g++ $(CPPSTD) $(DEFINES) $(INCLUDES) -o $@ -c $+ $(CFLAGS)

%.o: %.c
	gcc $(DEFINES) $(INCLUDES) -o $@ -c $+ $(CFLAGS)

clean:
	rm -f $(OBJS)
	rm -f $(PROGRAM)
	rm -f *.o *.a *~ core
