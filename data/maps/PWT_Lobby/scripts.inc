PWT_Lobby_MapScripts::
	.byte 0


PWT_Lobby_EventScript_Attendant::
	lock
	faceplayer
	msgbox PWT_Lobby_EventScript_Attendant_Text_0, MSGBOX_DEFAULT
	waitmessage
	dynmultichoice 17, 6, FALSE, 3, 0, DYN_MULTICHOICE_CB_NONE, PWT_Lobby_EventScript_Attendant_Text_1, PWT_Lobby_EventScript_Attendant_Text_2, PWT_Lobby_EventScript_Attendant_Text_3
	switch VAR_RESULT
	case 0, PWT_Lobby_EventScript_Attendant_3
	case 1, PWT_Lobby_EventScript_Attendant_4
	case 2, PWT_Lobby_EventScript_Attendant_5
	case MULTI_B_PRESSED, PWT_Lobby_EventScript_Attendant_5
PWT_Lobby_EventScript_Attendant_1:
	special SavePlayerParty
	pwt_checkineligible
	goto_if_eq VAR_0x8004, TRUE, PWT_Lobby_EventScript_PWT_NotEnoughValidMons
	msgbox PWT_Lobby_EventScript_Attendant_Text_5, MSGBOX_DEFAULT
	fadescreen FADE_TO_BLACK
	callnative RemoveFollowingPokemon
	copyvar VAR_0x8004, FRONTIER_LVL_OPEN
	switch VAR_FRONTIER_BATTLE_MODE
	case PWT_MODE_SINGLES, PWT_Lobby_EventScript_Attendant_8
	case PWT_MODE_DOUBLES, PWT_Lobby_EventScript_Attendant_9
PWT_Lobby_EventScript_Attendant_6:
	special ChoosePartyForPWT
	waitstate
	goto_if_eq VAR_RESULT, 0, PWT_Lobby_EventScript_PWT_LoadPartyCancel
	msgbox PWT_Lobby_EventScript_Attendant_Text_6, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, PWT_Lobby_EventScript_Attendant_11
	case MULTI_B_PRESSED, PWT_Lobby_EventScript_Attendant_11
	case YES, PWT_Lobby_EventScript_Attendant_12
	return

PWT_Lobby_EventScript_Attendant_3:
	setvar VAR_FRONTIER_FACILITY, FACILITY_PWT
	setvar VAR_FRONTIER_BATTLE_MODE, PWT_MODE_SINGLES
	goto PWT_Lobby_EventScript_Attendant_1

PWT_Lobby_EventScript_Attendant_4:
	setvar VAR_FRONTIER_FACILITY, FACILITY_PWT
	setvar VAR_FRONTIER_BATTLE_MODE, PWT_MODE_DOUBLES
	goto PWT_Lobby_EventScript_Attendant_1

PWT_Lobby_EventScript_Attendant_5:
	msgbox PWT_Lobby_EventScript_Attendant_Text_4, MSGBOX_DEFAULT
	goto PWT_Lobby_EventScript_PWT_End
	goto PWT_Lobby_EventScript_Attendant_1

PWT_Lobby_EventScript_Attendant_8:
	setvar VAR_0x8005, PWT_PARTY_SIZE
	goto PWT_Lobby_EventScript_Attendant_6

PWT_Lobby_EventScript_Attendant_9:
	setvar VAR_0x8005, PWT_DOUBLES_PARTY_SIZE
	goto PWT_Lobby_EventScript_Attendant_6

PWT_Lobby_EventScript_Attendant_11:
	goto PWT_Lobby_EventScript_PWT_LoadPartyCancel
	return

PWT_Lobby_EventScript_Attendant_12:
	goto PWT_Lobby_EventScript_PWT_SaveBeforeChallenge
	return


PWT_Lobby_EventScript_PWT_SaveBeforeChallenge::
	pwt_set PWT_DATA_SELECTED_MON_ORDER
	pwt_init
	special LoadPlayerParty
	closemessage
	delay 2
	call Common_EventScript_SaveGame
	setvar VAR_TEMP_CHALLENGE_STATUS, 255
	goto_if_eq VAR_RESULT, 0, PWT_Lobby_EventScript_PWT_CancelChallengeSaveFailed
	switch VAR_FRONTIER_BATTLE_MODE
	case PWT_MODE_SINGLES, PWT_Lobby_EventScript_PWT_SaveBeforeChallenge_3
	case PWT_MODE_DOUBLES, PWT_Lobby_EventScript_PWT_SaveBeforeChallenge_4
PWT_Lobby_EventScript_PWT_SaveBeforeChallenge_1:
	pwt_inittrainers
	msgbox PWT_Lobby_EventScript_PWT_SaveBeforeChallenge_Text_0, MSGBOX_DEFAULT
	goto PWT_Lobby_EventScript_PWT_EnterChallenge
	return

PWT_Lobby_EventScript_PWT_SaveBeforeChallenge_3:
	setvar VAR_0x8005, PWT_PARTY_SIZE
	goto PWT_Lobby_EventScript_PWT_SaveBeforeChallenge_1

PWT_Lobby_EventScript_PWT_SaveBeforeChallenge_4:
	setvar VAR_0x8005, PWT_DOUBLES_PARTY_SIZE
	goto PWT_Lobby_EventScript_PWT_SaveBeforeChallenge_1


PWT_Lobby_EventScript_PWT_EnterChallenge::
	special SavePlayerParty
	pwt_setpartyorder PWT_PARTY_SIZE
	pwt_settrainers
	msgbox PWT_Lobby_EventScript_PWT_EnterChallenge_Text_0, MSGBOX_DEFAULT
	closemessage
	call PWT_Lobby_EventScript_WalkToDoor
	special HealPlayerParty
	warp MAP_PWT_CORRIDOR, 23, 6
	setvar VAR_TEMP_CHALLENGE_STATUS, 0
	waitstate
	end


PWT_Lobby_EventScript_WalkToDoor::
	applymovement LOCALID_PWT_ATTENDANT, PWT_Lobby_EventScript_WalkToDoor_Movement_0
	applymovement LOCALID_PLAYER, PWT_Lobby_EventScript_WalkToDoor_Movement_0
	waitmovement 0
	opendoor 5, 4
	waitdooranim
	applymovement LOCALID_PWT_ATTENDANT, PWT_Lobby_EventScript_WalkToDoor_Movement_1
	applymovement LOCALID_PLAYER, PWT_Lobby_EventScript_WalkToDoor_Movement_2
	waitmovement 0
	closedoor 5, 4
	waitdooranim
	return


PWT_Lobby_EventScript_PWT_CancelChallengeSaveFailed::
	msgbox PWT_Lobby_EventScript_PWT_CancelChallengeSaveFailed_Text_0, MSGBOX_DEFAULT
	goto PWT_Lobby_EventScript_PWT_End
	return


PWT_Lobby_EventScript_PWT_LoadPartyCancel::
	special LoadPlayerParty
	callnative UpdateFollowingPokemon
	msgbox PWT_Lobby_EventScript_PWT_LoadPartyCancel_Text_0, MSGBOX_DEFAULT
	goto PWT_Lobby_EventScript_PWT_End
	return


PWT_Lobby_EventScript_PWT_NotEnoughValidMons::
	msgbox PWT_Lobby_EventScript_PWT_NotEnoughValidMons_Text_0, MSGBOX_DEFAULT
	goto PWT_Lobby_EventScript_PWT_End
	return


PWT_Lobby_EventScript_PWT_End::
	special CloseLink
	release
	end


PWT_Lobby_EventScript_WalkToDoor_Movement_0:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

PWT_Lobby_EventScript_WalkToDoor_Movement_1:
	walk_up
	set_invisible
	step_end

PWT_Lobby_EventScript_WalkToDoor_Movement_2:
	walk_up
	walk_up
	set_invisible
	step_end

PWT_Lobby_EventScript_Attendant_Text_0:
	.string "Which battle format would\nyou like to try?$"

PWT_Lobby_EventScript_Attendant_Text_1:
	.string "Singles$"

PWT_Lobby_EventScript_Attendant_Text_2:
	.string "Doubles$"

PWT_Lobby_EventScript_Attendant_Text_3:
	.string "Cancel$"

PWT_Lobby_EventScript_Attendant_Text_4:
	.string "Cancelling$"

PWT_Lobby_EventScript_Attendant_Text_5:
	.string "Pick your mons$"

PWT_Lobby_EventScript_Attendant_Text_6:
	.string "Ready to save?$"

PWT_Lobby_EventScript_PWT_SaveBeforeChallenge_Text_0:
	.string "TMP: Tournament\nInitialized$"

PWT_Lobby_EventScript_PWT_EnterChallenge_Text_0:
	.string "Time to go$"

PWT_Lobby_EventScript_PWT_CancelChallengeSaveFailed_Text_0:
	.string "No save means no play$"

PWT_Lobby_EventScript_PWT_LoadPartyCancel_Text_0:
	.string "Error during processing$"

PWT_Lobby_EventScript_PWT_NotEnoughValidMons_Text_0:
	.string "Not Enough Valid Mons$"
