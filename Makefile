MARSDEV ?= ${HOME}/mars
MARSBIN  = $(MARSDEV)/m68k-elf/bin
TOOLSBIN = $(MARSDEV)/bin

CC   = $(MARSBIN)/m68k-elf-gcc
AS   = $(MARSBIN)/m68k-elf-as
LD   = $(MARSBIN)/m68k-elf-ld
NM   = $(MARSBIN)/m68k-elf-nm
OBJC = $(MARSBIN)/m68k-elf-objcopy

ASMZ80   = $(TOOLSBIN)/sjasm
BINTOS   = $(TOOLSBIN)/bintos
RESCOMP  = $(TOOLSBIN)/rescomp
XGMTOOL  = $(TOOLSBIN)/xgmtool
WAVTORAW = $(TOOLSBIN)/wavtoraw

GCC_VER := $(shell $(CC) -dumpversion)
PLUGIN   = $(MARSDEV)/m68k-elf/libexec/gcc/m68k-elf/$(GCC_VER)
LTO_SO   = liblto_plugin.so
ifeq ($(OS),Windows_NT)
	LTO_SO = liblto_plugin-0.dll
endif

INCS     = -Isrc -Ires -Iinc
LIBS     = -L$(MARSDEV)/m68k-elf/lib/gcc/m68k-elf/$(GCC_VER) -lgcc
CCFLAGS  = -m68000 -Wall -Wextra -std=c99 -fno-builtin -fshort-enums
OPTIONS  = 
ASFLAGS  = -m68000 --register-prefix-optional
LDFLAGS  = -T $(MARSDEV)/ldscripts/sgdk.ld -nostdlib
Z80FLAGS = -isrc/xgm

RESS  = res/resources.res
Z80S  = $(wildcard src/xgm/*.s80)
CS    = $(wildcard src/*.c)
SS    = $(wildcard src/*.s)
OBJS  = $(RESS:.res=.o)
OBJS += $(Z80S:.s80=.o)
OBJS += $(CS:.c=.o)
OBJS += $(SS:.s=.o)

.SECONDARY: gold.elf

.PHONY: all release debug main-build

all: release

release: OPTIONS  = -O3 -fno-web -fno-gcse -fno-unit-at-a-time -fomit-frame-pointer
release: OPTIONS += -flto -fuse-linker-plugin
release: main-build

# Gens-KMod, BlastEm and UMDK support GDB tracing, enabled by this target
debug: OPTIONS = -g -O2 -DDEBUG -DKDEBUG
debug: main-build

main-build: gold.bin symbol.txt

# Cross reference symbol.txt with the addresses displayed in the crash handler
symbol.txt: gold.bin
	$(NM) --plugin=$(PLUGIN)/$(LTO_SO) -n gold.elf > symbol.txt

boot.o:
	$(AS) $(ASFLAGS) boot.s -o $@

gold.bin: gold.elf
	$(OBJC) -O binary $< temp.bin
	dd if=temp.bin of=$@ bs=8K conv=sync

gold.elf: boot.o $(OBJS)
	$(CC) -o $@ $(LDFLAGS) boot.o $(OBJS) $(LIBS)

%.o: %.c
	@echo "CC $<"
	@$(CC) $(CCFLAGS) $(OPTIONS) $(INCS) -c $< -o $@

%.o: %.s 
	@echo "AS $<"
	@$(AS) $(ASFLAGS) $< -o $@

%.s: %.res
	$(RESCOMP) $< $@

%.o80: %.s80
	$(ASMZ80) $(Z80FLAGS) $< $@ out.lst

%.s: %.o80
	$(BINTOS) $<


.PHONY: clean

clean:
	rm -f $(OBJS)
	rm -f gold.bin gold.elf temp.bin symbol.txt boot.o
	rm -f src/xgm/z80_xgm.s src/xgm/z80_xgm.o80 src/xgm/z80_xgm.h out.lst
