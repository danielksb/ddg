.PHONY: all clean

CC=gcc
EXE=ddg

INSTALLDIR=$(HOME)/.bin
VPATH=src
BUILDDIR=build

CFLAGS=`pkg-config --cflags libcurl jansson`
LDFLAGS=`pkg-config --libs libcurl jansson`

all: $(BUILDDIR)/$(EXE)
	cp $(BUILDDIR)/$(EXE) .

%: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $^

$(BUILDDIR):
	mkdir -p $@

clean:
	rm -rvf $(EXE) $(BUILDDIR)

install: build
	mkdir -p $(INSTALLDIR)
	cp $(BUILDDIR)/$(EXE) $(INSTALLDIR)/$(EXE)

# Dependencies
#

OBJS=$(BUILDDIR)/ddg.o $(BUILDDIR)/http.o $(BUILDDIR)/searchresult.o $(BUILDDIR)/jsonparse.o 

$(BUILDDIR)/$(EXE): $(OBJS)| $(BUILDDIR)
$(OBJS): | $(BUILDDIR)
