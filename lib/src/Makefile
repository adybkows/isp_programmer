CC := gcc

SYS := $(shell gcc -dumpmachine)

ifneq (, $(findstring linux, $(SYS)))
  # Do linux things
else
ifeq (x86_64-w64-mingw32, $(SYS))
  # mingw 64-bit target
else
ifeq (mingw32, $(SYS))
  # mingw 32-bit target
else
ifeq (i686-pc-cygwin, $(SYS))
  # cygwin 32-bit target
else
  $(error Unsupported target.)
endif
endif
endif
endif

#for debugging
CFLAGS := -O0 -g -Wall -Wextra -Werror
#CFLAGS := -O3 -Wall -Wextra -Werror
CPPFLAGS := -I. -I../include
LDFLAGS := -g

OUT := ../lib/$(SYS)/libispprog.a

SRC := dataflash.c globals.c isp.c isp_eeprom.c isp_flash.c isp_fusebits.c isp_lockbits.c isp_usersign.c \
  membuffer.c pinsio.c processors.c serialflash.c spi.c

WINDOWS_SRC := delays.c enumlpt.c inpout32.c getprogramversionstring.c portsio.c registryisp.c serialflash.c

####################

SRC += $(addprefix os/windows/,$(WINDOWS_SRC))

BIN := bin/$(SYS)

OBJ := $(addprefix $(BIN)/,$(patsubst %.c,%.o,$(notdir $(SRC))))

.PHONY: all clean

all: $(BIN) $(OUT)

$(OUT): $(OBJ)
	mkdir -p $(dir $@)
	$(AR) rcs $@ $^

$(BIN)/%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(BIN)/%.o: os/windows/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(BIN):
	mkdir -p $(BIN)

clean:
	rm -rf $(BIN) $(OUT)
