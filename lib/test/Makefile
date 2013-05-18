CC := gcc

SYS := $(shell gcc -dumpmachine)

CFLAGS := -O3 -Wall -Werror --pedantic -std=c99
CPPFLAGS := -I../src
LDFLAGS := -L../lib/$(SYS) 

BIN := bin/$(SYS)

OUT := $(BIN)/test_enumlpt

SRC := test_enumlpt.c

LIBS := -lispprog -lsetupapi -lcfgmgr32

####################

.PHONY: all clean

all: $(OUT)

$(OUT): $(SRC)
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf $(BIN)
