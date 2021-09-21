PREFIX ?= /usr/local
SHELL  := /bin/bash

CC     ?= gcc
OBJDIR := obj
LIBDIR := lib

SRC  := $(wildcard *.c)
OBJS := $(SRC:%.c=$(OBJDIR)/%.o)

CEE_UTILS_DIR  := cee-utils
CEE_UTILS_SRC  := $(wildcard $(CEE_UTILS_DIR)/*.c) 
CEE_UTILS_OBJS := $(CEE_UTILS_SRC:%.c=$(OBJDIR)/%.o)


CFLAGS += -O0 -g                     \
          -Wall -Wno-unused-function \
          -I. -I./$(CEE_UTILS_DIR)   \
          -DLOG_USE_COLOR

# generic compilation
$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: release

release: $(OBJS)

$(OBJS): | $(OBJDIR)

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
	rm -rf $(OBJDIR)
purge: clean
	rm -rf $(CEE_UTILS_DIR)

.PHONY: all clean cee_utils echo
