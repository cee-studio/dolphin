TOP = ..
CC ?= gcc

SRC   = $(wildcard *.c)
EXES  = $(filter %.out, $(SRC:.c=.out))

CFLAGS  = -fno-exceptions -g -I$(TOP)/ -I$(TOP)/cee-utils
LDFLAGS = -L$(TOP)/lib -ldolphin -lm -lcurl

# generic compilation
%.out: %.c
	$(CC) $(CFLAGS) -g -o $@ $< $(LDFLAGS)

all: $(EXES)

clean:
	rm -f *.out

.PHONY : all clean
