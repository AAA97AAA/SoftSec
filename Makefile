CXX=g++
CXXFLAGS=-Wall -Wextra -g -fno-stack-protector -z execstack -std=c++11 -m32
LIBS=-lpthread
SRC=$(PWD)/src
BUILDDIR=$(PWD)/build
OUTDIR=$(BUILDDIR)/bin
TEMPDIR=$(BUILDDIR)/tmp
MKDIR_P=mkdir -p
AR=ar
GINCLUDE=$(PWD)/src

export

.PHONY: all clean

all: | $(OUTDIR) $(TEMPDIR)
	$(MAKE) --directory=src all
	rm -rf $(TEMPDIR)

clean:
	rm -rf $(OUTDIR) $(TEMPDIR)

$(OUTDIR) $(TEMPDIR):
	$(MKDIR_P) $@
