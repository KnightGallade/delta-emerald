PetalburgCity_House1_MapScripts::
	.byte 0

PetalburgCity_House1_EventScript_Man::
	msgbox PetalburgCity_House1_Text_TravelingIsWonderful, MSGBOX_NPC
	end

PetalburgCity_House1_EventScript_Woman::
	msgbox PetalburgCity_House1_Text_GoOnAdventure, MSGBOX_NPC
	end

PetalburgCity_House1_EventScript_AdvanceQuests::
	lockall
	faceplayer
	msgbox PetalburgCity_House1_Text_TravelingIsWonderful, MSGBOX_DEFAULT
	checkitem ITEM_JOURNAL
	goto_if_eq VAR_RESULT, TRUE, PetalburgCity_House1_EventScript_AdvanceQuestsRelease
	giveitem ITEM_JOURNAL
	questmenu QUEST_MENU_SET_ACTIVE WINGS_OF_POWER
	questmenu QUEST_MENU_SET_ACTIVE MUTATED_WINGS
	questmenu QUEST_MENU_SET_ACTIVE BEASTS_OF_RESURRECTION
	questmenu QUEST_MENU_SET_ACTIVE GOLEMS_OF_THE_ERAS
	questmenu QUEST_MENU_SET_ACTIVE SPIRITS_OF_THE_SOUL
	questmenu QUEST_MENU_SET_ACTIVE PRIMORDIAL_SEA
	questmenu QUEST_MENU_SET_ACTIVE DESOLATE_LAND
	questmenu QUEST_MENU_SET_ACTIVE ANCIENT_DRAGON_OF_CONQUEST
	questmenu QUEST_MENU_SET_ACTIVE LEGENDS_LIE
	questmenu QUEST_MENU_SET_ACTIVE IMPRISONED_CURSES
	questmenu QUEST_MENU_SET_ACTIVE SENTINELS_OF_THE_WILDS
	questmenu QUEST_MENU_SET_ACTIVE GENIES_OF_NATURE
	questmenu QUEST_MENU_SET_ACTIVE CHRONO_ALCHEMY
	questmenu QUEST_MENU_SET_ACTIVE TECHNO_REBIRTH
	questmenu QUEST_MENU_SET_ACTIVE CONCEALED_FORCES_OF_EXISTENCE
	questmenu QUEST_MENU_SET_ACTIVE BROKEN_WORMHOLES
	questmenu QUEST_MENU_SET_ACTIVE CELESTIAL_ECLIPSE
	questmenu QUEST_MENU_SET_ACTIVE WARDENS_OF_THE_NEBULA
	questmenu QUEST_MENU_SET_ACTIVE TWIN_SOULS
	questmenu QUEST_MENU_SET_ACTIVE ARTIFICIAL_POWER
	questmenu QUEST_MENU_SET_ACTIVE TAO_KINGS
	questmenu QUEST_MENU_SET_ACTIVE DISTORTED_DIMENSIONS
	questmenu QUEST_MENU_SET_ACTIVE GOLDEN_HEART_SILVER_SOUL
	questmenu QUEST_MENU_SET_ACTIVE LONE_FIRE
	questmenu QUEST_MENU_SET_ACTIVE LUNAR_SPIRITS
	questmenu QUEST_MENU_SET_ACTIVE HEARTS_OF_THE_SEA
	questmenu QUEST_MENU_SET_ACTIVE STALWART_SOLDIERS
	questmenu QUEST_MENU_SET_ACTIVE MARTIAL_STUDENT
	questmenu QUEST_MENU_SET_ACTIVE KINGS_STEED
	questmenu QUEST_MENU_SET_ACTIVE STELLAR_SHRINE
	questmenu QUEST_MENU_SET_ACTIVE MYTHICAL_PIXIES
	questmenu QUEST_MENU_SET_ACTIVE FRAGMENTED_DNA
	questmenu QUEST_MENU_SET_ACTIVE CHALLENGE_THE_PROGENITOR
	questmenu QUEST_MENU_SET_ACTIVE FALLEN_IDOL
	questmenu QUEST_MENU_SET_ACTIVE PRINCESS_JEWEL
	questmenu QUEST_MENU_SET_ACTIVE BURNING_WATER
	questmenu QUEST_MENU_SET_ACTIVE ARTIFICIAL_SOUL
	questmenu QUEST_MENU_SET_ACTIVE HIDDEN_SHADOW
	questmenu QUEST_MENU_SET_ACTIVE ZERAORA_QUEST
	questmenu QUEST_MENU_SET_ACTIVE MYSTERY_METAL
	questmenu QUEST_MENU_SET_ACTIVE ZARUDE_QUEST
	questmenu QUEST_MENU_SET_ACTIVE CORRUPTED_PECHA
	release
	end

PetalburgCity_House1_EventScript_AdvanceQuestsRelease::
	release
	end

PetalburgCity_House1_Text_TravelingIsWonderful:
	.string "Traveling is wonderful!\p"
	.string "When I was young, I roamed the seas\n"
	.string "and the mountains!$"

PetalburgCity_House1_Text_GoOnAdventure:
	.string "Sigh…\p"
	.string "I wish I could go on an adventure\n"
	.string "with some POKéMON…\p"
	.string "Crawl through some damp grass…\n"
	.string "Climb rocky, rugged mountains…\p"
	.string "Cross the raging seas…\n"
	.string "Wander about in dark caves…\p"
	.string "And, sometimes, even get a little\n"
	.string "homesick…\p"
	.string "It must be fabulous to travel!$"
