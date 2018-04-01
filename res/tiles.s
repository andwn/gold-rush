	.globl PAT_Font
	.globl PAT_Money
	.globl PAT_Layout
	.globl PAT_Notes
	.globl PAT_Bar
	.globl PAT_Words
	.globl PAT_Glow
	.globl PAT_Speed
	.globl PAT_EasyWin
	.globl PAT_EasyLose
	.globl PAT_NormalWin
	.globl PAT_NormalLose

	.align 0x20000

PAT_Font:		.incbin "res/font.pat"
PAT_Money:		.incbin "res/money.pat"
PAT_Layout:		.incbin "res/layout.pat"
PAT_Notes:		.incbin "res/notes.pat"
PAT_Bar:		.incbin "res/bar.pat"
PAT_Words:		.incbin "res/words.pat"
PAT_Glow:		.incbin "res/glow.pat"
PAT_Speed:		.incbin "res/speed.pat"
PAT_EasyWin:	.incbin "res/easywin.pat"
PAT_EasyLose:	.incbin "res/easylose.pat"
PAT_NormalWin:	.incbin "res/normalwin.pat"
PAT_NormalLose:	.incbin "res/normallose.pat"
