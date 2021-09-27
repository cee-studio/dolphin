PREFIX ?= /usr/local
SHELL  := /bin/bash

CC     ?= gcc

OBJDIR  := obj
LIBDIR  := lib
TESTDIR := test

CEE_UTILS_DIR  := cee-utils
CEE_UTILS_SRC  := $(wildcard $(CEE_UTILS_DIR)/*.c) 
CEE_UTILS_OBJS := $(CEE_UTILS_SRC:%.c=$(OBJDIR)/%.o)

SRC  := dolphin.c curl-websocket.c curl-websocket-utils.c $(wildcard $(CEE_UTILS_DIR)/*)
OBJS := $(SRC:%.c=$(OBJDIR)/%.o)
LIB  := $(LIBDIR)/libdolphin.a


CFLAGS += -O0 -g                     \
          -Wall -Wno-unused-function \
          -I. -I./$(CEE_UTILS_DIR)   \
          -DLOG_USE_COLOR

# generic compilation
$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: release

test: release
	$(MAKE) -C $(TESTDIR) -f test.mk

release: $(OBJS) $(LIB)

$(OBJS): | $(OBJDIR)

$(LIB): $(OBJS) | $(LIBDIR)
	$(AR) -cqsv $@ $?

# Create/Fetch dependencies
$(OBJDIR): | $(CEE_UTILS_DIR)
	mkdir -p $(OBJDIR)/$(CEE_UTILS_DIR)
$(LIBDIR):
	mkdir -p $(LIBDIR)
$(CEE_UTILS_DIR):
	if [[ ! -d $@ ]]; then        \
	  ./scripts/get-cee-utils.sh; \
	fi

echo:
	@ echo "SRC: $(SRC)"
	@ echo "CEE_UTILS_SRC: $(CEE_UTILS_SRC)"

clean:
	rm -rf $(OBJDIR) $(LIBDIR)
	$(MAKE) -C $(TESTDIR) -f test.mk clean
purge: clean
	rm -rf $(CEE_UTILS_DIR)

.PHONY: all clean cee_utils echo test
