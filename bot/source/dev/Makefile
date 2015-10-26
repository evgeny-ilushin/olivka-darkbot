# Generated automatically from Makefile.in by configure.
CC=gcc
CCP=g++
CFLAGS=-g -I/usr/include -Wall 
LD=-L/usr/lib
DISTDIR=main-release
DISTFILE=${DISTDIR}.tar.gz

OBJS=main.o

all: main

main: $(OBJS)
	$(CCP) $(OBJS) -o main $(LD)

clean:	
	rm *.o main
dist:
	rm -rf ${DISTDIR}; mkdir ${DISTDIR}
	cp ${DIST} ${DISTDIR}
	rm -f ${DISTFILE}
	tar cf - ${DISTDIR} | gzip > ${DISTFILE}
