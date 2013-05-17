#for debugging
CFLAGS = -std=c99 -O0 -g

#CFLAGS = -std=c99 -O3
CPPFLAGS = -I. -I../include
LDFLAGS = -g

OUT = bin/ispprog.a

SRC = globals.c processors.c #membuffer.c

####################

OBJ = $(addprefix bin/,$(SRC:.c=.o))

.PHONY: all clean

all: bin $(OUT)

$(OUT): $(OBJ)
	$(AR) rcs $@ $^

bin/%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

bin:
	mkdir bin

clean:
	rm -rf bin
