#
# Makefile for the HDFF firmware command interface library
#

VERSION = 0.1.0

LIB_NAME = libhdffcmd

LIB_OBJS = bitbuffer.o hdffcmd_base.o hdffcmd_osd.o

LIB_STATIC = $(LIB_NAME).a
LIB_SHARED = $(LIB_NAME)-$(VERSION).so

CC     = gcc
CFLAGS = -g -O2 -fPIC -Wall
AR     = ar -r

### Implicit rules:

%.o: %.c
	$(CC) $(CFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = $(CC) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

all: $(LIB_STATIC) $(LIB_SHARED)

$(LIB_STATIC): $(LIB_OBJS)
	$(AR) $(LIB_STATIC) $(LIB_OBJS)

$(LIB_SHARED): $(LIB_OBJS)
	$(CC) -fPIC -shared -o $(LIB_SHARED) $(LIB_OBJS)
	ln -sf $(LIB_SHARED) $(LIB_NAME).so

clean:
	rm -f $(LIB_OBJS) $(DEPFILE) $(LIB_STATIC) $(LIB_NAME)*.so
