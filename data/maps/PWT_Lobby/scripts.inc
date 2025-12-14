PWT_Lobby_MapScripts::
	.byte 0


PWT_Lobby_EventScript_Attendant::
	lock
	faceplayer
	setvar VAR_FRONTIER_FACILITY, FACILITY_PWT
	special SavePlayerParty
	setspeaker PWT_Lobby_EventScript_Attendant_Text_0
	msgbox PWT_Lobby_Text_Introduction, MSGBOX_DEFAULT
	goto PWT_Lobby_EventScript_AskTakeChallenge
	return


PWT_Lobby_EventScript_AskTakeChallenge::
	message PWT_Lobby_EventScript_AskTakeChallenge_Text_0
	waitmessage
	dynmultichoice 20, 6, FALSE, 3, 0, DYN_MULTICHOICE_CB_NONE, PWT_Lobby_EventScript_AskTakeChallenge_Text_1, PWT_Lobby_EventScript_AskTakeChallenge_Text_2, PWT_Lobby_EventScript_AskTakeChallenge_Text_3
	switch VAR_RESULT
	case 0, PWT_Lobby_EventScript_AskTakeChallenge_2
	case 1, PWT_Lobby_EventScript_AskTakeChallenge_3
	case 2, PWT_Lobby_EventScript_AskTakeChallenge_4
	case MULTI_B_PRESSED, PWT_Lobby_EventScript_AskTakeChallenge_4
	return

PWT_Lobby_EventScript_AskTakeChallenge_2:
	goto PWT_Lobby_EventScript_CheckElligibility
	return

PWT_Lobby_EventScript_AskTakeChallenge_3:
	msgbox PWT_Lobby_Text_ExplainChallenge, MSGBOX_DEFAULT
	goto PWT_Lobby_EventScript_AskTakeChallenge
	return

PWT_Lobby_EventScript_AskTakeChallenge_4:
	goto PWT_Lobby_EventScript_CancelChallenge
	return


PWT_Lobby_EventScript_CheckElligibility::
	specialvar VAR_RESULT, CountPartyNonEggMons
	goto_if_lt VAR_RESULT, 6, PWT_Lobby_EventScript_NotEnoughMons
	pwt_CheckIneligible
	goto_if_eq VAR_RESULT, TRUE, PWT_Lobby_EventScript_NotEligible
	goto PWT_Lobby_EventScript_TryEnterChallenge
	return


PWT_Lobby_EventScript_TryEnterChallenge::
	setspeaker PWT_Lobby_EventScript_Attendant_Text_0
	message PWT_Lobby_EventScript_TryEnterChallenge_Text_0
	waitmessage
	dynmultichoice 20, 6, FALSE, 3, 0, DYN_MULTICHOICE_CB_NONE, PWT_Lobby_EventScript_TryEnterChallenge_Text_1, PWT_Lobby_EventScript_TryEnterChallenge_Text_2, PWT_Lobby_EventScript_AskTakeChallenge_Text_3
	switch VAR_RESULT
	case 0, PWT_Lobby_EventScript_TryEnterChallenge_3
	case 1, PWT_Lobby_EventScript_TryEnterChallenge_4
	case 2, PWT_Lobby_EventScript_TryEnterChallenge_5
	case MULTI_B_PRESSED, PWT_Lobby_EventScript_TryEnterChallenge_5
PWT_Lobby_EventScript_TryEnterChallenge_1:
	setspeaker PWT_Lobby_EventScript_Attendant_Text_0
	message PWT_Lobby_EventScript_TryEnterChallenge_Text_3
	waitmessage
	dynmultichoice 20, 6, FALSE, 3, 0, DYN_MULTICHOICE_CB_NONE, PWT_Lobby_EventScript_TryEnterChallenge_Text_4, PWT_Lobby_EventScript_TryEnterChallenge_Text_5, PWT_Lobby_EventScript_TryEnterChallenge_Text_6, PWT_Lobby_EventScript_TryEnterChallenge_Text_7, PWT_Lobby_EventScript_AskTakeChallenge_Text_3
	switch VAR_RESULT
	case 0, PWT_Lobby_EventScript_TryEnterChallenge_8
	case 1, PWT_Lobby_EventScript_TryEnterChallenge_9
	case 2, PWT_Lobby_EventScript_TryEnterChallenge_10
	case 3, PWT_Lobby_EventScript_TryEnterChallenge_10
	case 4, PWT_Lobby_EventScript_TryEnterChallenge_10
	case MULTI_B_PRESSED, PWT_Lobby_EventScript_TryEnterChallenge_10
PWT_Lobby_EventScript_TryEnterChallenge_6:
	goto PWT_Lobby_EventScript_PrepareTournament
	return

PWT_Lobby_EventScript_TryEnterChallenge_3:
	pwt_SetData PWT_DATA_TOURNAMENT_TYPE, PWT_TOURNAMENT_TYPE_HOENN
	goto PWT_Lobby_EventScript_TryEnterChallenge_1

PWT_Lobby_EventScript_TryEnterChallenge_4:
	pwt_SetData PWT_DATA_TOURNAMENT_TYPE, PWT_TOURNAMENT_TYPE_ROYALE
	goto PWT_Lobby_EventScript_TryEnterChallenge_1

PWT_Lobby_EventScript_TryEnterChallenge_5:
	goto PWT_Lobby_EventScript_CancelChallenge
	goto PWT_Lobby_EventScript_TryEnterChallenge_1

PWT_Lobby_EventScript_TryEnterChallenge_8:
	pwt_SetData PWT_DATA_BATTLE_STYLE, PWT_BATTLE_STYLE_SINGLES
	goto PWT_Lobby_EventScript_TryEnterChallenge_6

PWT_Lobby_EventScript_TryEnterChallenge_9:
	pwt_SetData PWT_DATA_BATTLE_STYLE, PWT_BATTLE_STYLE_DOUBLES
	goto PWT_Lobby_EventScript_TryEnterChallenge_6

PWT_Lobby_EventScript_TryEnterChallenge_10:
	goto PWT_Lobby_EventScript_CancelChallenge
	goto PWT_Lobby_EventScript_TryEnterChallenge_6


PWT_Lobby_EventScript_PrepareTournament::
	callnative RemoveFollowingPokemon
	special SavePlayerParty
	pwt_init
	pwt_InitializeTrainers
	pwt_SetTrainers
	closemessage
	delay 2
	msgbox PWT_Lobby_Text_LetTheTournamentBegin, MSGBOX_DEFAULT
	closemessage
	call PWT_Lobby_EventScript_WalkToDoor
	special HealPlayerParty
	warp MAP_PWT_CORRIDOR, 23, 6
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


PWT_Lobby_EventScript_NotEnoughMons::
	msgbox PWT_Lobby_Text_NotEnoughMons, MSGBOX_DEFAULT
	goto PWT_Lobby_EventScript_ComeBackLater
	return


PWT_Lobby_EventScript_NotEligible::
	msgbox PWT_Lobby_EventScript_NotEligible_Text_0, MSGBOX_DEFAULT
	goto PWT_Lobby_EventScript_ComeBackLater
	return


PWT_Lobby_EventScript_ComeBackLater::
	msgbox PWT_Lobby_Text_ComeBackLater, MSGBOX_DEFAULT
	goto PWT_Lobby_EventScript_EndCancelChallenge
	return


PWT_Lobby_EventScript_LoadPartyCancelChallenge::
	special LoadPlayerParty
	callnative UpdateFollowingPokemon
	goto PWT_Lobby_EventScript_CancelChallenge
	return


PWT_Lobby_EventScript_CancelChallenge::
	msgbox PWT_Lobby_EventScript_CancelChallenge_Text_0, MSGBOX_DEFAULT
	goto PWT_Lobby_EventScript_EndCancelChallenge
	return


PWT_Lobby_EventScript_EndCancelChallenge::
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
	.string "Attendant$"

PWT_Lobby_EventScript_AskTakeChallenge_Text_0:
	.string "Would you like to enter?$"

PWT_Lobby_EventScript_AskTakeChallenge_Text_1:
	.string "Enter$"

PWT_Lobby_EventScript_AskTakeChallenge_Text_2:
	.string "Info$"

PWT_Lobby_EventScript_AskTakeChallenge_Text_3:
	.string "Exit$"

PWT_Lobby_EventScript_TryEnterChallenge_Text_0:
	.string "In which tournament would you like to\nparticipate?$"

PWT_Lobby_EventScript_TryEnterChallenge_Text_1:
	.string "Hoenn$"

PWT_Lobby_EventScript_TryEnterChallenge_Text_2:
	.string "Royale$"

PWT_Lobby_EventScript_TryEnterChallenge_Text_3:
	.string "In what battle style would you like to\nparticipate?$"

PWT_Lobby_EventScript_TryEnterChallenge_Text_4:
	.string "Singles$"

PWT_Lobby_EventScript_TryEnterChallenge_Text_5:
	.string "Doubles$"

PWT_Lobby_EventScript_TryEnterChallenge_Text_6:
	.string "Multi$"

PWT_Lobby_EventScript_TryEnterChallenge_Text_7:
	.string "Link$"

PWT_Lobby_EventScript_NotEligible_Text_0:
	.string "TMP INELIGIBLE MSG$"

PWT_Lobby_EventScript_CancelChallenge_Text_0:
	.string "We hope to see you again.$"

PWT_Lobby_Text_Introduction::
	.string "Where legends across the globe for the\n title of World Champion!\p Welcome to the PWT,\n the POKéMON World Tournament!$"

PWT_Lobby_Text_ExplainChallenge::
	.string "The PWT supports all manners of\n tournaments and battle styles.\p To begin, you are required to have six\n POKéMON, without any duplicate species\l or held items.\p Levels are irrelevant as any and all PWT\n battles will set levels to 50.\p Then, the trainer bracket will be\n decided depending on the tournament\l you entered.\p Before every battle, you will be given a\n chance to see the team of all participating\l trainers.\p This naturally includes your next\n opponent.\p You will then be asked to select a party\n of three POKéMON from your six for the\l upcoming battle.\p The winner of the battle will move on,\n and we repeat until we have a winner!$"

PWT_Lobby_Text_LetTheTournamentBegin::
	.string "Then follow me, and the let the\n tournament begin!$"

PWT_Lobby_Text_NotEnoughMons::
	.string "Excuse me!\p You must have a full party of six\n Pokémon to enter.$"

PWT_Lobby_Text_ComeBackLater::
	.string "Please come see me when you are ready.$"
