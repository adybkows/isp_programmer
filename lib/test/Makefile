CC = gcc

CFLAGS = -O3 -Wall -Werror --pedantic -std=c99
CPPFLAGS = -I../src
LDFLAGS = -L.. 

OUT = test_enumlpt

SRC = test_enumlpt.c

LIBS = -lispprog -lsetupapi -lcfgmgr32

####################

.PHONY: all clean

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(OUT) $(OUT).exe
