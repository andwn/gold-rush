MARSDEV ?= ${HOME}/mars
MARSBIN  = $(MARSDEV)/m68k-elf/bin
TOOLSBIN = $(MARSDEV)/bin

CC   = $(MARSBIN)/m68k-elf-gcc
AS   = $(MARSBIN)/m68k-elf-as
LD   = $(MARSBIN)/m68k-elf-ld
NM   = $(MARSBIN)/m68k-elf-nm
OBJC = $(MARSBIN)/m68k-elf-objcopy

# Z80 assembler used to build XGM
ASMZ80   = $(TOOLSBIN)/sjasm
# Stef's Tools
BINTOS   = $(TOOLSBIN)/bintos
LZ4W     = java -jar $(TOOLSBIN)/lz4w.jar
RESCOMP  = $(TOOLSBIN)/rescomp
XGMTOOL  = $(TOOLSBIN)/xgmtool
WAVTORAW = $(TOOLSBIN)/wavtoraw
# Sik's Tools
MDTILER = $(TOOLSBIN)/mdtiler
UFTC	= $(TOOLSBIN)/uftc

# We need to use the GCC version to locate the correct plugin & libgcc path
GCC_VER := $(shell $(CC) -dumpversion)
PLUGIN   = $(MARSDEV)/m68k-elf/libexec/gcc/m68k-elf/$(GCC_VER)
# Added for Windows compatibility via MSYS2
LTO_SO   = liblto_plugin.so
ifeq ($(OS),Windows_NT)
	LTO_SO = liblto_plugin-0.dll
endif

INCS     = -Isrc -Ires -Ires/video -Iinc
LIBS     = -L$(MARSDEV)/m68k-elf/lib/gcc/m68k-elf/$(GCC_VER) -lgcc
CCFLAGS  = -m68000 -Wall -Wextra -std=c99 -fno-builtin -fshort-enums
OPTIONS  = 
ASFLAGS  = -m68000 --register-prefix-optional
LDFLAGS  = -T $(MARSDEV)/ldscripts/sgdk.ld -nostdlib
Z80FLAGS = -isrc/xgm

RESS  = res/resources.res
Z80S  = $(wildcard src/xgm/*.s80)
CS    = $(wildcard src/*.c)
CS   += $(wildcard res/video/*.c)
SS    = $(wildcard src/*.s)
OBJS  = $(RESS:.res=.o)
OBJS += $(Z80S:.s80=.o)
OBJS += $(CS:.c=.o)
OBJS += $(SS:.s=.o)

# mdtiler scripts to convert pngs to tilesets and tilemaps
MDTS  = $(wildcard res/*.mdt)
PATS  = $(MDTS:.mdt=.pat)
MAPS  = $(MDTS:.mdt=.map)

#FRAMES = $(wildcard res/video/out16/*.mdt)
#FRAMEO = $(FRAMES:.mdt=.cpat)

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

gold.elf: boot.o $(PATS) $(OBJS)
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

%.cpat: %.pat
	$(LZ4W) p "$<" "$@" -s

%.pat: %.mdt
	$(MDTILER) $<


.PHONY: clean

clean:
	rm -f $(OBJS) $(PATS) $(MAPS)
	rm -f gold.bin gold.elf temp.bin symbol.txt boot.o
	rm -f src/xgm/pcm_drv.s src/xgm/pcm_drv.o80 src/xgm/pcm_drv.h out.lst
