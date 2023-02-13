LD_LIBRARY_PATH=/usr/local/build/lib:${LD_LIBRARY_PATH}
LD_RUN_PATH=/usr/local/build/lib:${LD_RUN_PATH}
PATH=/usr/local/build/bin:/usr/local/bin:/usr/bin:/bin
CC = /usr/local/build/bin/g++
LD = /usr/local/build/bin/ld
CFLAGS = -g -pg -Wall -ftemplate-depth-30
LIBS = -lm 
LDFLAGS =-pg -Wl,--rpath -Wl,/usr/local/build/lib -static
MV = /bin/mv
TAR = /usr/local/bin/tar
MKDIR = /bin/mkdir


TOP        	:= ..
BIN 	   	:= $(TOP)/bin
INCLUDE    	+= -I$(TOP)/include
LIB	   	:= -L/usr/local/lib -L/usr/X11R6/lib
LIBS		+= -lX11 
DEFINES         := -DUSE_NAMESPACES -DUSE_STLPORT

TMPDIR     	:= $(TOP)/src

BIN 	 = ../bin
RM	 = rm -f 
MV	 = mv -f

source = main.cc bit_vector.cc shared_memory.cc arena.cc \
	 allocator.cc
objects = $(source:%.cc=%.o)
dependencies = $(source:%.cc=%.d)

all: allocator

allocator: $(dependencies) $(objects)
	$(CC) $(LDFLAGS) -o $@ $(objects) $(INCLUDE) $(LIB) $(LIBS)
	$(MV) $@ $(BIN)/

depend: $(dependencies)

modes:
	-chmod -f 600 *.cc *.o *.d *~ TAGS *.out Makefile $(INCLUDE)/*
	-chmod -f 700 RCS
	-chmod -f 700 $(BIN)
	-chmod -f 700 $(BIN)/*

%.o: %.cc
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(DEFINES) $(INCLUDE) $< -o $@ $(DEBUG_MESGS)


%.d: %.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $(DEFINES) $(INCLUDE) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

tags:
	$(RM) $@
	find ../ -follow \(  -name "*.[chCH]" -o -name "*.cc" \
		-o -name "*.hh" \) -print | etags - -C -o TAGS

clean:
	$(RM) *.o
	$(RM) TAGS
	$(RM) *.d

backup:
	@echo "Disk contents before backup:"
	-$(TAR) tvMf /dev/fd0 
	@echo "Backup with compression: tar cvzf /dev/fd0 "
	-$(TAR) cvMf /dev/fd0 ./*.cc ./Makefile ./*.h 
	@echo "Disk space in use: /bin/df -k /dev/fd0"
	/bin/df -k /dev/fd0
	@echo "Disk contents after backup:"
	-$(TAR) tvMf /dev/fd0

include $(dependencies)
