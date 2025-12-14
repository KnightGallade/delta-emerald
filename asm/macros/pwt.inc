    @ ---------- TOURAMENT DATA MANAGEMENT MACROS ----------

    @ Takes in two variables, a type of data to set and the data itself
    .macro pwt_SetData type:req, data:req
    setvar VAR_0x8004, PWT_FUNC_SET_DATA
    setvar VAR_0x8005, \type
    setvar VAR_0x8006, \data
	special CallPWTFunction
	.endm

    @ Takes in a data type, returns the value in that data type to VAR_RESULT
    .macro pwt_GetData type:req
    setvar VAR_0x8004, PWT_FUNC_GET_DATA
    setvar VAR_0x8005, \type
	special CallPWTFunction
	.endm
    
    @ ---------- TOURAMENT INITIALIZATION MACROS ----------

    @ Check the eligibility of the player's team, writes result to VAR_RESULT
    .macro pwt_CheckIneligible
    setvar VAR_0x8004, PWT_FUNC_CHECK_INELIGIBLE
    special CallPWTFunction
    .endm

    @ initialize the tournament
    .macro pwt_init
    setvar VAR_0x8004, PWT_FUNC_INIT
	special CallPWTFunction
	.endm

    @ initialize the tournament's trainers
    .macro pwt_InitializeTrainers
    setvar VAR_0x8004, PWT_FUNC_INIT_TRAINERS
	special CallPWTFunction
	.endm

    @ set the tournament's trainers
    .macro pwt_SetTrainers
    setvar VAR_0x8004, PWT_FUNC_SET_TRAINERS
	special CallPWTFunction
	.endm

    @ ---------- PRE-BATTLE ROOM MACROS ----------

    @ show the opponent's info card
    .macro pwt_ShowOpponentCard
    setvar VAR_0x8004, PWT_FUNC_SHOW_OPPONENT_CARD
	special CallPWTFunction
	.endm

    @ Displays the tourney tree
	.macro pwt_ShowTourneyTree
	setvar VAR_0x8004, PWT_FUNC_SHOW_TOURNEY_TREE
	special CallPWTFunction
	.endm

    @ Save the selected party for the battle
    .macro pwt_SetPlayerParty
    setvar VAR_0x8004, PWT_FUNC_SET_PLAYER_PARTY
	special CallPWTFunction
	.endm

    @ Sets the next opponent's id
    .macro pwt_SetOpponentId
    setvar VAR_0x8004, PWT_FUNC_SET_OPPONENT_ID
	special CallPWTFunction
	.endm

    @ ---------- BATTLE ROOM MACROS ----------

    @ Sets the next opponent's gfx
    .macro pwt_SetOpponentGFX
    setvar VAR_0x8004, PWT_FUNC_SET_OPPONENT_GFX
	special CallPWTFunction
	.endm

    @ Announce the opponent
    .macro pwt_AnnounceTheOpponent
    setvar VAR_0x8004, PWT_FUNC_ANNOUNCE_OPPONENT
	special CallPWTFunction
	.endm

    @ Get the opponent's name
    .macro pwt_BufferBattleStrings
    setvar VAR_0x8004, PWT_FUNC_BUFFER_BATTLE_STRINGS
	special CallPWTFunction
	.endm

    @ Get the opponent's introduction string
    .macro pwt_ShowOpponentIntro
    setvar VAR_0x8004, PWT_FUNC_SHOW_OPPONENT_INTRO
	special CallPWTFunction
	.endm

    @ Set the opponent's party
    .macro pwt_SetOpponentParty
    setvar VAR_0x8004, PWT_FUNC_SET_OPPONENT_PARTY
	special CallPWTFunction
	.endm
