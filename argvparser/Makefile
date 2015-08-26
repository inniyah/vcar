rm=/bin/rm -f
CC= g++ -Wall
DEFS= 
PROGNAME= argvparsertest
INCLUDES=  
LIBS= 


DEFINES= $(INCLUDES) $(DEFS)
CFLAGS= -g $(DEFINES)

SRCS = tests.cpp argvparser.cpp 

OBJS = tests.o argvparser.o 

.cpp.o:
	$(rm) $@
	$(CC) $(CFLAGS) -c $*.cpp

all: $(PROGNAME)

$(PROGNAME) : $(OBJS)
	$(CC) $(CFLAGS) -o $(PROGNAME) $(OBJS) $(LIBS)

clean:
	$(rm) $(OBJS) $(PROGNAME) core *~
