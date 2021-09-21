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

all: cee_utils $(OBJS)

echo:
	@ echo "SRC: $(SRC)"
	@ echo "CEE_UTILS_SRC: $(CEE_UTILS_SRC)"

cee_utils: $(CEE_UTILS_OBJS) | $(CEE_UTILS_DIR)

$(OBJS): | $(OBJDIR)

$(CEE_UTILS_DIR):
	if [[ ! -d $@ ]]; then        \
	  ./scripts/get-cee-utils.sh; \
	fi
$(OBJDIR):
	mkdir -p $(OBJDIR)/$(CEE_UTILS_DIR)
$(LIBDIR):
	mkdir -p $(LIBDIR)

clean:
	rm -rf $(OBJDIR)

.PHONY: all clean cee_utils echo
