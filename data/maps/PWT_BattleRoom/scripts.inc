PWT_BattleRoom_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, PWT_BattleRoom_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, PWT_BattleRoom_OnResume
	map_script MAP_SCRIPT_ON_FRAME_TABLE, PWT_BattleRoom_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, PWT_BattleRoom_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	.byte 0

PWT_BattleRoom_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_TEMP_0, 0, PWT_BattleRoom_EventScript_EnterRoom
	.2byte 0

PWT_BattleRoom_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	map_script_2 VAR_TEMP_1, 0, PWT_BattleRoom_EventScript_SetUpObjects
	.2byte 0


PWT_BattleRoom_OnTransition::
	setflag FLAG_TEMP_HIDE_FOLLOWER
	pwt_SetOpponentGFX
	pwt_GetData PWT_DATA_ROUND_NUM
	copyvar VAR_TEMP_F, VAR_RESULT
	call_if_eq VAR_RESULT, PWT_ROUND1, PWT_BattleRoom_EventScript_SetWalkingAudienceMemberPos
	setoutfitgfxintovar VAR_OBJ_GFX_ID_1
	end


PWT_BattleRoom_EventScript_EnterRoom::
	lockall
	pwt_GetData PWT_DATA_ROUND_NUM
	call_if_eq VAR_RESULT, PWT_ROUND1, PWT_BattleRoom_EventScript_TryDoAudienceMemberWalkToSeat
	applymovement LOCALID_PWT_ANNOUNCER, Common_Movement_WalkInPlaceDown
	waitmovement 0
	call PWT_BattleRoom_EventScript_AnnouncePlayer
	setspeaker PWT_BattleRoom_EventScript_EnterRoom_Text_0
	message PWT_BattleRoom_EventScript_EnterRoom_Text_1
	waitbuttonpress
	closemessage
	showobjectat LOCALID_PWT_PLAYER, MAP_PWT_BATTLE_ROOM
	applymovement LOCALID_PWT_PLAYER, PWT_BattleRoom_EventScript_EnterRoom_Movement_0
	playse SE_M_ENCORE2
	call PWT_BattleRoom_EventScript_AudienceLookAround
	waitmovement 0
	waitse
	setspeaker PWT_BattleRoom_EventScript_EnterRoom_Text_0
	pwt_AnnounceTheOpponent
	waitbuttonpress
	closemessage
	showobjectat LOCALID_PWT_OPPONENT, MAP_PWT_BATTLE_ROOM
	applymovement LOCALID_PWT_OPPONENT, PWT_BattleRoom_EventScript_EnterRoom_Movement_1
	playse SE_M_ENCORE2
	call PWT_BattleRoom_EventScript_AudienceLookAround
	waitmovement 0
	waitse
	pwt_BufferBattleStrings
	setspeaker PWT_BattleRoom_EventScript_EnterRoom_Text_0
	msgbox PWT_BattleRoom_EventScript_EnterRoom_Text_2, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PWT_PLAYER, PWT_BattleRoom_EventScript_EnterRoom_Movement_2
	applymovement LOCALID_PWT_OPPONENT, PWT_BattleRoom_EventScript_EnterRoom_Movement_3
	waitmovement 0
	setspeaker PWT_BattleRoom_EventScript_EnterRoom_Text_3
	pwt_ShowOpponentIntro
	waitmessage
	closemessage
	pwt_SetOpponentParty
	call PWT_BattleRoom_EventScript_DoPWTBattle
	releaseall
	end


PWT_BattleRoom_EventScript_AnnouncePlayer::
	setspeaker PWT_BattleRoom_EventScript_EnterRoom_Text_0
	pwt_GetData PWT_DATA_ROUND_NUM
	switch VAR_RESULT
	case PWT_ROUND1, PWT_BattleRoom_EventScript_AnnouncePlayer_3
	case PWT_ROUND2, PWT_BattleRoom_EventScript_AnnouncePlayer_4
	case PWT_SEMIFINAL, PWT_BattleRoom_EventScript_AnnouncePlayer_5
	case PWT_FINAL, PWT_BattleRoom_EventScript_AnnouncePlayer_6
PWT_BattleRoom_EventScript_AnnouncePlayer_1:
	waitmessage
	return

PWT_BattleRoom_EventScript_AnnouncePlayer_3:
	message PWT_BattleRoom_EventScript_AnnouncePlayer_Text_0
	goto PWT_BattleRoom_EventScript_AnnouncePlayer_1

PWT_BattleRoom_EventScript_AnnouncePlayer_4:
	message PWT_BattleRoom_EventScript_AnnouncePlayer_Text_1
	goto PWT_BattleRoom_EventScript_AnnouncePlayer_1

PWT_BattleRoom_EventScript_AnnouncePlayer_5:
	message PWT_BattleRoom_EventScript_AnnouncePlayer_Text_2
	goto PWT_BattleRoom_EventScript_AnnouncePlayer_1

PWT_BattleRoom_EventScript_AnnouncePlayer_6:
	message PWT_BattleRoom_EventScript_AnnouncePlayer_Text_3
	goto PWT_BattleRoom_EventScript_AnnouncePlayer_1


PWT_BattleRoom_EventScript_DoPWTBattle::
	special HealPlayerParty
	setvar VAR_TEMP_9, 1
	special DoPWTTrainerBattle
	waitstate
	setvar VAR_TEMP_9, 0
	special LoadPlayerParty
	special HealPlayerParty
	return


PWT_BattleRoom_EventScript_SetUpObjects::
	hideobjectat LOCALID_PWT_PLAYER, MAP_PWT_BATTLE_ROOM
	hideobjectat LOCALID_PWT_OPPONENT, MAP_PWT_BATTLE_ROOM
	call PWT_BattleRoom_EventScript_AddAudience
	setoutfitgfxintovar VAR_OBJ_GFX_ID_1
	setvar VAR_TEMP_1, 1
	applymovement LOCALID_PLAYER, PWT_BattleRoom_EventScript_SetUpObjects_Movement_0
	applymovement LOCALID_PWT_OPPONENT, PWT_BattleRoom_EventScript_SetUpObjects_Movement_1
	pwt_GetData PWT_DATA_ROUND_NUM
	return


PWT_BattleRoom_OnResume::
	call_if_eq VAR_TEMP_9, 1, PWT_BattleRoom_EventScript_CallAddAudience
	end


PWT_BattleRoom_EventScript_CallAddAudience::
	call PWT_BattleRoom_EventScript_AddAudience
	return


PWT_BattleRoom_EventScript_AddAudience::
	call_if_eq VAR_TEMP_F, PWT_ROUND1, PWT_BattleRoom_EventScript_AddRound1Audience
	call_if_eq VAR_TEMP_F, PWT_ROUND2, PWT_BattleRoom_EventScript_AddRound2Audience
	call_if_eq VAR_TEMP_F, PWT_SEMIFINAL, PWT_BattleRoom_EventScript_AddSemifinalAudience
	call_if_eq VAR_TEMP_F, PWT_FINAL, PWT_BattleRoom_EventScript_AddFinalAudience
	return


PWT_BattleRoom_EventScript_AddRound1Audience::
	return


PWT_BattleRoom_EventScript_AddRound2Audience::
	createvobject OBJ_EVENT_GFX_SCIENTIST_1, 1, 3, 0
	createvobject OBJ_EVENT_GFX_EXPERT_F, 4, 6, 0
	createvobject OBJ_EVENT_GFX_NINJA_BOY, 6, 8, 0
	createvobject OBJ_EVENT_GFX_LITTLE_GIRL, 9, 11, 0
	createvobject OBJ_EVENT_GFX_SCIENTIST_1, 11, 13, 0
	createvobject OBJ_EVENT_GFX_MAN_5, 13, 15, 0
	createvobject OBJ_EVENT_GFX_BEAUTY, 19, 7, 1
	createvobject OBJ_EVENT_GFX_WOMAN_5, 22, 11, 1
	createvobject OBJ_EVENT_GFX_LITTLE_BOY, 25, 15, 1
	createvobject OBJ_EVENT_GFX_YOUNGSTER, 26, 2, 2
	createvobject OBJ_EVENT_GFX_SCHOOL_KID_M, 29, 5, 1
	return


PWT_BattleRoom_EventScript_AddSemifinalAudience::
	createvobject OBJ_EVENT_GFX_SCIENTIST_1, 1, 3, 0
	createvobject OBJ_EVENT_GFX_EXPERT_F, 4, 6, 0
	createvobject OBJ_EVENT_GFX_NINJA_BOY, 6, 8, 0
	createvobject OBJ_EVENT_GFX_WOMAN_2, 7, 9, 0
	createvobject OBJ_EVENT_GFX_LITTLE_GIRL, 9, 11, 0
	createvobject OBJ_EVENT_GFX_LASS, 10, 12, 0
	createvobject OBJ_EVENT_GFX_SCIENTIST_1, 11, 13, 0
	createvobject OBJ_EVENT_GFX_MAN_5, 13, 15, 0
	createvobject OBJ_EVENT_GFX_GENTLEMAN, 15, 2, 1
	createvobject OBJ_EVENT_GFX_NINJA_BOY, 16, 3, 1
	createvobject OBJ_EVENT_GFX_WOMAN_2, 17, 4, 1
	createvobject OBJ_EVENT_GFX_BEAUTY, 19, 7, 1
	createvobject OBJ_EVENT_GFX_EXPERT_F, 20, 9, 1
	createvobject OBJ_EVENT_GFX_WOMAN_5, 22, 11, 1
	createvobject OBJ_EVENT_GFX_SCIENTIST_1, 23, 13, 1
	createvobject OBJ_EVENT_GFX_LITTLE_BOY, 25, 15, 1
	createvobject OBJ_EVENT_GFX_YOUNGSTER, 26, 2, 2
	createvobject OBJ_EVENT_GFX_HEX_MANIAC, 28, 5, 2
	createvobject OBJ_EVENT_GFX_SCHOOL_KID_M, 29, 5, 1
	createvobject OBJ_EVENT_GFX_MART_EMPLOYEE, 30, 6, 2
	createvobject OBJ_EVENT_GFX_WOMAN_5, 31, 8, 2
	return


PWT_BattleRoom_EventScript_AddFinalAudience::
	createvobject OBJ_EVENT_GFX_NINJA_BOY, 0, 2, 0
	createvobject OBJ_EVENT_GFX_SCIENTIST_1, 1, 3, 0
	createvobject OBJ_EVENT_GFX_BEAUTY, 2, 15, 0
	createvobject OBJ_EVENT_GFX_MAN_5, 3, 5, 0
	createvobject OBJ_EVENT_GFX_EXPERT_F, 4, 6, 0
	createvobject OBJ_EVENT_GFX_SCIENTIST_1, 5, 7, 0
	createvobject OBJ_EVENT_GFX_NINJA_BOY, 6, 8, 0
	createvobject OBJ_EVENT_GFX_WOMAN_2, 7, 9, 0
	createvobject OBJ_EVENT_GFX_WOMAN_3, 8, 10, 0
	createvobject OBJ_EVENT_GFX_LITTLE_GIRL, 9, 11, 0
	createvobject OBJ_EVENT_GFX_LASS, 10, 12, 0
	createvobject OBJ_EVENT_GFX_SCIENTIST_1, 11, 13, 0
	createvobject OBJ_EVENT_GFX_BEAUTY, 12, 14, 0
	createvobject OBJ_EVENT_GFX_MAN_5, 13, 15, 2
	createvobject OBJ_EVENT_GFX_HIKER, 14, 12, 2
	createvobject OBJ_EVENT_GFX_GENTLEMAN, 15, 2, 1
	createvobject OBJ_EVENT_GFX_NINJA_BOY, 16, 3, 1
	createvobject OBJ_EVENT_GFX_WOMAN_2, 17, 4, 1
	createvobject OBJ_EVENT_GFX_WOMAN_3, 18, 6, 1
	createvobject OBJ_EVENT_GFX_BEAUTY, 19, 7, 1
	createvobject OBJ_EVENT_GFX_EXPERT_F, 20, 9, 1
	createvobject OBJ_EVENT_GFX_MAN_2, 21, 10, 1
	createvobject OBJ_EVENT_GFX_WOMAN_5, 22, 11, 1
	createvobject OBJ_EVENT_GFX_SCIENTIST_1, 23, 13, 1
	createvobject OBJ_EVENT_GFX_GENTLEMAN, 24, 14, 1
	createvobject OBJ_EVENT_GFX_LITTLE_BOY, 25, 15, 1
	createvobject OBJ_EVENT_GFX_YOUNGSTER, 26, 2, 2
	createvobject OBJ_EVENT_GFX_FAT_MAN, 27, 3, 2
	createvobject OBJ_EVENT_GFX_HEX_MANIAC, 28, 5, 2
	createvobject OBJ_EVENT_GFX_SCHOOL_KID_M, 29, 5, 1
	createvobject OBJ_EVENT_GFX_MART_EMPLOYEE, 30, 6, 2
	createvobject OBJ_EVENT_GFX_WOMAN_5, 31, 8, 2
	return


PWT_BattleRoom_EventScript_AudienceLookAround::
	turnvobject 0, DIR_EAST
	turnvobject 2, DIR_EAST
	turnvobject 4, DIR_EAST
	turnvobject 6, DIR_EAST
	turnvobject 8, DIR_EAST
	turnvobject 10, DIR_WEST
	turnvobject 12, DIR_WEST
	turnvobject 14, DIR_WEST
	turnvobject 16, DIR_WEST
	turnvobject 18, DIR_WEST
	turnvobject 20, DIR_EAST
	turnvobject 22, DIR_EAST
	turnvobject 24, DIR_EAST
	turnvobject 26, DIR_EAST
	turnvobject 28, DIR_EAST
	turnvobject 30, DIR_EAST
	delay 20
	turnvobject 0, DIR_SOUTH
	turnvobject 2, DIR_SOUTH
	turnvobject 4, DIR_SOUTH
	turnvobject 6, DIR_SOUTH
	turnvobject 8, DIR_SOUTH
	turnvobject 10, DIR_SOUTH
	turnvobject 12, DIR_SOUTH
	turnvobject 14, DIR_SOUTH
	turnvobject 16, DIR_SOUTH
	turnvobject 18, DIR_SOUTH
	turnvobject 20, DIR_SOUTH
	turnvobject 22, DIR_SOUTH
	turnvobject 24, DIR_SOUTH
	turnvobject 26, DIR_SOUTH
	turnvobject 28, DIR_SOUTH
	turnvobject 30, DIR_SOUTH
	delay 20
	turnvobject 1, DIR_EAST
	turnvobject 3, DIR_EAST
	turnvobject 5, DIR_EAST
	turnvobject 7, DIR_EAST
	turnvobject 9, DIR_EAST
	turnvobject 11, DIR_WEST
	turnvobject 13, DIR_WEST
	turnvobject 15, DIR_WEST
	turnvobject 17, DIR_WEST
	turnvobject 19, DIR_WEST
	turnvobject 21, DIR_EAST
	turnvobject 23, DIR_EAST
	turnvobject 25, DIR_EAST
	turnvobject 27, DIR_WEST
	turnvobject 31, DIR_WEST
	delay 20
	turnvobject 1, DIR_SOUTH
	turnvobject 3, DIR_SOUTH
	turnvobject 5, DIR_SOUTH
	turnvobject 7, DIR_SOUTH
	turnvobject 9, DIR_SOUTH
	turnvobject 11, DIR_SOUTH
	turnvobject 13, DIR_SOUTH
	turnvobject 15, DIR_SOUTH
	turnvobject 17, DIR_SOUTH
	turnvobject 19, DIR_SOUTH
	turnvobject 21, DIR_SOUTH
	turnvobject 23, DIR_SOUTH
	turnvobject 25, DIR_SOUTH
	turnvobject 27, DIR_SOUTH
	turnvobject 31, DIR_SOUTH
	delay 20
	return


PWT_BattleRoom_EventScript_SetWalkingAudienceMemberPos::
	random 2
	copyvar VAR_TEMP_D, VAR_RESULT
	goto_if_eq VAR_TEMP_D, 0, Common_EventScript_NopReturn
	setobjectxyperm LOCALID_PWT_AUDIENCE_WALKING, 2, 0
	setobjectmovementtype LOCALID_PWT_AUDIENCE_WALKING, MOVEMENT_TYPE_FACE_RIGHT
	return


PWT_BattleRoom_EventScript_TryDoAudienceMemberWalkToSeat::
	goto_if_eq VAR_TEMP_D, 0, Common_EventScript_NopReturn
	applymovement LOCALID_PWT_AUDIENCE_WALKING, PWT_BattleRoom_EventScript_TryDoAudienceMemberWalkToSeat_Movement_0
	return


PWT_BattleRoom_EventScript_EnterRoom_Movement_0:
	set_visible
	delay_16
	walk_up
	walk_up
	walk_up
	walk_right
	walk_right
	step_end

PWT_BattleRoom_EventScript_EnterRoom_Movement_1:
	set_visible
	delay_16
	walk_up
	walk_up
	walk_up
	walk_left
	walk_left
	step_end

PWT_BattleRoom_EventScript_EnterRoom_Movement_2:
	walk_right
	step_end

PWT_BattleRoom_EventScript_EnterRoom_Movement_3:
	walk_left
	step_end

PWT_BattleRoom_EventScript_SetUpObjects_Movement_0:
	set_invisible
	step_end

PWT_BattleRoom_EventScript_SetUpObjects_Movement_1:
	set_invisible
	face_up
	step_end

PWT_BattleRoom_EventScript_TryDoAudienceMemberWalkToSeat_Movement_0:
	walk_down
	walk_down
	walk_right
	walk_right
	walk_in_place_faster_down
	step_end

PWT_BattleRoom_EventScript_EnterRoom_Text_0:
	.string "Announcer$"

PWT_BattleRoom_EventScript_EnterRoom_Text_1:
	.string "The Littleroot Hero,\n{PLAYER} has entered!$"

PWT_BattleRoom_EventScript_EnterRoom_Text_2:
	.string "{STR_VAR_1}\n{PLAYER} versus {STR_VAR_2}!\lLet the battle begin!$"

PWT_BattleRoom_EventScript_EnterRoom_Text_3:
	.string "{STR_VAR_2}$"

PWT_BattleRoom_EventScript_AnnouncePlayer_Text_0:
	.string "It is my honor to announce that the\nfirst round of the PWT has commenced!\p$"

PWT_BattleRoom_EventScript_AnnouncePlayer_Text_1:
	.string "ROUND 2 MESSAGE\nPLAYER INTRODUCTION\p$"

PWT_BattleRoom_EventScript_AnnouncePlayer_Text_2:
	.string "SEMIFINAL MESSAGE\nPLAYER INTRODUCTION\p$"

PWT_BattleRoom_EventScript_AnnouncePlayer_Text_3:
	.string "FINALS MESSAGE\nPLAYER INTRODUCTION\p$"
