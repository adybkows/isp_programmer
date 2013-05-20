CC := gcc

SYS := $(shell gcc -dumpmachine)

CFLAGS := -g -O0 -Wall -Werror --pedantic -std=c99
CPPFLAGS := -I../src
LDFLAGS := -L../lib/$(SYS) 
LIBS := -lispprog -lsetupapi -lcfgmgr32

BIN := bin

DATA := testdata

SRC := test_enumlpt.c test_membuffer.c

####################

OUT := $(addprefix $(BIN)/,$(SRC:.c=))

.PHONY: all test clean

all: $(OUT)

$(OUT): $(BIN) ../lib/$(SYS)/libispprog.a

$(BIN)/%: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)

$(BIN):
	mkdir -p $(BIN)

test:
	$(BIN)/test_enumlpt
	#
	!( $(BIN)/test_membuffer ishex $(DATA)/test1.bin )
	!( $(BIN)/test_membuffer ishexfname $(DATA)/test1.bin )
	$(BIN)/test_membuffer bin2hex $(DATA)/test1.bin $(DATA)/test1.hex.out
	diff -q --strip-trailing-cr $(DATA)/test1.hex.out $(DATA)/test1.hex
	#
	$(BIN)/test_membuffer ishex $(DATA)/test1.hex
	$(BIN)/test_membuffer ishexfname $(DATA)/test1.hex
	$(BIN)/test_membuffer hex2bin $(DATA)/test1.hex $(DATA)/test1.bin.out
	diff -q $(DATA)/test1.bin.out $(DATA)/test1.bin

clean:
	rm -rf $(BIN) $(DATA)/*.out
