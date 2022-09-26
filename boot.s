.section .data

	.globl v_err_reg
	.globl v_err_pc
	.globl v_err_addr
	.globl v_err_ext1
	.globl v_err_ext2
	.globl v_err_sr
	.globl v_err_type
v_err_reg:	ds.l 16
v_err_pc:	ds.l 1
v_err_addr:	ds.l 1
v_err_ext1:	ds.w 1
v_err_ext2: ds.w 1
v_err_sr:	ds.w 1
v_err_type:	ds.w 1

.section .text.keepboot

	.org    0x00000000

StartOfRom:
		dc.l 0x000000, _start
		dc.l BusError, AddressError, IllegalInstr, ZeroDivide
		dc.l 0, 0
	.rept 5
		dc.l 0, 0, 0, 0
	.endr
		dc.l HBlank, 0, VBlank, 0
	.rept 8
		dc.l 0, 0, 0, 0
	.endr
RomHeader:
		.ascii	"SEGA MEGA DRIVE "
		.ascii	"GRIND   2018.APR"
		.ascii	"Gold Rush                                       "
		.ascii	"Gold Rush                                       "
		.ascii	"GM ANDYG010-00"
		dc.w	0
		.ascii	"J               "
		dc.l	0x000000
		dc.l	0x1FFFFF
		dc.l	0xFF0000
		dc.l	0xFFFFFF
	.rept 16
		dc.l	0x20202020
	.endr
		.ascii	"JUE             "

_start:
        move    #0x2700,sr
        tst.l   0xA10008
        bne.s   SkipJoyDetect
        tst.w   0xA1000C
SkipJoyDetect:
		move.b	(0xA10001),d0
        andi.b  #0x0F,d0
        beq.s   NoTMSS
        move.l  #0x53454741,0xA14000
NoTMSS:
        move.w  (0xC00004),d0
        moveq   #0x00,d0
        movea.l d0,a6
        move    a6,usp
        move.w  #0x0100,(0xA11100)
        move.w  #0x0100,(0xA11200)
        jmp     main


RegDump:
        move.l d0,v_err_reg+0
        move.l d1,v_err_reg+4
        move.l d2,v_err_reg+8
        move.l d3,v_err_reg+12
        move.l d4,v_err_reg+16
        move.l d5,v_err_reg+20
        move.l d6,v_err_reg+24
        move.l d7,v_err_reg+28
        move.l a0,v_err_reg+32
        move.l a1,v_err_reg+36
        move.l a2,v_err_reg+40
        move.l a3,v_err_reg+44
        move.l a4,v_err_reg+48
        move.l a5,v_err_reg+52
        move.l a6,v_err_reg+56
        move.l a7,v_err_reg+60
        rts

AddressDump:
        move.w 4(sp),v_err_ext1
        move.l 6(sp),v_err_addr
        move.w 10(sp),v_err_ext2
        move.w 12(sp),v_err_sr
        move.l 14(sp),v_err_pc
        jmp RegDump

IllegalDump:
        move.w 4(sp),v_err_sr
        move.l 6(sp),v_err_pc
        move.w 10(sp),v_err_ext1
        jmp RegDump

ZeroDump:
        move.w 4(sp),v_err_sr
        move.l 6(sp),v_err_pc
        jmp RegDump

BusError:
		move.w	#0,(v_err_type)
        jsr AddressDump
        movem.l d0-d1/a0-a1,-(sp)
		jsr _error
        movem.l (sp)+,d0-d1/a0-a1
        rte

AddressError:
		move.w	#1,(v_err_type)
        jsr AddressDump
        movem.l d0-d1/a0-a1,-(sp)
        jsr _error
        movem.l (sp)+,d0-d1/a0-a1
        rte

IllegalInstr:
		move.w	#2,(v_err_type)
        jsr IllegalDump
        movem.l d0-d1/a0-a1,-(sp)
        jsr _error
        movem.l (sp)+,d0-d1/a0-a1
        rte

ZeroDivide:
		move.w	#3,(v_err_type)
        jsr ZeroDump
        movem.l d0-d1/a0-a1,-(sp)
        jsr _error
        movem.l (sp)+,d0-d1/a0-a1
        rte

VBlank:
HBlank:
        rte
