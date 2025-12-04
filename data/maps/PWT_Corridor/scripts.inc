PWT_Corridor_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, PWT_Corridor_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	.byte 0

PWT_Corridor_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_TEMP_0, 0, PWT_Corridor_EventScript_EnterCorridor
	.2byte 0


PWT_Corridor_EventScript_EnterCorridor::
	delay 16
	setvar VAR_TEMP_0, 1
	applymovement LOCALID_PWT_CORRIDOR_ATTENDANT, PWT_Corridor_EventScript_EnterCorridor_Movement_0
	applymovement LOCALID_PLAYER, PWT_Corridor_EventScript_EnterCorridor_Movement_1
	waitmovement 0
	opendoor 37, 3
	waitdooranim
	applymovement LOCALID_PWT_CORRIDOR_ATTENDANT, PWT_Corridor_EventScript_EnterCorridor_Movement_2
	applymovement LOCALID_PLAYER, PWT_Corridor_EventScript_EnterCorridor_Movement_3
	waitmovement 0
	closedoor 37, 3
	waitdooranim
	waitmovement 0
	setvar VAR_0x8006, 0
	warp MAP_PWT_PRE_BATTLE_ROOM, 5, 7
	waitstate
	end


PWT_Corridor_EventScript_EnterCorridor_Movement_0:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_up
	step_end

PWT_Corridor_EventScript_EnterCorridor_Movement_1:
	walk_up
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_in_place_faster_up
	step_end

PWT_Corridor_EventScript_EnterCorridor_Movement_2:
	walk_up
	set_invisible
	step_end

PWT_Corridor_EventScript_EnterCorridor_Movement_3:
	walk_up
	walk_up
	set_invisible
	step_end
