#for debugging
CFLAGS = -O0 -g -Wall --pedantic -std=c99

#CFLAGS = -O3 -Wall --pedantic -std=c99
CPPFLAGS = -I. -I../include
LDFLAGS = -g

OUT = bin/ispprog.a

SRC = globals.c membuffer.c processors.c
SRC += os/windows/delays.c os/windows/getprogramversionstring.c

####################

OBJ = $(addprefix bin/,$(patsubst %.c,%.o,$(notdir $(SRC))))

.PHONY: all clean

all: bin $(OUT)

$(OUT): $(OBJ)
	$(AR) rcs $@ $^

bin/%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

bin/%.o: os/windows/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

bin:
	mkdir bin

clean:
	rm -rf bin
