* Here comes the gay ghost

	.globl PAT_Vid01
	.globl PAT_Vid02
	.globl PAT_Vid03
	.globl PAT_Vid04
	.globl PAT_Vid05
	.globl PAT_Vid06
	.globl PAT_Vid07
	.globl PAT_Vid08
	.globl PAT_Vid09
	.globl PAT_Vid10
	.globl PAT_Vid11
	.globl PAT_Vid12
	.globl PAT_Vid13
	.globl PAT_Vid14
	.globl PAT_Vid15
	.globl PAT_Vid16
	.globl PAT_Vid17
	.globl PAT_Vid18
	.globl PAT_Vid19
	.globl PAT_Vid20
	.globl PAT_Vid21
	.globl PAT_Vid22
	.globl PAT_Vid23
	.globl PAT_Vid24
	.globl PAT_Vid25
	.globl PAT_Vid26
	.globl PAT_Vid27
	.globl PAT_Vid28
	.globl PAT_Vid29
	.globl PAT_Vid30
	.globl PAT_Vid31
	.globl PAT_Vid32
	.globl PAT_Vid33
	.globl PAT_Vid34
	.globl PAT_Vid35
	.globl PAT_Vid36
	.globl PAT_Vid37
	.globl PAT_Vid38
	.globl PAT_Vid39
	.globl PAT_Vid40
	.globl PAT_Vid41
	.globl PAT_Vid42
	.globl PAT_Vid43
	.globl PAT_Vid44
	.globl PAT_Vid45
	.globl PAT_Vid46
	.globl PAT_Vid47
	.globl PAT_Vid48
	.globl PAT_Vid49
	.globl PAT_Vid50
	.globl PAT_Vid51
	.globl PAT_Vid52
	.globl PAT_Vid53
	.globl PAT_Vid54
	.globl PAT_Vid55
	.globl PAT_Vid56
	.globl PAT_Vid57
	.globl PAT_Vid58
	.globl PAT_Vid59
	.globl PAT_Vid60
	.globl PAT_Vid61
	.globl PAT_Vid62
	.globl PAT_Vid63

* Need to align DMA uploaded resources to 128K
	.align 0x20000

PAT_Vid01:	.incbin "res/video/01.pat"
PAT_Vid02:	.incbin "res/video/02.pat"
PAT_Vid03:	.incbin "res/video/03.pat"
PAT_Vid04:	.incbin "res/video/04.pat"
PAT_Vid05:	.incbin "res/video/05.pat"
PAT_Vid06:	.incbin "res/video/06.pat"
PAT_Vid07:	.incbin "res/video/07.pat"
PAT_Vid08:	.incbin "res/video/08.pat"
PAT_Vid09:	.incbin "res/video/09.pat"
PAT_Vid10:	.incbin "res/video/10.pat"
PAT_Vid11:	.incbin "res/video/11.pat"
PAT_Vid12:	.incbin "res/video/12.pat"
PAT_Vid13:	.incbin "res/video/13.pat"
PAT_Vid14:	.incbin "res/video/14.pat"
PAT_Vid15:	.incbin "res/video/15.pat"
PAT_Vid16:	.incbin "res/video/16.pat"

* The 17th frame is where the 128K boundary overlaps
	.align 0x20000

PAT_Vid17:	.incbin "res/video/17.pat"
PAT_Vid18:	.incbin "res/video/18.pat"
PAT_Vid19:	.incbin "res/video/19.pat"
PAT_Vid20:	.incbin "res/video/20.pat"
PAT_Vid21:	.incbin "res/video/21.pat"
PAT_Vid22:	.incbin "res/video/22.pat"
PAT_Vid23:	.incbin "res/video/23.pat"
PAT_Vid24:	.incbin "res/video/24.pat"
PAT_Vid25:	.incbin "res/video/25.pat"
PAT_Vid26:	.incbin "res/video/26.pat"
PAT_Vid27:	.incbin "res/video/27.pat"
PAT_Vid28:	.incbin "res/video/28.pat"
PAT_Vid29:	.incbin "res/video/29.pat"
PAT_Vid30:	.incbin "res/video/30.pat"
PAT_Vid31:	.incbin "res/video/31.pat"
PAT_Vid32:	.incbin "res/video/32.pat"

	.align 0x20000

PAT_Vid33:	.incbin "res/video/33.pat"
PAT_Vid34:	.incbin "res/video/34.pat"
PAT_Vid35:	.incbin "res/video/35.pat"
PAT_Vid36:	.incbin "res/video/36.pat"
PAT_Vid37:	.incbin "res/video/37.pat"
PAT_Vid38:	.incbin "res/video/38.pat"
PAT_Vid39:	.incbin "res/video/39.pat"
PAT_Vid40:	.incbin "res/video/40.pat"
PAT_Vid41:	.incbin "res/video/41.pat"
PAT_Vid42:	.incbin "res/video/42.pat"
PAT_Vid43:	.incbin "res/video/43.pat"
PAT_Vid44:	.incbin "res/video/44.pat"
PAT_Vid45:	.incbin "res/video/45.pat"
PAT_Vid46:	.incbin "res/video/46.pat"
PAT_Vid47:	.incbin "res/video/47.pat"
PAT_Vid48:	.incbin "res/video/48.pat"

	.align 0x20000

PAT_Vid49:	.incbin "res/video/49.pat"
PAT_Vid50:	.incbin "res/video/50.pat"
PAT_Vid51:	.incbin "res/video/51.pat"
PAT_Vid52:	.incbin "res/video/52.pat"
PAT_Vid53:	.incbin "res/video/53.pat"
PAT_Vid54:	.incbin "res/video/54.pat"
PAT_Vid55:	.incbin "res/video/55.pat"
PAT_Vid56:	.incbin "res/video/56.pat"
PAT_Vid57:	.incbin "res/video/57.pat"
PAT_Vid58:	.incbin "res/video/58.pat"
PAT_Vid59:	.incbin "res/video/59.pat"
PAT_Vid60:	.incbin "res/video/60.pat"
PAT_Vid61:	.incbin "res/video/61.pat"
PAT_Vid62:	.incbin "res/video/62.pat"
PAT_Vid63:	.incbin "res/video/63.pat"
