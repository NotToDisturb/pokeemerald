Std_MsgboxNPC: @ 8271315
	lock
	faceplayer
	setmessagewindow 0x0
	message 0x0
	waitmessage
	waitbuttonpress
	release
	return

Std_MsgboxSign: @ 8271320
	lockall
	setmessagewindow 0x0
	message 0x0
	waitmessage
	waitbuttonpress
	releaseall
	return

Std_MsgboxDefault: @ 827132A
	setmessagewindow 0x0
	message 0x0
	waitmessage
	waitbuttonpress
	return

Std_MsgboxYesNo: @ 8271332
	setmessagewindow 0x0
	message 0x0
	waitmessage
	yesnobox 20, 8
	return

Std_MsgboxGetPoints: @ 827133C
	setmessagewindow 0x0
	message 0x0
	playfanfare MUS_OBTAIN_B_POINTS
	waitfanfare
	waitmessage
	return

Std_10: @ 8271347
	pokenavcall 0x0
	waitmessage
	return

EventScript_UnusedReturn: @ 827134E
	return

Common_EventScript_SaveGame:: @ 827134F
	special SaveGame
	waitstate
	return
