CC := gcc
CXX := g++

SYS := $(shell gcc -dumpmachine)

CFLAGS := -g -O0 -Wall -Wextra -Werror
CPPFLAGS := -I../include
LDFLAGS := -L../lib/$(SYS) 
LIBS := -lispprog -lsetupapi -lcfgmgr32

ifneq (,$(findstring mingw32,$(SYS)))
  LDFLAGS += -static-libstdc++ -static-libgcc 
endif

BIN := bin

DATA := testdata

SRC := test_enumlpt.cpp test_membuffer.c test_processors.c

####################

OUT := $(addprefix $(BIN)/,$(SRC:.c=) $(SRC:.cpp=))

.PHONY: all test clean

all: $(OUT)

$(OUT): $(BIN) ../lib/$(SYS)/libispprog.a

$(BIN)/%: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)

$(BIN)/%: %.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)

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
	#
	$(BIN)/test_processors ATmega8
	!( $(BIN)/test_processors foobar )
	$(BIN)/test_processors 0x1e 0x93 0x07
	!( $(BIN)/test_processors 0x77 0x77 0x77 )
	@echo "===  ALL TESTS OK  ==="

clean:
	rm -rf $(BIN) $(DATA)/*.out
