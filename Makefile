.PHONY: all build clean

CC=gcc
EXE=ddg
SOURCES=src/*.c
OBJECTS=*.o
INSTALLDIR=$(HOME)/.bin
CCFLAGS=`pkg-config --cflags libcurl jansson`
LINKFLAGS=`pkg-config --libs libcurl jansson`

all: build

build: $(SOURCES)
	$(CC) -o $(EXE) $(CCFLAGS) $(LINKFLAGS) $(SOURCES)

clean:
	rm -vf $(EXE) $(OBJECTS)

install: build
	mkdir -p $(INSTALLDIR)
	cp $(EXE) $(INSTALLDIR)/$(EXE)
