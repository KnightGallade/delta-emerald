DistortionWorld_End_MapScripts::
	.byte 0


DistortionWorld_End_EventScript_Giratina::
	lockall
	playmoncry SPECIES_GIRATINA_ORIGIN, CRY_MODE_ENCOUNTER
	setwildbattle SPECIES_GIRATINA_ORIGIN, 70
	delay 40
	waitmoncry
	special BattleSetup_StartLegendaryBattle
	releaseall
	end

