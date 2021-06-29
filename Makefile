#
# server program
#
# Makefile used to build the software
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the MIT License
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
PREFIX=/usr/local
HOME=/home/aahanad/
REST_SERVER=$(HOME)/restServer
CRUN_LOCATION=$(HOME)/crun/.libs
CRUN_INCLUDE=$(HOME)/crun/src
CRUN_LIBOCISPEC=$(HOME)/crun/libocispec/src/
ULFIUS_LOCATION=$(HOME)/ulfius/src
ULFIUS_INCLUDE=$(HOME)/ulfius/include
EXAMPLE_INCLUDE=$(HOME)/ulfius/example_programs/include

CC=gcc
CFLAGS+=-c -Wall -I$(ULFIUS_INCLUDE) -I$(EXAMPLE_INCLUDE) -I$(REST_SERVER) -I$(CRUN_LIBOCISPEC) -D_REENTRANT $(ADDITIONALFLAGS) $(CPPFLAGS) -I$(CRUN_INCLUDE) -I/home/aahanad/crun
LIBS=-L$(LIBYDER_LOCATION) -lc -lulfius -lorcania -lcrun -lseccomp -lcap -lcriu -lyajl -ljansson -L$(ULFIUS_LOCATION) -L$(CRUN_LOCATION)

ifndef YDERFLAG
LIBS+= -lyder
endif

all: server

clean:
	rm -f *.o server

debug: ADDITIONALFLAGS=-DDEBUG -g -O0

debug: server

../../src/libulfius.so:
	cd $(ULFIUS_LOCATION) && $(MAKE) debug CURLFLAG=1 GNUTLSFLAG=1

server.o: server.c
	$(CC) $(CFLAGS) server.c -DDEBUG -g -O0

spec: spec.c
	$(CC) $(CFLAGS) spec.c -DDEBUG -g -O0

crun: crun.c
	$(CC) $(CFLAGS) crun.c -DDEBUG -g -O0
	
create: create.c
	$(CC) $(CFLAGS) create.c -DDEBUG -g -O0
	
server: crun create spec $(ULFIUS_LOCATION)/libulfius.so server.o
	$(CC) -o server server.o spec.o crun.o create.o $(LIBS)

test: server
	LD_LIBRARY_PATH=$(ULFIUS_LOCATION):${LD_LIBRARY_PATH} ./server

static: server.o
	$(CC) -o server server.o $(PREFIX)/lib/liborcania.a $(PREFIX)/lib/libyder.a $(PREFIX)/lib/libulfius.a -ljansson -lmicrohttpd -lpthread -lgnutls
