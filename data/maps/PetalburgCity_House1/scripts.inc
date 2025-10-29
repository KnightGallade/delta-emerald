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
	subquestmenu QUEST_MENU_COMPLETE_QUEST, WINGS_OF_POWER, SUB_QUEST_WINGS_OF_POWER_1
	questmenu QUEST_MENU_SET_ACTIVE CORRUPTED_PECHA
	release
	end

PetalburgCity_House1_EventScript_AdvanceQuestsRelease::
	release
	end

PetalburgCity_House1_EventScript_UgradeShop::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	upgradeshop PetalburgCity_House1_Upgrades
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
PetalburgCity_House1_Upgrades:
	.2byte ITEM_AXE
	.2byte ITEM_LANTERN
	.2byte ITEM_PICKAXE
	.2byte ITEM_POWER_GLOVES
	.2byte ITEM_MOCK_WAILMER
	.2byte ITEM_RIDE_PAGER
	.2byte ITEM_SCUBA_MASK
	.2byte ITEM_MOCK_GYARADOS
	.2byte ITEM_DIGGING_KIT
	.2byte ITEM_TIDAL_TICKET
	.2byte ITEM_EGG_INCUBATOR
	.2byte ITEM_PLATINUM_CARD
	.2byte ITEM_GYM_MEMBERSHIP
	pokemartlistend

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
