#Edit here if needed
PROG=server
####################

CC=g++
CPPPLAGS+=-Wall -Werror -g
CPPFLAGS+=-I/usr/include
CPPFLAGS+=-L/usr/lib
LDFLAGS= -lnsl

SOURCES=$(wildcard *.C)
OBJECTS=$(SOURCES:.C=.o)

default: all

#Edit here if needed
$(PROG):$(OBJECTS)
	$(CC) $(LDFLAGS) -o $(PROG) $(OBJECTS) 
####################

all:	$(PROG)
clean:
	rm -f $(PROG) $(OBJECTS) *~
%.o:	%.C
	$(CC) $(CPPFLAGS) -o $@ -c $<
