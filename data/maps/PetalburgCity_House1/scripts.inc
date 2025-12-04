
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
	createfieldmugshot MUGSHOT_TEST, EMOTE_NORMAL
	msgbox PetalburgCity_House1_Text_TravelingIsWonderful, MSGBOX_DEFAULT
	removefieldmugshot
	checkitem ITEM_JOURNAL
	goto_if_eq VAR_RESULT, TRUE, PetalburgCity_House1_EventScript_AdvanceQuestsRelease
	giveitem ITEM_JOURNAL
	giveitem ITEM_OUTFIT_BOX
	questmenu QUEST_MENU_SET_ACTIVE WINGS_OF_POWER
	questmenu QUEST_MENU_SET_ACTIVE CORRUPTED_PECHA
	releaseall
	end

PetalburgCity_House1_EventScript_AdvanceQuestsRelease::
	release
	end

PetalburgCity_House1_EventScript_UpgradeShop::
	lockall
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	upgradeshop PetalburgCity_House1_Upgrades
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	releaseall
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
	upgradeshoplistend

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

PetalburgCity_House1_PersonalityTest::
	lockall
	faceplayer
	msgbox PetalburgCity_House1_PersonalityTest_Text_0
	waitmessage
	msgbox PetalburgCity_House1_PersonalityTest_Text_1
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_2, PetalburgCity_House1_PersonalityTest_Text_3, PetalburgCity_House1_PersonalityTest_Text_4
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_3
	case 1, PetalburgCity_House1_PersonalityTest_4
	case 2, PetalburgCity_House1_PersonalityTest_5
PetalburgCity_House1_PersonalityTest_1:
	msgbox PetalburgCity_House1_PersonalityTest_Text_5
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_6, PetalburgCity_House1_PersonalityTest_Text_7
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_8
	case 1, PetalburgCity_House1_PersonalityTest_9
PetalburgCity_House1_PersonalityTest_6:
	msgbox PetalburgCity_House1_PersonalityTest_Text_8
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_6, PetalburgCity_House1_PersonalityTest_Text_7
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_12
	case 1, PetalburgCity_House1_PersonalityTest_13
PetalburgCity_House1_PersonalityTest_10:
	msgbox PetalburgCity_House1_PersonalityTest_Text_9
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_10, PetalburgCity_House1_PersonalityTest_Text_11, PetalburgCity_House1_PersonalityTest_Text_12
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_16
	case 1, PetalburgCity_House1_PersonalityTest_17
	case 2, PetalburgCity_House1_PersonalityTest_18
PetalburgCity_House1_PersonalityTest_14:
	msgbox PetalburgCity_House1_PersonalityTest_Text_13
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_14, PetalburgCity_House1_PersonalityTest_Text_15
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_21
	case 1, PetalburgCity_House1_PersonalityTest_22
PetalburgCity_House1_PersonalityTest_19:
	msgbox PetalburgCity_House1_PersonalityTest_Text_16
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_17, PetalburgCity_House1_PersonalityTest_Text_18
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_25
	case 1, PetalburgCity_House1_PersonalityTest_26
PetalburgCity_House1_PersonalityTest_23:
	msgbox PetalburgCity_House1_PersonalityTest_Text_19
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_20, PetalburgCity_House1_PersonalityTest_Text_21, PetalburgCity_House1_PersonalityTest_Text_22
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_29
	case 1, PetalburgCity_House1_PersonalityTest_30
	case 2, PetalburgCity_House1_PersonalityTest_31
PetalburgCity_House1_PersonalityTest_27:
	msgbox PetalburgCity_House1_PersonalityTest_Text_23
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_24, PetalburgCity_House1_PersonalityTest_Text_25, PetalburgCity_House1_PersonalityTest_Text_26
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_34
	case 1, PetalburgCity_House1_PersonalityTest_35
	case 2, PetalburgCity_House1_PersonalityTest_36
PetalburgCity_House1_PersonalityTest_32:
	msgbox PetalburgCity_House1_PersonalityTest_Text_27
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_6, PetalburgCity_House1_PersonalityTest_Text_7
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_39
	case 1, PetalburgCity_House1_PersonalityTest_40
PetalburgCity_House1_PersonalityTest_37:
	msgbox PetalburgCity_House1_PersonalityTest_Text_28
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_29, PetalburgCity_House1_PersonalityTest_Text_30, PetalburgCity_House1_PersonalityTest_Text_31
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_43
	case 1, PetalburgCity_House1_PersonalityTest_44
	case 2, PetalburgCity_House1_PersonalityTest_45
PetalburgCity_House1_PersonalityTest_41:
	msgbox PetalburgCity_House1_PersonalityTest_Text_35
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_36, PetalburgCity_House1_PersonalityTest_Text_37
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_48
	case 1, PetalburgCity_House1_PersonalityTest_49
PetalburgCity_House1_PersonalityTest_46:
	msgbox PetalburgCity_House1_PersonalityTest_Text_38
	waitmessage
	dynmultichoice 14, 5, TRUE, 4, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_39, PetalburgCity_House1_PersonalityTest_Text_40, PetalburgCity_House1_PersonalityTest_Text_41, PetalburgCity_House1_PersonalityTest_Text_42
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_55
	case 1, PetalburgCity_House1_PersonalityTest_56
	case 2, PetalburgCity_House1_PersonalityTest_57
	case 3, PetalburgCity_House1_PersonalityTest_58
PetalburgCity_House1_PersonalityTest_53:
	msgbox PetalburgCity_House1_PersonalityTest_Text_43
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_6, PetalburgCity_House1_PersonalityTest_Text_7
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_61
	case 1, PetalburgCity_House1_PersonalityTest_62
PetalburgCity_House1_PersonalityTest_59:
	msgbox PetalburgCity_House1_PersonalityTest_Text_44
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_6, PetalburgCity_House1_PersonalityTest_Text_7
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_65
	case 1, PetalburgCity_House1_PersonalityTest_66
PetalburgCity_House1_PersonalityTest_63:
	msgbox PetalburgCity_House1_PersonalityTest_Text_45
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_46, PetalburgCity_House1_PersonalityTest_Text_47, PetalburgCity_House1_PersonalityTest_Text_48
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_69
	case 1, PetalburgCity_House1_PersonalityTest_70
	case 2, PetalburgCity_House1_PersonalityTest_71
PetalburgCity_House1_PersonalityTest_67:
	msgbox PetalburgCity_House1_PersonalityTest_Text_49
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_50, PetalburgCity_House1_PersonalityTest_Text_51, PetalburgCity_House1_PersonalityTest_Text_52
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_74
	case 1, PetalburgCity_House1_PersonalityTest_75
	case 2, PetalburgCity_House1_PersonalityTest_76
PetalburgCity_House1_PersonalityTest_72:
	msgbox PetalburgCity_House1_PersonalityTest_Text_53
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_6, PetalburgCity_House1_PersonalityTest_Text_7
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_79
	case 1, PetalburgCity_House1_PersonalityTest_80
PetalburgCity_House1_PersonalityTest_77:
	msgbox PetalburgCity_House1_PersonalityTest_Text_54
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_6, PetalburgCity_House1_PersonalityTest_Text_7
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_83
	case 1, PetalburgCity_House1_PersonalityTest_84
PetalburgCity_House1_PersonalityTest_81:
	msgbox PetalburgCity_House1_PersonalityTest_Text_55
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_6, PetalburgCity_House1_PersonalityTest_Text_7
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_87
	case 1, PetalburgCity_House1_PersonalityTest_88
PetalburgCity_House1_PersonalityTest_85:
	msgbox PetalburgCity_House1_PersonalityTest_Text_56
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_57, PetalburgCity_House1_PersonalityTest_Text_58, PetalburgCity_House1_PersonalityTest_Text_59
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_91
	case 1, PetalburgCity_House1_PersonalityTest_92
	case 2, PetalburgCity_House1_PersonalityTest_93
PetalburgCity_House1_PersonalityTest_89:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_96
	case 1, PetalburgCity_House1_PersonalityTest_97
	case 2, PetalburgCity_House1_PersonalityTest_98
PetalburgCity_House1_PersonalityTest_94:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_101
	case 1, PetalburgCity_House1_PersonalityTest_102
	case 2, PetalburgCity_House1_PersonalityTest_103
PetalburgCity_House1_PersonalityTest_99:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_106
	case 1, PetalburgCity_House1_PersonalityTest_107
	case 2, PetalburgCity_House1_PersonalityTest_108
PetalburgCity_House1_PersonalityTest_104:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_111
	case 1, PetalburgCity_House1_PersonalityTest_112
	case 2, PetalburgCity_House1_PersonalityTest_113
PetalburgCity_House1_PersonalityTest_109:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_116
	case 1, PetalburgCity_House1_PersonalityTest_117
	case 2, PetalburgCity_House1_PersonalityTest_118
PetalburgCity_House1_PersonalityTest_114:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_121
	case 1, PetalburgCity_House1_PersonalityTest_122
	case 2, PetalburgCity_House1_PersonalityTest_123
PetalburgCity_House1_PersonalityTest_119:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_126
	case 1, PetalburgCity_House1_PersonalityTest_127
	case 2, PetalburgCity_House1_PersonalityTest_128
PetalburgCity_House1_PersonalityTest_124:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_131
	case 1, PetalburgCity_House1_PersonalityTest_132
	case 2, PetalburgCity_House1_PersonalityTest_133
PetalburgCity_House1_PersonalityTest_129:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_136
	case 1, PetalburgCity_House1_PersonalityTest_137
	case 2, PetalburgCity_House1_PersonalityTest_138
PetalburgCity_House1_PersonalityTest_134:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_141
	case 1, PetalburgCity_House1_PersonalityTest_142
	case 2, PetalburgCity_House1_PersonalityTest_143
PetalburgCity_House1_PersonalityTest_139:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_146
	case 1, PetalburgCity_House1_PersonalityTest_147
	case 2, PetalburgCity_House1_PersonalityTest_148
PetalburgCity_House1_PersonalityTest_144:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_151
	case 1, PetalburgCity_House1_PersonalityTest_152
	case 2, PetalburgCity_House1_PersonalityTest_153
PetalburgCity_House1_PersonalityTest_149:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_156
	case 1, PetalburgCity_House1_PersonalityTest_157
	case 2, PetalburgCity_House1_PersonalityTest_158
PetalburgCity_House1_PersonalityTest_154:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_161
	case 1, PetalburgCity_House1_PersonalityTest_162
	case 2, PetalburgCity_House1_PersonalityTest_163
PetalburgCity_House1_PersonalityTest_159:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_166
	case 1, PetalburgCity_House1_PersonalityTest_167
	case 2, PetalburgCity_House1_PersonalityTest_168
PetalburgCity_House1_PersonalityTest_164:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_171
	case 1, PetalburgCity_House1_PersonalityTest_172
	case 2, PetalburgCity_House1_PersonalityTest_173
PetalburgCity_House1_PersonalityTest_169:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_176
	case 1, PetalburgCity_House1_PersonalityTest_177
	case 2, PetalburgCity_House1_PersonalityTest_178
PetalburgCity_House1_PersonalityTest_174:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_181
	case 1, PetalburgCity_House1_PersonalityTest_182
	case 2, PetalburgCity_House1_PersonalityTest_183
PetalburgCity_House1_PersonalityTest_179:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_186
	case 1, PetalburgCity_House1_PersonalityTest_187
	case 2, PetalburgCity_House1_PersonalityTest_188
PetalburgCity_House1_PersonalityTest_184:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_191
	case 1, PetalburgCity_House1_PersonalityTest_192
	case 2, PetalburgCity_House1_PersonalityTest_193
PetalburgCity_House1_PersonalityTest_189:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_196
	case 1, PetalburgCity_House1_PersonalityTest_197
	case 2, PetalburgCity_House1_PersonalityTest_198
PetalburgCity_House1_PersonalityTest_194:
	msgbox PetalburgCity_House1_PersonalityTest_Text_60
	waitmessage
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_61, PetalburgCity_House1_PersonalityTest_Text_62, PetalburgCity_House1_PersonalityTest_Text_63
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_201
	case 1, PetalburgCity_House1_PersonalityTest_202
	case 2, PetalburgCity_House1_PersonalityTest_203
PetalburgCity_House1_PersonalityTest_199:
	releaseall
	end

PetalburgCity_House1_PersonalityTest_3:
	addvar VAR_NATURE_HARDY, 2
	goto PetalburgCity_House1_PersonalityTest_1

PetalburgCity_House1_PersonalityTest_4:
	addvar VAR_NATURE_RELAXED, 2
	goto PetalburgCity_House1_PersonalityTest_1

PetalburgCity_House1_PersonalityTest_5:
	addvar VAR_NATURE_IMPISH, 2
	goto PetalburgCity_House1_PersonalityTest_1

PetalburgCity_House1_PersonalityTest_8:
	addvar VAR_NATURE_HARDY, 2
	addvar VAR_NATURE_DOCILE, 1
	goto PetalburgCity_House1_PersonalityTest_6

PetalburgCity_House1_PersonalityTest_9:
	addvar VAR_NATURE_QUIRKY, 2
	goto PetalburgCity_House1_PersonalityTest_6

PetalburgCity_House1_PersonalityTest_12:
	addvar VAR_NATURE_HARDY, 2
	addvar VAR_NATURE_BRAVE, 2
	goto PetalburgCity_House1_PersonalityTest_10

PetalburgCity_House1_PersonalityTest_13:
	addvar VAR_NATURE_SASSY, 2
	addvar VAR_NATURE_QUIRKY, 2
	goto PetalburgCity_House1_PersonalityTest_10

PetalburgCity_House1_PersonalityTest_16:
	addvar VAR_NATURE_HARDY, 2
	goto PetalburgCity_House1_PersonalityTest_14

PetalburgCity_House1_PersonalityTest_17:
	addvar VAR_NATURE_CALM, 2
	goto PetalburgCity_House1_PersonalityTest_14

PetalburgCity_House1_PersonalityTest_18:
	addvar VAR_NATURE_QUIRKY, 2
	goto PetalburgCity_House1_PersonalityTest_14

PetalburgCity_House1_PersonalityTest_21:
	addvar VAR_NATURE_DOCILE, 2
	addvar VAR_NATURE_NAIVE, 1
	goto PetalburgCity_House1_PersonalityTest_19

PetalburgCity_House1_PersonalityTest_22:
	addvar VAR_NATURE_TIMID, 2
	addvar VAR_NATURE_CALM, 1
	goto PetalburgCity_House1_PersonalityTest_19

PetalburgCity_House1_PersonalityTest_25:
	addvar VAR_NATURE_DOCILE, 2
	addvar VAR_NATURE_HASTY, 1
	goto PetalburgCity_House1_PersonalityTest_23

PetalburgCity_House1_PersonalityTest_26:
	addvar VAR_NATURE_NAIVE, 2
	addvar VAR_NATURE_RELAXED, 1
	goto PetalburgCity_House1_PersonalityTest_23

PetalburgCity_House1_PersonalityTest_29:
	addvar VAR_NATURE_DOCILE, 2
	goto PetalburgCity_House1_PersonalityTest_27

PetalburgCity_House1_PersonalityTest_30:
	addvar VAR_NATURE_NAIVE, 2
	addvar VAR_NATURE_LONELY, 1
	goto PetalburgCity_House1_PersonalityTest_27

PetalburgCity_House1_PersonalityTest_31:
	addvar VAR_NATURE_SASSY, 2
	goto PetalburgCity_House1_PersonalityTest_27

PetalburgCity_House1_PersonalityTest_34:
	addvar VAR_NATURE_DOCILE, 2
	goto PetalburgCity_House1_PersonalityTest_32

PetalburgCity_House1_PersonalityTest_35:
	addvar VAR_NATURE_NAIVE, 2
	goto PetalburgCity_House1_PersonalityTest_32

PetalburgCity_House1_PersonalityTest_36:
	addvar VAR_NATURE_IMPISH, 2
	goto PetalburgCity_House1_PersonalityTest_32

PetalburgCity_House1_PersonalityTest_39:
	addvar VAR_NATURE_BRAVE, 3
	addvar VAR_NATURE_IMPISH, 1
	goto PetalburgCity_House1_PersonalityTest_37

PetalburgCity_House1_PersonalityTest_40:
	addvar VAR_NATURE_DOCILE, 2
	addvar VAR_NATURE_TIMID, 1
	goto PetalburgCity_House1_PersonalityTest_37

PetalburgCity_House1_PersonalityTest_43:
	msgbox PetalburgCity_House1_PersonalityTest_Text_32
	dynmultichoice 14, 6, TRUE, 3, 0, DYN_MULTICHOICE_CB_NONE, PetalburgCity_House1_PersonalityTest_Text_33, PetalburgCity_House1_PersonalityTest_Text_34
	switch VAR_RESULT
	case 0, PetalburgCity_House1_PersonalityTest_51
	case 1, PetalburgCity_House1_PersonalityTest_52
	goto PetalburgCity_House1_PersonalityTest_41

PetalburgCity_House1_PersonalityTest_44:
	addvar VAR_NATURE_TIMID, 2
	goto PetalburgCity_House1_PersonalityTest_41

PetalburgCity_House1_PersonalityTest_45:
	addvar VAR_NATURE_RELAXED, 2
	goto PetalburgCity_House1_PersonalityTest_41

PetalburgCity_House1_PersonalityTest_48:
	addvar VAR_NATURE_HARDY, 1
	addvar VAR_NATURE_BRAVE, 2
	goto PetalburgCity_House1_PersonalityTest_46

PetalburgCity_House1_PersonalityTest_49:
	addvar VAR_NATURE_NAIVE, 2
	goto PetalburgCity_House1_PersonalityTest_46

PetalburgCity_House1_PersonalityTest_51:
	addvar VAR_NATURE_SASSY, 1
	addvar VAR_NATURE_RELAXED, 1
	goto PetalburgCity_House1_PersonalityTest_41

PetalburgCity_House1_PersonalityTest_52:
	addvar VAR_NATURE_BRAVE, 4
	goto PetalburgCity_House1_PersonalityTest_41

PetalburgCity_House1_PersonalityTest_55:
	addvar VAR_NATURE_BRAVE, 3
	goto PetalburgCity_House1_PersonalityTest_53

PetalburgCity_House1_PersonalityTest_56:
	addvar VAR_NATURE_HARDY, 2
	addvar VAR_NATURE_BRAVE, 2
	goto PetalburgCity_House1_PersonalityTest_53

PetalburgCity_House1_PersonalityTest_57:
	addvar VAR_NATURE_DOCILE, 1
	addvar VAR_NATURE_TIMID, 1
	addvar VAR_NATURE_RELAXED, 1
	goto PetalburgCity_House1_PersonalityTest_53

PetalburgCity_House1_PersonalityTest_58:
	addvar VAR_NATURE_TIMID, 2
	goto PetalburgCity_House1_PersonalityTest_53

PetalburgCity_House1_PersonalityTest_61:
	addvar VAR_NATURE_JOLLY, 2
	addvar VAR_NATURE_NAIVE, 1
	goto PetalburgCity_House1_PersonalityTest_59

PetalburgCity_House1_PersonalityTest_62:
	addvar VAR_NATURE_SASSY, 1
	addvar VAR_NATURE_QUIRKY, 1
	goto PetalburgCity_House1_PersonalityTest_59

PetalburgCity_House1_PersonalityTest_65:
	addvar VAR_NATURE_JOLLY, 2
	addvar VAR_NATURE_LONELY, 1
	goto PetalburgCity_House1_PersonalityTest_63

PetalburgCity_House1_PersonalityTest_66:
	addvar VAR_NATURE_TIMID, 1
	goto PetalburgCity_House1_PersonalityTest_63

PetalburgCity_House1_PersonalityTest_69:
	addvar VAR_NATURE_JOLLY, 2
	goto PetalburgCity_House1_PersonalityTest_67

PetalburgCity_House1_PersonalityTest_70:
	addvar VAR_NATURE_CALM, 2
	goto PetalburgCity_House1_PersonalityTest_67

PetalburgCity_House1_PersonalityTest_71:
	addvar VAR_NATURE_QUIRKY, 2
	goto PetalburgCity_House1_PersonalityTest_67

PetalburgCity_House1_PersonalityTest_74:
	addvar VAR_NATURE_JOLLY, 3
	goto PetalburgCity_House1_PersonalityTest_72

PetalburgCity_House1_PersonalityTest_75:
	addvar VAR_NATURE_HARDY, 2
	goto PetalburgCity_House1_PersonalityTest_72

PetalburgCity_House1_PersonalityTest_76:
	addvar VAR_NATURE_TIMID, 2
	goto PetalburgCity_House1_PersonalityTest_72

PetalburgCity_House1_PersonalityTest_79:
	addvar VAR_NATURE_IMPISH, 2
	addvar VAR_NATURE_LONELY, 1
	goto PetalburgCity_House1_PersonalityTest_77

PetalburgCity_House1_PersonalityTest_80:
	addvar VAR_NATURE_CALM, 2
	goto PetalburgCity_House1_PersonalityTest_77

PetalburgCity_House1_PersonalityTest_83:
	addvar VAR_NATURE_IMPISH, 2
	goto PetalburgCity_House1_PersonalityTest_81

PetalburgCity_House1_PersonalityTest_84:
	addvar VAR_NATURE_DOCILE, 1
	addvar VAR_NATURE_RELAXED, 1
	goto PetalburgCity_House1_PersonalityTest_81

PetalburgCity_House1_PersonalityTest_87:
	addvar VAR_NATURE_HARDY, 1
	addvar VAR_NATURE_IMPISH, 2
	goto PetalburgCity_House1_PersonalityTest_85

PetalburgCity_House1_PersonalityTest_88:
	addvar VAR_NATURE_SASSY, 1
	addvar VAR_NATURE_QUIRKY, 2
	goto PetalburgCity_House1_PersonalityTest_85

PetalburgCity_House1_PersonalityTest_91:
	addvar VAR_NATURE_BRAVE, 3
	goto PetalburgCity_House1_PersonalityTest_89

PetalburgCity_House1_PersonalityTest_92:
	addvar VAR_NATURE_TIMID, 2
	goto PetalburgCity_House1_PersonalityTest_89

PetalburgCity_House1_PersonalityTest_93:
	addvar VAR_NATURE_IMPISH, 2
	goto PetalburgCity_House1_PersonalityTest_89

PetalburgCity_House1_PersonalityTest_96:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_94

PetalburgCity_House1_PersonalityTest_97:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_94

PetalburgCity_House1_PersonalityTest_98:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_94

PetalburgCity_House1_PersonalityTest_101:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_99

PetalburgCity_House1_PersonalityTest_102:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_99

PetalburgCity_House1_PersonalityTest_103:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_99

PetalburgCity_House1_PersonalityTest_106:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_104

PetalburgCity_House1_PersonalityTest_107:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_104

PetalburgCity_House1_PersonalityTest_108:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_104

PetalburgCity_House1_PersonalityTest_111:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_109

PetalburgCity_House1_PersonalityTest_112:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_109

PetalburgCity_House1_PersonalityTest_113:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_109

PetalburgCity_House1_PersonalityTest_116:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_114

PetalburgCity_House1_PersonalityTest_117:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_114

PetalburgCity_House1_PersonalityTest_118:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_114

PetalburgCity_House1_PersonalityTest_121:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_119

PetalburgCity_House1_PersonalityTest_122:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_119

PetalburgCity_House1_PersonalityTest_123:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_119

PetalburgCity_House1_PersonalityTest_126:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_124

PetalburgCity_House1_PersonalityTest_127:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_124

PetalburgCity_House1_PersonalityTest_128:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_124

PetalburgCity_House1_PersonalityTest_131:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_129

PetalburgCity_House1_PersonalityTest_132:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_129

PetalburgCity_House1_PersonalityTest_133:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_129

PetalburgCity_House1_PersonalityTest_136:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_134

PetalburgCity_House1_PersonalityTest_137:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_134

PetalburgCity_House1_PersonalityTest_138:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_134

PetalburgCity_House1_PersonalityTest_141:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_139

PetalburgCity_House1_PersonalityTest_142:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_139

PetalburgCity_House1_PersonalityTest_143:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_139

PetalburgCity_House1_PersonalityTest_146:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_144

PetalburgCity_House1_PersonalityTest_147:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_144

PetalburgCity_House1_PersonalityTest_148:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_144

PetalburgCity_House1_PersonalityTest_151:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_149

PetalburgCity_House1_PersonalityTest_152:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_149

PetalburgCity_House1_PersonalityTest_153:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_149

PetalburgCity_House1_PersonalityTest_156:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_154

PetalburgCity_House1_PersonalityTest_157:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_154

PetalburgCity_House1_PersonalityTest_158:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_154

PetalburgCity_House1_PersonalityTest_161:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_159

PetalburgCity_House1_PersonalityTest_162:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_159

PetalburgCity_House1_PersonalityTest_163:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_159

PetalburgCity_House1_PersonalityTest_166:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_164

PetalburgCity_House1_PersonalityTest_167:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_164

PetalburgCity_House1_PersonalityTest_168:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_164

PetalburgCity_House1_PersonalityTest_171:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_169

PetalburgCity_House1_PersonalityTest_172:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_169

PetalburgCity_House1_PersonalityTest_173:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_169

PetalburgCity_House1_PersonalityTest_176:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_174

PetalburgCity_House1_PersonalityTest_177:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_174

PetalburgCity_House1_PersonalityTest_178:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_174

PetalburgCity_House1_PersonalityTest_181:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_179

PetalburgCity_House1_PersonalityTest_182:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_179

PetalburgCity_House1_PersonalityTest_183:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_179

PetalburgCity_House1_PersonalityTest_186:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_184

PetalburgCity_House1_PersonalityTest_187:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_184

PetalburgCity_House1_PersonalityTest_188:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_184

PetalburgCity_House1_PersonalityTest_191:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_189

PetalburgCity_House1_PersonalityTest_192:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_189

PetalburgCity_House1_PersonalityTest_193:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_189

PetalburgCity_House1_PersonalityTest_196:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_194

PetalburgCity_House1_PersonalityTest_197:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_194

PetalburgCity_House1_PersonalityTest_198:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_194

PetalburgCity_House1_PersonalityTest_201:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_199

PetalburgCity_House1_PersonalityTest_202:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_199

PetalburgCity_House1_PersonalityTest_203:
	addvar VAR_TEMP_0, 1
	goto PetalburgCity_House1_PersonalityTest_199


PetalburgCity_House1_PersonalityTest_Text_0:
	.string "Time for a quiz!$"

PetalburgCity_House1_PersonalityTest_Text_1:
	.string "A test is coming up.\nHow do you study?$"

PetalburgCity_House1_PersonalityTest_Text_2:
	.string "Study hard$"

PetalburgCity_House1_PersonalityTest_Text_3:
	.string "At the last second$"

PetalburgCity_House1_PersonalityTest_Text_4:
	.string "Ignore it and play$"

PetalburgCity_House1_PersonalityTest_Text_5:
	.string "Can you focus on something you like?$"

PetalburgCity_House1_PersonalityTest_Text_6:
	.string "Yes$"

PetalburgCity_House1_PersonalityTest_Text_7:
	.string "No$"

PetalburgCity_House1_PersonalityTest_Text_8:
	.string "When the going gets tough,\ndo you get going?$"

PetalburgCity_House1_PersonalityTest_Text_9:
	.string "There is a bucket. If you put water in it, how high will you fill it?$"

PetalburgCity_House1_PersonalityTest_Text_10:
	.string "Full$"

PetalburgCity_House1_PersonalityTest_Text_11:
	.string "Half$"

PetalburgCity_House1_PersonalityTest_Text_12:
	.string "A little$"

PetalburgCity_House1_PersonalityTest_Text_13:
	.string "You are offered a choice of two gifts.\nWhich one will you take?$"

PetalburgCity_House1_PersonalityTest_Text_14:
	.string "Big box$"

PetalburgCity_House1_PersonalityTest_Text_15:
	.string "Small box$"

PetalburgCity_House1_PersonalityTest_Text_16:
	.string "YOu broke a rotten egg in your room.\nWhat will you do?$"

PetalburgCity_House1_PersonalityTest_Text_17:
	.string "Open a window right away$"

PetalburgCity_House1_PersonalityTest_Text_18:
	.string "Take a sniff first$"

PetalburgCity_House1_PersonalityTest_Text_19:
	.string "A friend brought over something you'd forgotten. How do you thank your friend?$"

PetalburgCity_House1_PersonalityTest_Text_20:
	.string "Say thank you regularly$"

PetalburgCity_House1_PersonalityTest_Text_21:
	.string "Say thanks with a joke$"

PetalburgCity_House1_PersonalityTest_Text_22:
	.string "Say thanks, but be cool$"

PetalburgCity_House1_PersonalityTest_Text_23:
	.string "There is a wallet at the side of a road.$"

PetalburgCity_House1_PersonalityTest_Text_24:
	.string "Turn it in to the police!$"

PetalburgCity_House1_PersonalityTest_Text_25:
	.string "Yay! Yay!$"

PetalburgCity_House1_PersonalityTest_Text_26:
	.string "Is anyone watching…?$"

PetalburgCity_House1_PersonalityTest_Text_27:
	.string "You're going bungee jumping for the first time. Since it's scary, you decide to test the jump with a doll... And the bungee cord snaps! Will you still try to make a jump anyway?$"

PetalburgCity_House1_PersonalityTest_Text_28:
	.string "There is an alien invasion! What will you do?$"

PetalburgCity_House1_PersonalityTest_Text_29:
	.string "Fight$"

PetalburgCity_House1_PersonalityTest_Text_30:
	.string "Run$"

PetalburgCity_House1_PersonalityTest_Text_31:
	.string "Ignore it$"

PetalburgCity_House1_PersonalityTest_Text_32:
	.string "You valiantly fight the aliens... But, you are defeated... An alien says to you... 'YOU HAVE IMPRESSED US. IT WAS A PLEASURE TO SEE. JOIN US, AND TOGETHER WE SHALL RULE THE WORLD.' What will you do?$"

PetalburgCity_House1_PersonalityTest_Text_33:
	.string "Rule with the aliens$"

PetalburgCity_House1_PersonalityTest_Text_34:
	.string "Refuse$"

PetalburgCity_House1_PersonalityTest_Text_35:
	.string "There is a scream from behind a door! How will you react?$"

PetalburgCity_House1_PersonalityTest_Text_36:
	.string "Yank open the door$"

PetalburgCity_House1_PersonalityTest_Text_37:
	.string "Scream in unison$"

PetalburgCity_House1_PersonalityTest_Text_38:
	.string "A delinquent is hassling a girl on a busy city street! What will you do?$"

PetalburgCity_House1_PersonalityTest_Text_39:
	.string "Help without hesitation$"

PetalburgCity_House1_PersonalityTest_Text_40:
	.string "Help, even if scared$"

PetalburgCity_House1_PersonalityTest_Text_41:
	.string "Call the police$"

PetalburgCity_House1_PersonalityTest_Text_42:
	.string "Do nothing out of fear$"

PetalburgCity_House1_PersonalityTest_Text_43:
	.string "Are you a cheerful personality?$"

PetalburgCity_House1_PersonalityTest_Text_44:
	.string "Do you like to noisily enjoy yourself with others?$"

PetalburgCity_House1_PersonalityTest_Text_45:
	.string "It's the summer holidays! Where would you like to go?$"

PetalburgCity_House1_PersonalityTest_Text_46:
	.string "The beach!$"

PetalburgCity_House1_PersonalityTest_Text_47:
	.string "Spas$"

PetalburgCity_House1_PersonalityTest_Text_48:
	.string "Anywhere$"

PetalburgCity_House1_PersonalityTest_Text_49:
	.string "A foreign person has started up a conversation with you. To be honest, you don't have a clue what this fellow is saying. How do you reply?$"

PetalburgCity_House1_PersonalityTest_Text_50:
	.string "Haha! Yes. Very funny!$"

PetalburgCity_House1_PersonalityTest_Text_51:
	.string "Um... Could you say that again?$"

PetalburgCity_House1_PersonalityTest_Text_52:
	.string "Right... Well, I gotta go.$"

PetalburgCity_House1_PersonalityTest_Text_53:
	.string "Have you ever made a pitfall trap?$"

PetalburgCity_House1_PersonalityTest_Text_54:
	.string "Do you like pranks?$"

PetalburgCity_House1_PersonalityTest_Text_55:
	.string "Are there many things that you would like to do?$"

PetalburgCity_House1_PersonalityTest_Text_56:
	.string "Your friend is being bullied! What do you do?$"

PetalburgCity_House1_PersonalityTest_Text_57:
	.string "Face up to the bully$"

PetalburgCity_House1_PersonalityTest_Text_58:
	.string "Caution the bully from afar$"

PetalburgCity_House1_PersonalityTest_Text_59:
	.string "Heckle the bully from behind$"

PetalburgCity_House1_PersonalityTest_Text_60:
	.string "Q\nQ$"

PetalburgCity_House1_PersonalityTest_Text_61:
	.string "1$"

PetalburgCity_House1_PersonalityTest_Text_62:
	.string "2$"

PetalburgCity_House1_PersonalityTest_Text_63:
	.string "3$"
