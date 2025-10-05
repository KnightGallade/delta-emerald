#include "global.h"
#include "strings.h"
#include "bg.h"
#include "data.h"
#include "decompress.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "item.h"
#include "item_icon.h"
#include "item_menu.h"
#include "item_menu_icons.h"
#include "list_menu.h"
#include "item_use.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "palette.h"
#include "party_menu.h"
#include "scanline_effect.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text_window.h"
#include "quests.h"
#include "overworld.h"
#include "event_data.h"
#include "constants/items.h"
#include "constants/field_weather.h"
#include "constants/songs.h"
#include "constants/rgb.h"
#include "constants/event_objects.h"
#include "event_object_movement.h"
#include "pokemon_icon.h"

#include "random.h"

#define tPageItems      data[4]
#define tItemPcParam    data[6]

struct QuestMenuResources
{
	MainCallback savedCallback;
	u8 moveModeOrigPos;
	u8 spriteIconSlot;
	u16 oldPaletteTag;
	u8 maxShowed;
	u8 nItems;
	u8 scrollIndicatorArrowPairId;
	s16 data[3];
	u8 filterMode;
	u8 parentQuest;
	bool8 restoreCursor;
};

struct QuestMenuStaticResources
{
	MainCallback savedCallback;
	u16 scroll;
	u16 row;
	u8 initialized;
	u16 storedScrollOffset;
	u16 storedRowPosition;
};

// RAM
EWRAM_DATA static struct QuestMenuResources *sStateDataPtr = NULL;
EWRAM_DATA static u8 *sBg1TilemapBuffer = NULL;
EWRAM_DATA static struct ListMenuItem *sListMenuItems = NULL;
EWRAM_DATA static struct QuestMenuStaticResources sListMenuState = {0};
EWRAM_DATA static u8 sItemMenuIconSpriteIds[12] = {0};        // from pokefirered src/item_menu_icons.c
EWRAM_DATA static void *questNamePointer = NULL;
EWRAM_DATA static u8 **questNameArray = NULL;

// This File's Functions
void QuestMenu_Init(u8 a0, MainCallback callback);
static void MainCB(void);
static void VBlankCB(void);
static void RunSetup(void);

static bool8 SetupGraphics(void);
static bool8 LoadGraphics(void);
static void QuestMenu_InitWindows(void);
static bool8 InitBackgrounds(void);
static void InitItems(void);
static bool8 AllocateResourcesForListMenu(void);
static void AllocateMemoryForArray();
static void PlaceTopMenuScrollIndicatorArrows(void);
static void SetInitializedFlag(u8 a0);

static u8 GetCursorPosition(void);
static void SetCursorPosition(void);
static void SetScrollPosition(void);
static bool8 IfScrollIsOutOfBounds(void);
static bool8 IfRowIsOutOfBounds(void);
static void SaveScrollAndRow(s16 *data);

static void ClearModeOnStartup(void);
static u8 ManageMode(u8 action);
static u8 ToggleAlphaMode(u8 mode);
static u8 ToggleSubquestMode(u8 mode);
static u8 IncrementMode(u8 mode);
static bool8 IsSubquestMode(void);
static bool8 IsNotFilteredMode(void);
static bool8 IsAlphaMode(void);

static void BuildMenuTemplate(void);
static u8 GetModeAndGenerateList();
static u8 CountNumberListRows();
static u8 *DefineQuestOrder();
static u8 GenerateSubquestList();
static u8 GenerateList(bool8 isFiltered);
static void AssignCancelNameAndId(u8 numRow);

static u8 CountUnlockedQuests(void);
static u8 CountInactiveQuests(void);
static u8 CountActiveQuests(void);
static u8 CountRewardQuests(void);
static u8 CountCompletedQuests(void);
static u8 CountFavoriteQuests(void);

static void PopulateEmptyRow(u8 countQuest);
static void PrependQuestNumber(u8 countQuest);
static void SetFavoriteQuest(u8 countQuest);
static void PopulateQuestName(u8 countQuest);
static void PopulateSubquestName(u8 parentQuest, u8 countQuest);
static void PopulateListRowNameAndId(u8 row, u8 countQuest);
static bool8 DoesQuestHaveChildrenAndNotInactive(u16 itemId);
static void AddSubQuestButton(u8 countQuest);

static void QuestMenu_AddTextPrinterParameterized(u8 windowId, u8 fontId,
            const u8 *str, u8 x, u8 y, u8 letterSpacing, u8 lineSpacing, u8 speed,
            u8 colorIdx);

static void MoveCursorFunc(s32 itemIndex, bool8 onInit,
                           struct ListMenu *list);
static void PlayCursorSound(bool8 firstRun);
static void PrintDetailsForCancel();
static void GenerateAndPrintQuestDetails(s32 questId);
static void GenerateQuestLocation(s32 questId);
static void PrintQuestLocation(s32 questId);
static void GenerateQuestFlavorText(s32 questId);
static void UpdateQuestFlavorText(s32 questId);
static void PrintQuestFlavorText(s32 questId);

static bool8 IsQuestUnlocked(s32 questId);
static bool8 IsQuestActiveState(s32 questId);
static bool8 IsQuestInactiveState(s32 questId);
static bool8 IsQuestRewardState(s32 questId);
static bool8 IsQuestCompletedState(s32 questId);
static bool8 IsSubquestCompletedState(s32 questId);

static void DetermineSpriteType(s32 questId);
static void QuestMenu_CreateSprite(u16 itemId, u8 idx, u8 spriteType);
static void ResetSpriteState(void);
static void QuestMenu_DestroySprite(u8 idx);

static void GenerateStateAndPrint(u8 windowId, u32 itemId, u8 y);
static u8 GenerateSubquestState(u8 questId);
static u8 GenerateQuestState(u8 questId);
static void PrintQuestState(u8 windowId, u8 y, u8 colorIndex);

static void GenerateAndPrintHeader(void);
static void GenerateDenominatorNumQuests(void);
static void GenerateNumeratorNumQuests(void);
static void GenerateMenuContext(void);
static void PrintNumQuests(void);
static void PrintMenuContext(void);
static void PrintTypeFilterButton(void);

static void Task_Main(u8 taskId);
static void ManageFavorites(u8 index);
static void Task_QuestMenuCleanUp(u8 taskId);
static void RestoreSavedScrollAndRow(s16 *data);
static void ResetCursorToTop(s16 *data);
static void QuestMenu_RemoveScrollIndicatorArrowPair(void);
static void EnterSubquestModeAndCleanUp(u8 taskId, s16 *data, s32 input);
static void ChangeModeAndCleanUp(u8 taskId);
static void ToggleAlphaModeAndCleanUp(u8 taskId);
static void ToggleFavoriteAndCleanUp(u8 taskId, u8 selectedQuestId);
static bool8 CheckSelectedIsCancel(u8 selectedQuestId);
static void ReturnFromSubquestAndCleanUp(u8 taskId);

static void SetGpuRegBaseForFade(void);
static void InitFadeVariables(u8 taskId, u8 blendWeight, u8 frameDelay,
                              u8 frameTimerBase, u8 delta);
static void PrepareFadeOut(u8 taskId);
static bool8 HandleFadeOut(u8 taskId);
static void PrepareFadeIn(u8 taskId);
static bool8 HandleFadeIn(u8 taskId);
static void Task_FadeOut(u8 taskId);
static void Task_FadeIn(u8 taskId);

static void Task_QuestMenuWaitFadeAndBail(u8 taskId);
static void FadeAndBail(void);
static void FreeResources(void);
static void TurnOffQuestMenu(u8 taskId);
static void Task_QuestMenuTurnOff1(u8 taskId);
static void Task_QuestMenuTurnOff2(u8 taskId);

// Tiles, palettes and tilemaps for the Quest Menu
static const u32 sQuestMenuTiles[] =
        INCBIN_U32("graphics/quest_menu/menu.4bpp.smol");
static const u32 sQuestMenuBgPals[] =
        INCBIN_U32("graphics/quest_menu/menu.gbapal");
static const u32 sQuestMenuTilemap[] =
        INCBIN_U32("graphics/quest_menu/menu.bin.smolTM");

//Strings used for the Quest Menu
static const u8 sText_Empty[] = _("");
static const u8 sText_AllHeader[] = _("All Missions");
static const u8 sText_InactiveHeader[] = _("Inactive Missions");
static const u8 sText_ActiveHeader[] = _("Active Missions");
static const u8 sText_RewardHeader[] = _("Reward Available");
static const u8 sText_CompletedHeader[] =
      _("Completed Missions");
static const u8 sText_QuestNumberDisplay[] =
      _("{STR_VAR_1}/{STR_VAR_2}");
static const u8 sText_Unk[] = _("??????");
static const u8 sText_Active[] = _("Active");
static const u8 sText_Reward[] = _("Reward");
static const u8 sText_Complete[] = _("Done");
static const u8 sText_Ready[] = _("Ready");
static const u8 sText_ShowLocation[] =
      _("Location: {STR_VAR_2}");
static const u8 sText_StartForMore[] =
      _("Start for more details.");
static const u8 sText_ReturnRecieveReward[] =
      _("Return to {STR_VAR_2}\nto recieve your reward!");
static const u8 sText_SubQuestButton[] = _(" {A_BUTTON}");
static const u8 sText_Type[] = _("{R_BUTTON}Type");
static const u8 sText_Caught[] = _("Caught");
static const u8 sText_Found[] = _("Found");
static const u8 sText_Read[] = _("Read");
static const u8 sText_Back[] = _("Back");
static const u8 sText_DotSpace[] = _(". ");
static const u8 sText_Close[] = _("Close");
static const u8 sText_ColorGreen[] = _("{COLOR}{GREEN}");
static const u8 sText_AZ[] = _(" A-Z");

///////////////////////////////////////////////////////////////////////////////
//////////////////////BEGIN SUBQUEST CUSTOMIZATION/////////////////////////////

//Declaration of subquest structures. Edits to subquests are made here.
#define sub_quest(i, n, d, m, s, st, t) {.id = i, .name = n, .desc = d, .map = m, .sprite = s, .spritetype = st, .type = t}

static const struct SubQuest sSubQuests_Articuno[QUEST_ARTICUNO_SUB_COUNT] =
{
	sub_quest(
	    0,
	    gText_SubQuest_Articuno_Name1,
	    gText_SubQuest_Articuno_Desc1,
	    gText_SubQuest_Articuno_Map1,
	    ITEM_FAME_CHECKER,
	    ITEM,
	    sText_Ready
	),

	sub_quest(
	    1,
	    gText_SubQuest_Articuno_Name2,
	    gText_SubQuest_Articuno_Desc2,
	    gText_SubQuest_Articuno_Map2,
	    ITEM_POKE_RADAR,
	    ITEM,
	    sText_Ready
	),

	sub_quest(
	    2,
	    gText_SubQuest_Articuno_Name3,
	    gText_SubQuest_Articuno_Desc3,
	    gText_SubQuest_Articuno_Map3,
	    SPECIES_ARTICUNO,
	    PKMN,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Zapdos[QUEST_ZAPDOS_SUB_COUNT] =
{
	sub_quest(
	    3,
	    gText_SubQuest_Zapdos_Name1,
	    gText_SubQuest_Zapdos_Desc1,
	    gText_SubQuest_Zapdos_Map1,
	    ITEM_FAME_CHECKER,
	    ITEM,
	    sText_Ready
	),

	sub_quest(
	    4,
	    gText_SubQuest_Zapdos_Name2,
	    gText_SubQuest_Zapdos_Desc2,
	    gText_SubQuest_Zapdos_Map2,
	    ITEM_POKE_RADAR,
	    ITEM,
	    sText_Ready
	),

	sub_quest(
	    5,
	    gText_SubQuest_Zapdos_Name3,
	    gText_SubQuest_Zapdos_Desc3,
	    gText_SubQuest_Zapdos_Map3,
	    SPECIES_ZAPDOS,
	    PKMN,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Moltres[QUEST_MOLTRES_SUB_COUNT] =
{
	sub_quest(
	    6,
	    gText_SubQuest_Moltres_Name1,
	    gText_SubQuest_Moltres_Desc1,
	    gText_SubQuest_Moltres_Map1,
	    ITEM_FAME_CHECKER,
	    ITEM,
	    sText_Ready
	),

	sub_quest(
	    7,
	    gText_SubQuest_Moltres_Name2,
	    gText_SubQuest_Moltres_Desc2,
	    gText_SubQuest_Moltres_Map2,
	    ITEM_POKE_RADAR,
	    ITEM,
	    sText_Ready
	),

	sub_quest(
	    8,
	    gText_SubQuest_Moltres_Name3,
	    gText_SubQuest_Moltres_Desc3,
	    gText_SubQuest_Moltres_Map3,
	    SPECIES_MOLTRES,
	    PKMN,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Mewtwo[QUEST_MEWTWO_SUB_COUNT] =
{
	sub_quest(
	    9,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    10,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    11,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Mew[QUEST_MEW_SUB_COUNT] =
{
	sub_quest(
	    12,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    13,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    14,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Raikou[QUEST_RAIKOU_SUB_COUNT] =
{
	sub_quest(
	    15,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    16,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    17,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Entei[QUEST_ENTEI_SUB_COUNT] =
{
	sub_quest(
	    18,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    19,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    20,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Suicune[QUEST_SUICUNE_SUB_COUNT] =
{
	sub_quest(
	    21,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    22,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    23,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Lugia[QUEST_LUGIA_SUB_COUNT] =
{
	sub_quest(
	    24,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    25,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    26,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Ho_Oh[QUEST_HO_OH_SUB_COUNT] =
{
	sub_quest(
	    27,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    28,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    29,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Celebi[QUEST_CELEBI_SUB_COUNT] =
{
	sub_quest(
	    30,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    31,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    32,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Regirock[QUEST_REGIROCK_SUB_COUNT] =
{
	sub_quest(
	    33,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    34,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    35,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Regice[QUEST_REGICE_SUB_COUNT] =
{
	sub_quest(
	    36,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    37,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    38,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Registeel[QUEST_REGISTEEL_SUB_COUNT] =
{
	sub_quest(
	    39,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    40,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    41,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Latias[QUEST_LATIAS_SUB_COUNT] =
{
	sub_quest(
	    42,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    43,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    44,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Latios[QUEST_LATIOS_SUB_COUNT] =
{
	sub_quest(
	    45,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    46,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    47,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Kyogre[QUEST_KYOGRE_SUB_COUNT] =
{
	sub_quest(
	    48,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    49,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    50,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Groudon[QUEST_GROUDON_SUB_COUNT] =
{
	sub_quest(
	    51,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    52,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    53,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Rayquaza[QUEST_RAYQUAZA_SUB_COUNT] =
{
	sub_quest(
	    54,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    55,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    56,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Jirachi[QUEST_JIRACHI_SUB_COUNT] =
{
	sub_quest(
	    57,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    58,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    59,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Deoxys[QUEST_DEOXYS_SUB_COUNT] =
{
	sub_quest(
	    60,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    61,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    62,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Uxie[QUEST_UXIE_SUB_COUNT] =
{
	sub_quest(
	    63,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    64,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    65,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Mesprit[QUEST_MESPRIT_SUB_COUNT] =
{
	sub_quest(
	    66,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    67,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    68,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Azelf[QUEST_AZELF_SUB_COUNT] =
{
	sub_quest(
	    69,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    70,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    71,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Dialga[QUEST_DIALGA_SUB_COUNT] =
{
	sub_quest(
	    72,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    73,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    74,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Palkia[QUEST_PALKIA_SUB_COUNT] =
{
	sub_quest(
	    75,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    76,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    77,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Heatran[QUEST_HEATRAN_SUB_COUNT] =
{
	sub_quest(
	    78,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    79,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    80,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Regigigas[QUEST_REGIGIGAS_SUB_COUNT] =
{
	sub_quest(
	    81,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    82,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    83,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Giratina[QUEST_GIRATINA_SUB_COUNT] =
{
	sub_quest(
	    84,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    85,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    86,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Cresselia[QUEST_CRESSELIA_SUB_COUNT] =
{
	sub_quest(
	    87,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    88,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    89,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Phione[QUEST_PHIONE_SUB_COUNT] =
{
	sub_quest(
	    90,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    91,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    92,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Manaphy[QUEST_MANAPHY_SUB_COUNT] =
{
	sub_quest(
	    93,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    94,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    95,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Darkrai[QUEST_DARKRAI_SUB_COUNT] =
{
	sub_quest(
	    96,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    97,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    98,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Shaymin[QUEST_SHAYMIN_SUB_COUNT] =
{
	sub_quest(
	    99,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    100,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    101,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Arceus[QUEST_ARCEUS_SUB_COUNT] =
{
	sub_quest(
	    102,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    103,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    104,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Victini[QUEST_VICTINI_SUB_COUNT] =
{
	sub_quest(
	    105,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    106,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    107,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Cobalion[QUEST_COBALION_SUB_COUNT] =
{
	sub_quest(
	    108,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    109,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    110,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Terrakion[QUEST_TERRAKION_SUB_COUNT] =
{
	sub_quest(
	    111,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    112,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    113,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Virizion[QUEST_VIRIZION_SUB_COUNT] =
{
	sub_quest(
	    114,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    115,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    116,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Tornadus[QUEST_TORNADUS_SUB_COUNT] =
{
	sub_quest(
	    117,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    118,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    119,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Thundurus[QUEST_THUNDURUS_SUB_COUNT] =
{
	sub_quest(
	    120,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    121,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    122,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Reshiram[QUEST_RESHIRAM_SUB_COUNT] =
{
	sub_quest(
	    123,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    124,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    125,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Zekrom[QUEST_ZEKROM_SUB_COUNT] =
{
	sub_quest(
	    126,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    127,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    128,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Landorus[QUEST_LANDORUS_SUB_COUNT] =
{
	sub_quest(
	    129,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    130,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    131,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Kyurem[QUEST_KYUREM_SUB_COUNT] =
{
	sub_quest(
	    132,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    133,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    134,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Keldeo[QUEST_KELDEO_SUB_COUNT] =
{
	sub_quest(
	    135,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    136,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    137,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Meloetta[QUEST_MELOETTA_SUB_COUNT] =
{
	sub_quest(
	    138,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    139,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    140,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Genesect[QUEST_GENESECT_SUB_COUNT] =
{
	sub_quest(
	    141,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    142,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    143,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Xerneas[QUEST_XERNEAS_SUB_COUNT] =
{
	sub_quest(
	    144,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    145,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    146,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Yveltal[QUEST_YVELTAL_SUB_COUNT] =
{
	sub_quest(
	    147,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    148,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    149,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Zygarde[QUEST_ZYGARDE_SUB_COUNT] =
{
	sub_quest(
	    150,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    151,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    152,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Diancie[QUEST_DIANCIE_SUB_COUNT] =
{
	sub_quest(
	    153,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    154,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    155,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Hoopa[QUEST_HOOPA_SUB_COUNT] =
{
	sub_quest(
	    156,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    157,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    158,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Volcanion[QUEST_VOLCANION_SUB_COUNT] =
{
	sub_quest(
	    159,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    160,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    161,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Tapu_Koko[QUEST_TAPU_KOKO_SUB_COUNT] =
{
	sub_quest(
	    162,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    163,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    164,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Tapu_Lele[QUEST_TAPU_LELE_SUB_COUNT] =
{
	sub_quest(
	    165,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    166,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    167,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Tapu_Bulu[QUEST_TAPU_BULU_SUB_COUNT] =
{
	sub_quest(
	    168,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    169,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    170,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Tapu_Fini[QUEST_TAPU_FINI_SUB_COUNT] =
{
	sub_quest(
	    171,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    172,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    173,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Cosmog[QUEST_COSMOG_SUB_COUNT] =
{
	sub_quest(
	    174,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    175,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    176,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Solgaleo[QUEST_SOLGALEO_SUB_COUNT] =
{
	sub_quest(
	    177,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    178,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    179,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Lunala[QUEST_LUNALA_SUB_COUNT] =
{
	sub_quest(
	    180,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    181,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    182,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Necrozma[QUEST_NECROZMA_SUB_COUNT] =
{
	sub_quest(
	    183,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    184,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    185,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Magearna[QUEST_MAGEARNA_SUB_COUNT] =
{
	sub_quest(
	    186,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    187,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    188,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Marshadow[QUEST_MARSHADOW_SUB_COUNT] =
{
	sub_quest(
	    189,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    190,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    191,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Zeraora[QUEST_ZERAORA_SUB_COUNT] =
{
	sub_quest(
	    192,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    193,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    194,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Meltan[QUEST_MELTAN_SUB_COUNT] =
{
	sub_quest(
	    195,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    196,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    197,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};
/* 
static const struct SubQuest sSubQuests_Zacian[QUEST_ZACIAN_SUB_COUNT] =
{
	sub_quest(
	    198,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    199,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    200,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Zamazenta[QUEST_ZAMAZENTA_SUB_COUNT] =
{
	sub_quest(
	    201,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    202,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    203,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Eternatus[QUEST_ETERNATUS_SUB_COUNT] =
{
	sub_quest(
	    204,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    205,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    206,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Kubfu[QUEST_KUBFU_SUB_COUNT] =
{
	sub_quest(
	    207,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    208,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    209,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Zarude[QUEST_ZARUDE_SUB_COUNT] =
{
	sub_quest(
	    210,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    211,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    212,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Articuno_Galar[QUEST_ARTICUNO_GALAR_SUB_COUNT] =
{
	sub_quest(
	    213,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    214,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    215,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Zapdos_Galar[QUEST_ZAPDOS_GALAR_SUB_COUNT] =
{
	sub_quest(
	    216,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    217,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    218,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Moltres_Galar[QUEST_MOLTRES_GALAR_SUB_COUNT] =
{
	sub_quest(
	    219,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    220,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    221,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Regieleki[QUEST_REGIELEKI_SUB_COUNT] =
{
	sub_quest(
	    222,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    223,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    224,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Regidrago[QUEST_REGIDRAGO_SUB_COUNT] =
{
	sub_quest(
	    225,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    226,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    227,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Glastrier[QUEST_GLASTRIER_SUB_COUNT] =
{
	sub_quest(
	    228,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    229,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    230,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Spectrier[QUEST_SPECTRIER_SUB_COUNT] =
{
	sub_quest(
	    231,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    232,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    233,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Calyrex[QUEST_CALYREX_SUB_COUNT] =
{
	sub_quest(
	    234,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    235,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    236,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Enamorus[QUEST_ENAMORUS_SUB_COUNT] =
{
	sub_quest(
	    237,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    238,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    239,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Wo_Chien[QUEST_WO_CHIEN_SUB_COUNT] =
{
	sub_quest(
	    240,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    241,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    242,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Chien_Pao[QUEST_CHIEN_PAO_SUB_COUNT] =
{
	sub_quest(
	    243,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    244,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    245,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Ting_Lu[QUEST_TING_LU_SUB_COUNT] =
{
	sub_quest(
	    246,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    247,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    248,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Chi_Yu[QUEST_CHI_YU_SUB_COUNT] =
{
	sub_quest(
	    249,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    250,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    251,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Koraidon[QUEST_KORAIDON_SUB_COUNT] =
{
	sub_quest(
	    252,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    253,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    254,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Miraidon[QUEST_MIRAIDON_SUB_COUNT] =
{
	sub_quest(
	    255,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    256,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    257,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Okidogi[QUEST_OKIDOGI_SUB_COUNT] =
{
	sub_quest(
	    258,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    259,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    260,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Munkidori[QUEST_MUNKIDORI_SUB_COUNT] =
{
	sub_quest(
	    261,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    262,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    263,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Fezandipiti[QUEST_FEZANDIPITI_SUB_COUNT] =
{
	sub_quest(
	    264,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    265,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    266,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Ogerpon[QUEST_OGERPON_SUB_COUNT] =
{
	sub_quest(
	    267,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    268,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    269,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Terapagos[QUEST_TERAPAGOS_SUB_COUNT] =
{
	sub_quest(
	    270,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    271,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    272,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};

static const struct SubQuest sSubQuests_Pecharunt[QUEST_PECHARUNT_SUB_COUNT] =
{
	sub_quest(
	    273,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    274,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
	sub_quest(
	    275,
	    gText_SubQuest1_Name1,
	    gText_SubQuest1_Desc1,
	    gText_SideQuestMap1,
	    OBJ_EVENT_GFX_WALLY,
	    OBJECT,
	    sText_Ready
	),
};
 */
////////////////////////END SUBQUEST CUSTOMIZATION/////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
////////////////////////BEGIN QUEST CUSTOMIZATION//////////////////////////////

//Declaration of side quest structures. Edits to quests are made here.
#define side_quest(n, d, dd, m, s, st, sq, ns) {.name = n, .desc = d, .donedesc = dd, .map = m, .sprite = s, .spritetype = st, .subquests = sq, .numSubquests = ns}
static const struct SideQuest sSideQuests[QUEST_COUNT] =
{
	side_quest(
	    gText_SideQuestName_Articuno,
	    gText_SideQuestDesc_Articuno,
	    gText_SideQuestDoneDesc_Articuno,
	    gText_SideQuestMap_Articuno,
	    SPECIES_ARTICUNO,
	    PKMN,
	    sSubQuests_Articuno,
	    QUEST_ARTICUNO_SUB_COUNT
	),
	side_quest(
	    gText_SideQuestName_Zapdos,
	    gText_SideQuestDesc_Zapdos,
	    gText_SideQuestDoneDesc_Zapdos,
	    gText_SideQuestMap_Zapdos,
	    SPECIES_ZAPDOS,
	    PKMN,
	    sSubQuests_Zapdos,
	    QUEST_ZAPDOS_SUB_COUNT
	),
	side_quest(
	    gText_SideQuestName_Moltres,
	    gText_SideQuestDesc_Moltres,
	    gText_SideQuestDoneDesc_Moltres,
	    gText_SideQuestMap_Moltres,
	    SPECIES_MOLTRES,
	    PKMN,
	    sSubQuests_Moltres,
	    QUEST_MOLTRES_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_MEWTWO,
	    PKMN,
	    sSubQuests_Mewtwo,
	    QUEST_MEWTWO_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_MEW,
	    PKMN,
	    sSubQuests_Mew,
	    QUEST_MEW_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_RAIKOU,
	    PKMN,
	    sSubQuests_Raikou,
	    QUEST_RAIKOU_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_ENTEI,
	    PKMN,
	    sSubQuests_Entei,
	    QUEST_ENTEI_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_SUICUNE,
	    PKMN,
	    sSubQuests_Suicune,
	    QUEST_SUICUNE_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_LUGIA,
	    PKMN,
	    sSubQuests_Lugia,
	    QUEST_LUGIA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_HO_OH,
	    PKMN,
	    sSubQuests_Ho_Oh,
	    QUEST_HO_OH_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_CELEBI,
	    PKMN,
	    sSubQuests_Celebi,
	    QUEST_CELEBI_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_REGIROCK,
	    PKMN,
	    sSubQuests_Regirock,
	    QUEST_REGIROCK_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_REGICE,
	    PKMN,
	    sSubQuests_Regice,
	    QUEST_REGICE_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_REGISTEEL,
	    PKMN,
	    sSubQuests_Registeel,
	    QUEST_REGISTEEL_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_LATIAS,
	    PKMN,
	    sSubQuests_Latias,
	    QUEST_LATIAS_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_LATIOS,
	    PKMN,
	    sSubQuests_Latios,
	    QUEST_LATIOS_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_KYOGRE,
	    PKMN,
	    sSubQuests_Kyogre,
	    QUEST_KYOGRE_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_GROUDON,
	    PKMN,
	    sSubQuests_Groudon,
	    QUEST_GROUDON_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_RAYQUAZA,
	    PKMN,
	    sSubQuests_Rayquaza,
	    QUEST_RAYQUAZA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_JIRACHI,
	    PKMN,
	    sSubQuests_Jirachi,
	    QUEST_JIRACHI_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_DEOXYS,
	    PKMN,
	    sSubQuests_Deoxys,
	    QUEST_DEOXYS_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_UXIE,
	    PKMN,
	    sSubQuests_Uxie,
	    QUEST_UXIE_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_MESPRIT,
	    PKMN,
	    sSubQuests_Mesprit,
	    QUEST_MESPRIT_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_AZELF,
	    PKMN,
	    sSubQuests_Azelf,
	    QUEST_AZELF_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_DIALGA,
	    PKMN,
	    sSubQuests_Dialga,
	    QUEST_DIALGA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_PALKIA,
	    PKMN,
	    sSubQuests_Palkia,
	    QUEST_PALKIA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_HEATRAN,
	    PKMN,
	    sSubQuests_Heatran,
	    QUEST_HEATRAN_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_REGIGIGAS,
	    PKMN,
	    sSubQuests_Regigigas,
	    QUEST_REGIGIGAS_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_GIRATINA,
	    PKMN,
	    sSubQuests_Giratina,
	    QUEST_GIRATINA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_CRESSELIA,
	    PKMN,
	    sSubQuests_Cresselia,
	    QUEST_CRESSELIA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_PHIONE,
	    PKMN,
	    sSubQuests_Phione,
	    QUEST_PHIONE_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_MANAPHY,
	    PKMN,
	    sSubQuests_Manaphy,
	    QUEST_MANAPHY_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_DARKRAI,
	    PKMN,
	    sSubQuests_Darkrai,
	    QUEST_DARKRAI_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_SHAYMIN,
	    PKMN,
	    sSubQuests_Shaymin,
	    QUEST_SHAYMIN_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_ARCEUS,
	    PKMN,
	    sSubQuests_Arceus,
	    QUEST_ARCEUS_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_VICTINI,
	    PKMN,
	    sSubQuests_Victini,
	    QUEST_VICTINI_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_COBALION,
	    PKMN,
	    sSubQuests_Cobalion,
	    QUEST_COBALION_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_TERRAKION,
	    PKMN,
	    sSubQuests_Terrakion,
	    QUEST_TERRAKION_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_VIRIZION,
	    PKMN,
	    sSubQuests_Virizion,
	    QUEST_VIRIZION_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_TORNADUS,
	    PKMN,
	    sSubQuests_Tornadus,
	    QUEST_TORNADUS_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_THUNDURUS,
	    PKMN,
	    sSubQuests_Thundurus,
	    QUEST_THUNDURUS_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_RESHIRAM,
	    PKMN,
	    sSubQuests_Reshiram,
	    QUEST_RESHIRAM_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_ZEKROM,
	    PKMN,
	    sSubQuests_Zekrom,
	    QUEST_ZEKROM_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_LANDORUS,
	    PKMN,
	    sSubQuests_Landorus,
	    QUEST_LANDORUS_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_KYUREM,
	    PKMN,
	    sSubQuests_Kyurem,
	    QUEST_KYUREM_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_KELDEO,
	    PKMN,
	    sSubQuests_Keldeo,
	    QUEST_KELDEO_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_MELOETTA,
	    PKMN,
	    sSubQuests_Meloetta,
	    QUEST_MELOETTA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_GENESECT,
	    PKMN,
	    sSubQuests_Genesect,
	    QUEST_GENESECT_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_XERNEAS,
	    PKMN,
	    sSubQuests_Xerneas,
	    QUEST_XERNEAS_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_YVELTAL,
	    PKMN,
	    sSubQuests_Yveltal,
	    QUEST_YVELTAL_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_ZYGARDE,
	    PKMN,
	    sSubQuests_Zygarde,
	    QUEST_ZYGARDE_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_DIANCIE,
	    PKMN,
	    sSubQuests_Diancie,
	    QUEST_DIANCIE_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_HOOPA,
	    PKMN,
	    sSubQuests_Hoopa,
	    QUEST_HOOPA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_VOLCANION,
	    PKMN,
	    sSubQuests_Volcanion,
	    QUEST_VOLCANION_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_TAPU_KOKO,
	    PKMN,
	    sSubQuests_Tapu_Koko,
	    QUEST_TAPU_KOKO_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_TAPU_LELE,
	    PKMN,
	    sSubQuests_Tapu_Lele,
	    QUEST_TAPU_LELE_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_TAPU_BULU,
	    PKMN,
	    sSubQuests_Tapu_Bulu,
	    QUEST_TAPU_BULU_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_TAPU_FINI,
	    PKMN,
	    sSubQuests_Tapu_Fini,
	    QUEST_TAPU_FINI_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_COSMOG,
	    PKMN,
	    sSubQuests_Cosmog,
	    QUEST_COSMOG_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_SOLGALEO,
	    PKMN,
	    sSubQuests_Solgaleo,
	    QUEST_SOLGALEO_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_LUNALA,
	    PKMN,
	    sSubQuests_Lunala,
	    QUEST_LUNALA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_NECROZMA,
	    PKMN,
	    sSubQuests_Necrozma,
	    QUEST_NECROZMA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_MAGEARNA,
	    PKMN,
	    sSubQuests_Magearna,
	    QUEST_MAGEARNA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_MARSHADOW,
	    PKMN,
	    sSubQuests_Marshadow,
	    QUEST_MARSHADOW_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_ZERAORA,
	    PKMN,
	    sSubQuests_Zeraora,
	    QUEST_ZERAORA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_MELTAN,
	    PKMN,
	    sSubQuests_Meltan,
	    QUEST_MELTAN_SUB_COUNT
	),/* 
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_ZACIAN,
	    PKMN,
	    sSubQuests_Zacian,
	    QUEST_ZACIAN_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_ZAMAZENTA,
	    PKMN,
	    sSubQuests_Zamazenta,
	    QUEST_ZAMAZENTA_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_ETERNATUS,
	    PKMN,
	    sSubQuests_Eternatus,
	    QUEST_ETERNATUS_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_KUBFU,
	    PKMN,
	    sSubQuests_Kubfu,
	    QUEST_KUBFU_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_ZARUDE,
	    PKMN,
	    sSubQuests_Zarude,
	    QUEST_ZARUDE_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_ARTICUNO_GALAR,
	    PKMN,
	    sSubQuests_Articuno_Galar,
	    QUEST_ARTICUNO_GALAR_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_ZAPDOS_GALAR,
	    PKMN,
	    sSubQuests_Zapdos_Galar,
	    QUEST_ZAPDOS_GALAR_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_MOLTRES_GALAR,
	    PKMN,
	    sSubQuests_Moltres_Galar,
	    QUEST_MOLTRES_GALAR_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_REGIELEKI,
	    PKMN,
	    sSubQuests_Regieleki,
	    QUEST_REGIELEKI_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_REGIDRAGO,
	    PKMN,
	    sSubQuests_Regidrago,
	    QUEST_REGIDRAGO_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_GLASTRIER,
	    PKMN,
	    sSubQuests_Glastrier,
	    QUEST_GLASTRIER_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_SPECTRIER,
	    PKMN,
	    sSubQuests_Spectrier,
	    QUEST_SPECTRIER_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_CALYREX,
	    PKMN,
	    sSubQuests_Calyrex,
	    QUEST_CALYREX_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_ENAMORUS,
	    PKMN,
	    sSubQuests_Enamorus,
	    QUEST_ENAMORUS_SUB_COUNT
	),
 	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_WO_CHIEN,
	    PKMN,
	    sSubQuests_Wo_Chien,
	    QUEST_WO_CHIEN_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_CHIEN_PAO,
	    PKMN,
	    sSubQuests_Chien_Pao,
	    QUEST_CHIEN_PAO_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_TING_LU,
	    PKMN,
	    sSubQuests_Ting_Lu,
	    QUEST_TING_LU_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_CHI_YU,
	    PKMN,
	    sSubQuests_Chi_Yu,
	    QUEST_CHI_YU_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_KORAIDON,
	    PKMN,
	    sSubQuests_Koraidon,
	    QUEST_KORAIDON_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_MIRAIDON,
	    PKMN,
	    sSubQuests_Miraidon,
	    QUEST_MIRAIDON_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_OKIDOGI,
	    PKMN,
	    sSubQuests_Okidogi,
	    QUEST_OKIDOGI_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_MUNKIDORI,
	    PKMN,
	    sSubQuests_Munkidori,
	    QUEST_MUNKIDORI_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_FEZANDIPITI,
	    PKMN,
	    sSubQuests_Fezandipiti,
	    QUEST_FEZANDIPITI_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_OGERPON,
	    PKMN,
	    sSubQuests_Ogerpon,
	    QUEST_OGERPON_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_TERAPAGOS,
	    PKMN,
	    sSubQuests_Terapagos,
	    QUEST_TERAPAGOS_SUB_COUNT
	),
	side_quest(
		gText_SideQuestName_1,
	    gText_SideQuestDesc_1,
	    gText_SideQuestDoneDesc_1,
	    gText_SideQuestMap1,
	    SPECIES_PECHARUNT,
	    PKMN,
	    sSubQuests_Pecharunt,
	    QUEST_PECHARUNT_SUB_COUNT
	), */
};

////////////////////////END QUEST CUSTOMIZATION////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//BG layer defintions
static const struct BgTemplate sQuestMenuBgTemplates[2] =
{
	{
		//All text and content is loaded to this window
		.bg = 0,
		.charBaseIndex = 0,
		.mapBaseIndex = 31,
		.priority = 1
	},
	{
		///Backgrounds and UI elements are loaded to this window
		.bg = 1,
		.charBaseIndex = 3,
		.mapBaseIndex = 30,
		.priority = 2
	}
};

//Window definitions
static const struct WindowTemplate sQuestMenuHeaderWindowTemplates[] =
{
	{
		//0: Content window
		.bg = 0,
		.tilemapLeft = 0,
		.tilemapTop = 2,
		.width = 30,
		.height = 8,
		.paletteNum = 15,
		.baseBlock = 1
	},
	{
		//1: Footer window
		.bg = 0,
		.tilemapLeft = 0,
		.tilemapTop = 12,
		.width = 30,
		.height = 12,
		.paletteNum = 15,
		.baseBlock = 361
	},
	{
		// 2: Header window
		.bg = 0,
		.tilemapLeft = 0,
		.tilemapTop = 0,
		.width = 30,
		.height = 2,
		.paletteNum = 15,
		.baseBlock = 721
	},
	DUMMY_WIN_TEMPLATE
};

//Font color combinations for printed text
static const u8 sQuestMenuWindowFontColors[][4] =
{
	{
		//Header of Quest Menu
		TEXT_COLOR_TRANSPARENT,
		TEXT_COLOR_DARK_GRAY,
		TEXT_COLOR_TRANSPARENT
	},
	{
		//Reward state progress indicator
		TEXT_COLOR_TRANSPARENT,
		TEXT_COLOR_RED,
		TEXT_COLOR_TRANSPARENT
	},
	{
		//Done state progress indicator
		TEXT_COLOR_TRANSPARENT,
		TEXT_COLOR_GREEN,
		TEXT_COLOR_TRANSPARENT
	},
	{
		//Active state progress indicator
		TEXT_COLOR_TRANSPARENT,
		TEXT_COLOR_BLUE,
		TEXT_COLOR_TRANSPARENT
	},
	{
		//Footer flavor text
		TEXT_COLOR_TRANSPARENT,
		TEXT_COLOR_WHITE,
		TEXT_COLOR_TRANSPARENT
	},
};

//Functions begin here

//ported from firered by ghoulslash
void QuestMenu_Init(u8 a0, MainCallback callback)
{
	u8 i;

	if (a0 >= 2)
	{
		SetMainCallback2(callback);
		return;
	}

	if ((sStateDataPtr = Alloc(sizeof(struct QuestMenuResources))) == NULL)
	{
		SetMainCallback2(callback);
		return;
	}

	if (a0 != 1)
	{
		sListMenuState.savedCallback = callback;
		sListMenuState.scroll = sListMenuState.row = 0;
	}

	sStateDataPtr->moveModeOrigPos = 0xFF;
	sStateDataPtr->spriteIconSlot = 0;
	sStateDataPtr->scrollIndicatorArrowPairId = 0xFF;
	sStateDataPtr->savedCallback = 0;
	for (i = 0; i < 3; i++)
	{
		sStateDataPtr->data[i] = 0;
	}

	SetMainCallback2(RunSetup);
}

static void MainCB(void)
{
	RunTasks();
	AnimateSprites();
	BuildOamBuffer();
	DoScheduledBgTilemapCopiesToVram();
	UpdatePaletteFade();
}

static void VBlankCB(void)
{
	LoadOam();
	ProcessSpriteCopyRequests();
	TransferPlttBuffer();
}

static void RunSetup(void)
{
	while (1)
	{
		if (SetupGraphics() == TRUE)
		{
			break;
		}
	}
}

static bool8 SetupGraphics(void)
{
	u8 taskId;
	switch (gMain.state)
	{
		case 0:
			SetVBlankHBlankCallbacksToNull();
			ClearScheduledBgCopiesToVram();
			gMain.state++;
			break;
		case 1:
			ScanlineEffect_Stop();
			gMain.state++;
			break;
		case 2:
			FreeAllSpritePalettes();
			gMain.state++;
			break;
		case 3:
			ResetPaletteFade();
			gMain.state++;
			break;
		case 4:
			ResetSpriteData();
			gMain.state++;
			break;
		case 5:
			ResetSpriteState();
			gMain.state++;
			break;
		case 6:
			ResetTasks();
			gMain.state++;
			break;
		case 7:
			if (InitBackgrounds())
			{
				sStateDataPtr->data[0] = 0;
				gMain.state++;
			}
			else
			{
				FadeAndBail();
				return TRUE;
			}
			break;
		case 8:
			if (LoadGraphics() == TRUE)
			{
				gMain.state++;
			}
			break;
		case 9:
			QuestMenu_InitWindows();
			gMain.state++;
			break;
		case 10:
			ClearModeOnStartup();
			InitItems();
			SetCursorPosition();
			SetScrollPosition();
			gMain.state++;
			break;
		case 11:
			if (AllocateResourcesForListMenu())
			{
				gMain.state++;
			}
			else
			{
				FadeAndBail();
				return TRUE;
			}
			break;
		case 12:
			AllocateMemoryForArray();
			BuildMenuTemplate();
			gMain.state++;
			break;
		case 13:
			GenerateAndPrintHeader();
			gMain.state++;
			break;
		case 14:
			gMain.state++;
			break;
		case 15:
			taskId = CreateTask(Task_Main, 0);
			gTasks[taskId].data[0] = ListMenuInit(&gMultiuseListMenuTemplate,
			                                      sListMenuState.scroll,
			                                      sListMenuState.row);
			gMain.state++;
			break;
		case 16:
			PlaceTopMenuScrollIndicatorArrows();
			gMain.state++;
			break;
		case 17:
			gMain.state++;
			break;
		case 18:
			if (sListMenuState.initialized == 1)
			{
				BlendPalettes(0xFFFFFFFF, 16, RGB_BLACK);
			}
			gMain.state++;
			break;
		case 19:
			if (sListMenuState.initialized == 1)
			{
				BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_BLACK);
			}
			else
			{

				BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_BLACK);
				SetInitializedFlag(1);
			}
			gMain.state++;
			break;
		default:
			SetVBlankCallback(VBlankCB);
			SetMainCallback2(MainCB);
			return TRUE;
	}
	return FALSE;
}

static bool8 LoadGraphics(void)
{
	switch (sStateDataPtr->data[0])
	{
		case 0:
			ResetTempTileDataBuffers();
			DecompressAndCopyTileDataToVram(1, sQuestMenuTiles, 0, 0, 0);
			sStateDataPtr->data[0]++;
			break;
		case 1:
			if (FreeTempTileDataBuffersIfPossible() != TRUE)
			{
				DecompressDataWithHeaderWram(sQuestMenuTilemap, sBg1TilemapBuffer);
				sStateDataPtr->data[0]++;
			}
			break;
		case 2:
			LoadPalette(sQuestMenuBgPals, 0x00, 0x60);
			sStateDataPtr->data[0]++;
			break;
		case 3:
			sStateDataPtr->data[0]++;
			break;
		default:
			sStateDataPtr->data[0] = 0;
			return TRUE;
	}
	return FALSE;
}

static void QuestMenu_InitWindows(void)
{
	u8 i;

	InitWindows(sQuestMenuHeaderWindowTemplates);
	DeactivateAllTextPrinters();

	for (i = 0; i < 3; i++)
	{
		FillWindowPixelBuffer(i, 0x00);
		PutWindowTilemap(i);
	}

	ScheduleBgCopyTilemapToVram(0);
}

static bool8 InitBackgrounds(void)
{
	ResetAllBgsCoordinatesAndBgCntRegs();
	sBg1TilemapBuffer = Alloc(0x800);
	if (sBg1TilemapBuffer == NULL)
	{
		return FALSE;
	}

	memset(sBg1TilemapBuffer, 0, 0x800);
	ResetBgsAndClearDma3BusyFlags(0);
	InitBgsFromTemplates(0, sQuestMenuBgTemplates,
	                     NELEMS(sQuestMenuBgTemplates));
	SetBgTilemapBuffer(1, sBg1TilemapBuffer);
	ScheduleBgCopyTilemapToVram(1);
	SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
	SetGpuReg(REG_OFFSET_BLDCNT, 0);
	ShowBg(0);
	ShowBg(1);
	return TRUE;
}

static void InitItems(void)
{
	sStateDataPtr->nItems = (CountNumberListRows()) - 1;

	sStateDataPtr->maxShowed = sStateDataPtr->nItems + 1 <= 4 ?
	                           sStateDataPtr->nItems + 1 : 4;
}

#define try_alloc(ptr__, size) ({ \
		void ** ptr = (void **)&(ptr__);             \
		*ptr = Alloc(size);                 \
		if (*ptr == NULL)                   \
		{                                   \
			FreeResources();                  \
			FadeAndBail();                  \
			return FALSE;                   \
		}                                   \
	})

static bool8 AllocateResourcesForListMenu(void)
{
	try_alloc(sListMenuItems,
	          sizeof(struct ListMenuItem) * CountNumberListRows() + 1);
	return TRUE;
}

void AllocateMemoryForArray(void)
{
	u8 i;
	u16 allocateRows = QUEST_ARRAY_COUNT + 1;

	questNameArray = Alloc(sizeof(void *) * allocateRows);

	for (i = 0; i < allocateRows; i++)
	{
		questNameArray[i] = Alloc(sizeof(u8) * 32);
	}
}

static void PlaceTopMenuScrollIndicatorArrows(void)
{
	u8 listSize = CountNumberListRows();

	if (listSize < sStateDataPtr->maxShowed)
	{
		listSize = sStateDataPtr->maxShowed;
	}

	sStateDataPtr->scrollIndicatorArrowPairId =
	      AddScrollIndicatorArrowPairParameterized(2, 94, 8, 90,
	                  (listSize - sStateDataPtr->maxShowed), 110, 110, &sListMenuState.scroll);
}

static void SetInitializedFlag(u8 a0)
{
	sListMenuState.initialized = a0;
}

static u8 GetCursorPosition(void)
{
	return sListMenuState.scroll + sListMenuState.row;
}

static void SetCursorPosition(void)
{
	if (IfScrollIsOutOfBounds())
	{
		sListMenuState.scroll = (sStateDataPtr->nItems + 1) -
		                        sStateDataPtr->maxShowed;
	}

	if (IfRowIsOutOfBounds())
	{
		if (sStateDataPtr->nItems + 1 < 2)
		{
			sListMenuState.row = 0;
		}
		else
		{
			sListMenuState.row = sStateDataPtr->nItems;
		}
	}
}


static void SetScrollPosition(void)
{
	u8 i;

	if (sListMenuState.row > 3)
	{
		for (i = 0; i <= sListMenuState.row - 3;
		            sListMenuState.row--, sListMenuState.scroll++, i++)
		{
			if (sListMenuState.scroll + sStateDataPtr->maxShowed ==
			            sStateDataPtr->nItems + 1)
			{
				break;
			}
		}
	}
}

bool8 IfScrollIsOutOfBounds(void)
{
	if (sListMenuState.scroll != 0
	            && sListMenuState.scroll + sStateDataPtr->maxShowed >
	            sStateDataPtr->nItems + 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool8 IfRowIsOutOfBounds(void)
{
	if (sListMenuState.scroll + sListMenuState.row >= sStateDataPtr->nItems +
	            1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static void SaveScrollAndRow(s16 *data)
{
	ListMenuGetScrollAndRow(data[0], &sListMenuState.storedScrollOffset,
	                        &sListMenuState.storedRowPosition);
}


void ClearModeOnStartup(void)
{
	sStateDataPtr->filterMode = 0;
}

static u8 ManageMode(u8 action)
{
	u8 mode = sStateDataPtr->filterMode;

	switch (action)
	{
		case SUB:
			mode = ToggleSubquestMode(mode);
			break;

		case ALPHA:
			mode = ToggleAlphaMode(mode);
			sStateDataPtr->restoreCursor = FALSE;
			break;

		default:
			mode = IncrementMode(mode);
			sStateDataPtr->restoreCursor = FALSE;
			break;
	}
	return mode;
}

u8 ToggleSubquestMode(u8 mode)
{
	if (IsSubquestMode())
	{
		mode -= SORT_SUBQUEST;
		sStateDataPtr->restoreCursor = TRUE;
	}
	else
	{
		mode += SORT_SUBQUEST;
		sStateDataPtr->restoreCursor = FALSE;
	}

	return mode;
}

u8 ToggleAlphaMode(u8 mode)
{
	if (IsAlphaMode())
	{
		mode -= SORT_DEFAULT_AZ;
	}
	else
	{
		mode += SORT_DEFAULT_AZ;
	}

	return mode;
}

u8 IncrementMode(u8 mode)
{
	if (mode % 10 == SORT_DONE)
	{
		mode -= SORT_DONE;
	}
	else
	{
		mode++;
	}

	return mode;
}

static bool8 IsSubquestMode(void)
{
	if (sStateDataPtr->filterMode > SORT_DONE_AZ)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static bool8 IsNotFilteredMode(void)
{
	u8 mode = sStateDataPtr->filterMode % 10;

	if (mode == FLAG_GET_UNLOCKED)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static bool8 IsAlphaMode(void)
{
	if (sStateDataPtr->filterMode < SORT_SUBQUEST
	            && sStateDataPtr->filterMode > SORT_DONE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static void BuildMenuTemplate(void)
{
	u8 lastRow = GetModeAndGenerateList();

	AssignCancelNameAndId(lastRow);

	gMultiuseListMenuTemplate.totalItems = CountNumberListRows();
	gMultiuseListMenuTemplate.items = sListMenuItems;
	gMultiuseListMenuTemplate.windowId = 0;
	gMultiuseListMenuTemplate.header_X = 0;
	gMultiuseListMenuTemplate.cursor_X = 15;
	gMultiuseListMenuTemplate.item_X = 23;
	gMultiuseListMenuTemplate.lettersSpacing = 1;
	gMultiuseListMenuTemplate.itemVerticalPadding = 2;
	gMultiuseListMenuTemplate.upText_Y = 2;
	gMultiuseListMenuTemplate.maxShowed = sStateDataPtr->maxShowed;
	gMultiuseListMenuTemplate.fontId = 2;
	gMultiuseListMenuTemplate.cursorPal = 2;
	gMultiuseListMenuTemplate.fillValue = 0;
	gMultiuseListMenuTemplate.cursorShadowPal = 0;
	gMultiuseListMenuTemplate.moveCursorFunc = MoveCursorFunc;
	gMultiuseListMenuTemplate.itemPrintFunc = GenerateStateAndPrint;
	gMultiuseListMenuTemplate.scrollMultiple = LIST_MULTIPLE_SCROLL_DPAD;
	gMultiuseListMenuTemplate.cursorKind = 0;
}

u8 GetModeAndGenerateList()
{
	if (IsSubquestMode())
	{
		return GenerateSubquestList();
	}
	else
	{
		return GenerateList(!IsNotFilteredMode());
	}
}

static u8 CountNumberListRows()
{
	u8 mode = sStateDataPtr->filterMode % 10;

	if (IsSubquestMode())
	{
		return sSideQuests[sStateDataPtr->parentQuest].numSubquests + 1;
	}

	switch (mode)
	{
		case SORT_DEFAULT:
			return QUEST_COUNT + 1;
		case SORT_INACTIVE:
			return CountInactiveQuests() + 1;
		case SORT_ACTIVE:
			return CountActiveQuests() + 1;
		case SORT_REWARD:
			return CountRewardQuests() + 1;
		case SORT_DONE:
			return CountCompletedQuests() + 1;
	}
	
	return 1;
}

u8 *DefineQuestOrder()
{
	static u8 sortedList[QUEST_COUNT];
	u8 a, c, d;
	u8 placeholderVariable;

	for (a = 0; a < QUEST_COUNT; a++)
	{
		sortedList[a] = a;
	}

	if (IsAlphaMode())
	{
		for (c = 0; c < QUEST_COUNT; c++)
		{
			for (d = c + 1; d < QUEST_COUNT; d++)
			{
				if (StringCompare(sSideQuests[sortedList[c]].name,
				                  sSideQuests[sortedList[d]].name) > 0)
				{
					placeholderVariable = sortedList[c];
					sortedList[c] = sortedList[d];
					sortedList[d] = placeholderVariable;
				}
			}
		}
	}

	return sortedList;
}

u8 GenerateSubquestList()
{
	u8 parentQuest = sStateDataPtr->parentQuest;
	u8 lastRow = 0, numRow = 0, countQuest = 0;

	for (numRow = 0; numRow < sSideQuests[parentQuest].numSubquests; numRow++)
	{
		PrependQuestNumber(countQuest);
		PopulateSubquestName(parentQuest, countQuest);
		PopulateListRowNameAndId(numRow, countQuest);

		countQuest++;
		lastRow = numRow + 1;
	}
	return lastRow;
}

u8 GenerateList(bool8 isFiltered)
{
	u8 mode = sStateDataPtr-> filterMode % 10;
	u8 numRow = 0, offset = 0, newRow = 0, countQuest = 0,
	   selectedQuestId = 0;
	u8 *sortedQuestList;

	sortedQuestList = DefineQuestOrder();

	for (countQuest = 0; countQuest < QUEST_COUNT; countQuest++)
	{
		selectedQuestId = *(sortedQuestList + countQuest);

		if (isFiltered && !QuestMenu_GetSetQuestState(selectedQuestId, mode))
		{
			continue;
		}

		PopulateEmptyRow(selectedQuestId);

		if (QuestMenu_GetSetQuestState(selectedQuestId, FLAG_GET_FAVORITE))
		{
			SetFavoriteQuest(selectedQuestId);
			newRow = numRow;
			numRow++;
		}
		else
		{
			newRow = CountFavoriteQuests() + offset;
			offset++;
		}

		PopulateQuestName(selectedQuestId);
		PopulateListRowNameAndId(newRow, selectedQuestId);
	}
	return numRow + offset;
}

static void AssignCancelNameAndId(u8 numRow)
{
	if (IsSubquestMode())
	{
		sListMenuItems[numRow].name = sText_Back;
	}
	else
	{
		sListMenuItems[numRow].name = sText_Close;
	}

	sListMenuItems[numRow].id = LIST_CANCEL;
}

u8 QuestMenu_GetSetSubquestState(u8 quest, u8 caseId, u8 childQuest)
{

	u8 uniqueId = sSideQuests[quest].subquests[childQuest].id;
	u8  index = uniqueId / 8; //8 bits per byte
	u8	bit = uniqueId % 8;
	u8	mask = 1 << bit;

	switch (caseId)
	{
		case FLAG_GET_COMPLETED:
			return gSaveBlock2Ptr->subQuests[index] & mask;
		case FLAG_SET_COMPLETED:
			gSaveBlock2Ptr->subQuests[index] |= mask;
			return 1;
	}

	return -1;
}

u8 QuestMenu_GetSetQuestState(u8 quest, u8 caseId)
{
	u8 index = quest * 5 / 8;
	u8 bit = quest * 5 % 8;
	u8 mask = 0, index2 = 0, bit2 = 0, index3 = 0, bit3 = 0, mask2 = 0,
	   mask3 = 0;

	// 0 : locked
	// 1 : actived
	// 2 : rewarded
	// 3 : completed
	// 4 : favorited

	switch (caseId)
	{
		case FLAG_GET_UNLOCKED:
		case FLAG_SET_UNLOCKED:
			break;
		case FLAG_GET_INACTIVE:
		case FLAG_GET_ACTIVE:
		case FLAG_SET_ACTIVE:
		case FLAG_REMOVE_ACTIVE:
			bit += 1;
			break;
		case FLAG_GET_REWARD:
		case FLAG_SET_REWARD:
		case FLAG_REMOVE_REWARD:
			bit += 2;
			break;
		case FLAG_GET_COMPLETED:
		case FLAG_SET_COMPLETED:
			bit += 3;
			break;
		case FLAG_GET_FAVORITE:
		case FLAG_SET_FAVORITE:
		case FLAG_REMOVE_FAVORITE:
			bit += 4;
			break;
	}
	if (bit >= 8)
	{
		index += 1;
		bit %= 8;
	}
	mask = 1 << bit;

	switch (caseId)
	{
		case FLAG_GET_UNLOCKED:
			return gSaveBlock2Ptr->questData[index] & mask;
		case FLAG_SET_UNLOCKED:
			gSaveBlock2Ptr->questData[index] |= mask;
			return 1;
		case FLAG_GET_INACTIVE:
			bit2 = bit + 1;
			bit3 = bit + 2;
			index2 = index;
			index3 = index;

			if (bit2 >= 8)
			{
				index2 += 1;
				bit2 %= 8;
			}
			if (bit3 >= 8)
			{
				index3 += 1;
				bit3 %= 8;
			}

			mask2 = 1 << bit2;
			mask3 = 1 << bit3;
			return !(gSaveBlock2Ptr->questData[index] & mask) && \
			       !(gSaveBlock2Ptr->questData[index2] & mask2) && \
			       !(gSaveBlock2Ptr->questData[index3] & mask3);
		case FLAG_GET_ACTIVE:
			return gSaveBlock2Ptr->questData[index] & mask;
		case FLAG_SET_ACTIVE:
			gSaveBlock2Ptr->questData[index] |= mask;
			return 1;
		case FLAG_REMOVE_ACTIVE:
			gSaveBlock2Ptr->questData[index] &= ~mask;
			return 1;
		case FLAG_GET_REWARD:
			return gSaveBlock2Ptr->questData[index] & mask;
		case FLAG_SET_REWARD:
			gSaveBlock2Ptr->questData[index] |= mask;
			return 1;
		case FLAG_REMOVE_REWARD:
			gSaveBlock2Ptr->questData[index] &= ~mask;
			return 1;
		case FLAG_GET_COMPLETED:
			return gSaveBlock2Ptr->questData[index] & mask;
		case FLAG_SET_COMPLETED:
			gSaveBlock2Ptr->questData[index] |= mask;
			return 1;
		case FLAG_GET_FAVORITE:
			return gSaveBlock2Ptr->questData[index] & mask;
		case FLAG_SET_FAVORITE:
			gSaveBlock2Ptr->questData[index] |= mask;
			return 1;
		case FLAG_REMOVE_FAVORITE:
			gSaveBlock2Ptr->questData[index] &= ~mask;
			return 1;
	}
	return -1;  //failure
}

u8 CountUnlockedQuests(void)
{
	u8 q = 0, i = 0;

	for (i = 0; i < QUEST_COUNT; i++)
	{
		if (QuestMenu_GetSetQuestState(i, FLAG_GET_UNLOCKED))
		{
			q++;
		}
	}
	return q;
}

u8 CountInactiveQuests(void)
{
	u8 q = 0, i = 0;

	for (i = 0; i < QUEST_COUNT; i++)
	{
		if (QuestMenu_GetSetQuestState(i, FLAG_GET_INACTIVE))
		{
			q++;
		}
	}
	return q;
}

u8 CountActiveQuests(void)
{
	u8 q = 0, i = 0;

	for (i = 0; i < QUEST_COUNT; i++)
	{
		if (QuestMenu_GetSetQuestState(i, FLAG_GET_ACTIVE))
		{
			q++;
		}
	}
	return q;
}

u8 CountRewardQuests(void)
{
	u8 q = 0, i = 0;

	for (i = 0; i < QUEST_COUNT; i++)
	{
		if (QuestMenu_GetSetQuestState(i, FLAG_GET_REWARD))
		{
			q++;
		}
	}
	return q;
}

u8 CountCompletedQuests(void)
{
	u8 q = 0, i = 0;

	u8 parentQuest = sStateDataPtr->parentQuest;

	if (IsSubquestMode())
	{
		for (i = 0; i < sSideQuests[parentQuest].numSubquests; i++)
		{
			if (QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_COMPLETED, i))
			{
				q++;
			}
		}
	}
	else
	{
		for (i = 0; i < QUEST_COUNT; i++)
		{
			if (QuestMenu_GetSetQuestState(i, FLAG_GET_COMPLETED))
			{
				q++;
			}
		}
	}

	return q;
}

u8 CountFavoriteQuests(void)
{
	u8 q = 0, i = 0, x = 0;
	u8 mode = sStateDataPtr->filterMode % 10;

	for (i = 0; i < QUEST_COUNT; i++)
	{
		if (QuestMenu_GetSetQuestState(i, FLAG_GET_FAVORITE))
		{
			if (QuestMenu_GetSetQuestState(i, mode))
			{
				x++;
			}
			q++;
		}
	}

	if (IsNotFilteredMode())
	{
		return q;
	}
	else
	{
		return x;
	}

}

void PopulateEmptyRow(u8 countQuest)
{
	questNamePointer = StringCopy(questNameArray[countQuest], sText_Empty);
}
void PrependQuestNumber(u8 countQuest)
{
	questNamePointer = ConvertIntToDecimalStringN(questNameArray[countQuest],
	                   countQuest + 1, STR_CONV_MODE_LEFT_ALIGN, 2);
	questNamePointer = StringAppend(questNamePointer,
	                                sText_DotSpace);
}

void SetFavoriteQuest(u8 countQuest)
{
	questNamePointer = StringAppend(questNameArray[countQuest],
	                                sText_ColorGreen);
}

void PopulateQuestName(u8 countQuest)
{
	if (QuestMenu_GetSetQuestState(countQuest, FLAG_GET_UNLOCKED))
	{
		questNamePointer = StringAppend(questNameArray[countQuest],
		                                sSideQuests[countQuest].name);
		AddSubQuestButton(countQuest);
	}
	else
	{
		StringAppend(questNameArray[countQuest], sText_Unk);
	}
}

void PopulateSubquestName(u8 parentQuest, u8 countQuest)
{
	if (IsSubquestCompletedState(countQuest))
	{
		questNamePointer = StringAppend(questNamePointer,
		                                sSideQuests[parentQuest].subquests[countQuest].name);
	}
	else
	{
		questNamePointer = StringAppend(questNamePointer, sText_Unk);
	}
}

void PopulateListRowNameAndId(u8 row, u8 countQuest)
{
	sListMenuItems[row].name = questNameArray[countQuest];
	sListMenuItems[row].id = countQuest;
}

static bool8 DoesQuestHaveChildrenAndNotInactive(u16 itemId)
{
	if (sSideQuests[itemId].numSubquests != 0
	            && QuestMenu_GetSetQuestState(itemId, FLAG_GET_UNLOCKED)
	            && !QuestMenu_GetSetQuestState(itemId, FLAG_GET_INACTIVE))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void AddSubQuestButton(u8 countQuest)
{
	if (DoesQuestHaveChildrenAndNotInactive(countQuest))
	{
		questNamePointer = StringAppend(questNameArray[countQuest],
		                                sText_SubQuestButton);
	}

}
static void QuestMenu_AddTextPrinterParameterized(u8 windowId, u8 fontId,
            const u8 *str, u8 x, u8 y,
            u8 letterSpacing, u8 lineSpacing, u8 speed, u8 colorIdx)
{
	AddTextPrinterParameterized4(windowId, fontId, x, y, letterSpacing,
	                             lineSpacing,
	                             sQuestMenuWindowFontColors[colorIdx], speed, str);
}

static void MoveCursorFunc(s32 questId, bool8 onInit,
                           struct ListMenu *list)
{
	PlayCursorSound(onInit);

	if (sStateDataPtr->moveModeOrigPos == 0xFF)
	{
		QuestMenu_DestroySprite(sStateDataPtr->spriteIconSlot ^ 1);
		sStateDataPtr->spriteIconSlot ^= 1;

		if (questId == LIST_CANCEL)
		{
			PrintDetailsForCancel();
		}
		else
		{
			GenerateAndPrintQuestDetails(questId);
			DetermineSpriteType(questId);
		}
	}
}

static void PlayCursorSound(bool8 firstRun)
{
	if (firstRun == FALSE)
	{
		PlaySE(SE_RG_BAG_CURSOR);
	}
}

static void PrintDetailsForCancel()
{
	FillWindowPixelBuffer(1, 0);

	QuestMenu_AddTextPrinterParameterized(1, 2, sText_Empty, 2, 3, 2, 0, 0,
	                                      0);
	QuestMenu_AddTextPrinterParameterized(1, 2, sText_Empty, 40, 19, 5, 0, 0,
	                                      0);

	QuestMenu_CreateSprite(-1, sStateDataPtr->spriteIconSlot, ITEM);
}

void GenerateAndPrintQuestDetails(s32 questId)
{
	GenerateQuestLocation(questId);
	PrintQuestLocation(questId);
	GenerateQuestFlavorText(questId);
	PrintQuestFlavorText(questId);
}
void GenerateQuestLocation(s32 questId)
{
	if (!IsSubquestMode())
	{
		StringCopy(gStringVar2, sSideQuests[questId].map);
	}
	else
	{
		StringCopy(gStringVar2,
		           sSideQuests[sStateDataPtr->parentQuest].subquests[questId].map);
	}

	StringExpandPlaceholders(gStringVar4, sText_ShowLocation);
}
void PrintQuestLocation(s32 questId)
{
	FillWindowPixelBuffer(1, 0);
	QuestMenu_AddTextPrinterParameterized(1, 2, gStringVar4, 2, 3, 2, 0, 0,
	                                      4);
}
void GenerateQuestFlavorText(s32 questId)
{
	if (IsSubquestMode() == FALSE)
	{
		if (IsQuestInactiveState(questId) == TRUE)
		{
			StringCopy(gStringVar1, sText_StartForMore);
		}
		if (IsQuestActiveState(questId) == TRUE)
		{
			UpdateQuestFlavorText(questId);
		}
		if (IsQuestRewardState(questId) == TRUE)
		{
			StringCopy(gStringVar1, sText_ReturnRecieveReward);
		}
		if (IsQuestCompletedState(questId) == TRUE)
		{
			StringCopy(gStringVar1, sSideQuests[questId].donedesc);
		}
	}
	else
	{
		if (IsSubquestCompletedState(questId) == TRUE)
		{
			StringCopy(gStringVar1,
			           sSideQuests[sStateDataPtr->parentQuest].subquests[questId].desc);
		}
		else
		{
			StringCopy(gStringVar1, sText_Empty);
		}
	}

	StringExpandPlaceholders(gStringVar3, gStringVar1);
}
void UpdateQuestFlavorText(s32 questId)
{
	StringCopy(gStringVar1, sSideQuests[questId].desc);
}
void PrintQuestFlavorText(s32 questId)
{
	QuestMenu_AddTextPrinterParameterized(1, 2, gStringVar3, 40, 19, 5, 0, 0,
	                                      4);
}

bool8 IsSubquestCompletedState(s32 questId)
{
	if (QuestMenu_GetSetSubquestState(sStateDataPtr->parentQuest,
	                                  FLAG_GET_COMPLETED,
	                                  questId))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
bool8 IsQuestRewardState(s32 questId)
{
	if (QuestMenu_GetSetQuestState(questId, FLAG_GET_REWARD))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool8 IsQuestInactiveState(s32 questId)
{
	if (!QuestMenu_GetSetQuestState(questId, FLAG_GET_ACTIVE))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool8 IsQuestActiveState(s32 questId)
{
	if (QuestMenu_GetSetQuestState(questId, FLAG_GET_ACTIVE))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool8 IsQuestCompletedState(s32 questId)
{
	if (QuestMenu_GetSetQuestState(questId, FLAG_GET_COMPLETED))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool8 UNUSED IsQuestUnlocked(s32 questId)
{
	if (QuestMenu_GetSetQuestState(questId, FLAG_GET_UNLOCKED))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void DetermineSpriteType(s32 questId)
{
	u16 spriteId;
	u8 spriteType;

	if (IsSubquestMode() == FALSE)
	{
		spriteId = sSideQuests[questId].sprite;
		spriteType = sSideQuests[questId].spritetype;

		QuestMenu_CreateSprite(spriteId, sStateDataPtr->spriteIconSlot,
		                       spriteType);
	}
	else if (IsSubquestCompletedState(questId) == TRUE)
	{
		spriteId =
		      sSideQuests[sStateDataPtr->parentQuest].subquests[questId].sprite;
		spriteType =
		      sSideQuests[sStateDataPtr->parentQuest].subquests[questId].spritetype;
		QuestMenu_CreateSprite(spriteId, sStateDataPtr->spriteIconSlot,
		                       spriteType);
	}
	else
	{
		QuestMenu_CreateSprite(ITEM_NONE, sStateDataPtr->spriteIconSlot, ITEM);
	}
	QuestMenu_DestroySprite(sStateDataPtr->spriteIconSlot ^ 1);
	sStateDataPtr->spriteIconSlot ^= 1;
}
static void QuestMenu_CreateSprite(u16 itemId, u8 idx, u8 spriteType)
{
	u8 *ptr = &sItemMenuIconSpriteIds[10];
	u8 spriteId = 0x00;

	if (ptr[idx] == 0xFF)
	{
		FreeSpriteTilesByTag(102 + idx);
		FreeSpritePaletteByTag(102 + idx);

		switch (spriteType)
		{
			case OBJECT:
				spriteId = CreateObjectGraphicsSprite(itemId, SpriteCallbackDummy, 20,
				                                      132, 0);
				break;
			case ITEM:
				spriteId = AddItemIconSprite(102 + idx, 102 + idx, itemId);
				break;
			case PKMN:
				LoadMonIconPalettes();
				spriteId = CreateMonIcon(itemId, SpriteCallbackDummy, 20, 132, 0, 1);
				break;
			default:
				break;
		}

		gSprites[spriteId].oam.objMode = ST_OAM_OBJ_BLEND;

		if (spriteId != MAX_SPRITES)
		{
			ptr[idx] = spriteId;

			if (spriteType == ITEM)
			{
				gSprites[spriteId].x2 = 24;
				gSprites[spriteId].y2 = 140;
			}
		}
	}
}

void ResetSpriteState(void)
{
	u16 i;

	for (i = 0; i < NELEMS(sItemMenuIconSpriteIds); i++)
	{
		sItemMenuIconSpriteIds[i] = 0xFF;
	}
}

static void QuestMenu_DestroySprite(u8 idx)
{
	u8 *ptr = &sItemMenuIconSpriteIds[10];

	if (ptr[idx] != 0xFF)
	{
		u16 palTag = GetSpritePaletteTagByPaletteNum(
		                   gSprites[ptr[idx]].oam.paletteNum);
		DestroySprite(&gSprites[ptr[idx]]);
		ptr[idx] = 0xFF;

		if (sStateDataPtr->oldPaletteTag != palTag)
		{
			if (sStateDataPtr->oldPaletteTag != 0)
			{
				FreeSpriteTilesByTag(sStateDataPtr->oldPaletteTag);
				sStateDataPtr->oldPaletteTag = palTag;
			}
		}
	}
}
static void GenerateStateAndPrint(u8 windowId, u32 questId,
                                  u8 y)
{
	u8 colorIndex;

	if (questId != LIST_CANCEL)
	{
		if (IsSubquestMode())
		{
			colorIndex = GenerateSubquestState(questId);
		}
		else
		{
			colorIndex = GenerateQuestState(questId);
		}

		PrintQuestState(windowId, y, colorIndex);
	}
}

u8 GenerateSubquestState(u8 questId)
{
	u8 parentQuest = sStateDataPtr->parentQuest;

	if (QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_COMPLETED,
	                                  questId))
	{
		StringCopy(gStringVar4, sSideQuests[parentQuest].subquests[questId].type);
	}
	else
	{
		StringCopy(gStringVar4, sText_Empty);
	}

	return 2;
}

u8 GenerateQuestState(u8 questId)
{
	if (QuestMenu_GetSetQuestState(questId, FLAG_GET_COMPLETED))
	{
		StringCopy(gStringVar4, sText_Complete);
		return 2;
	}
	else if (QuestMenu_GetSetQuestState(questId, FLAG_GET_REWARD))
	{
		StringCopy(gStringVar4, sText_Reward);
		return 1;
	}
	else if (QuestMenu_GetSetQuestState(questId, FLAG_GET_ACTIVE))
	{
		StringCopy(gStringVar4, sText_Active);
		return 3;
	}
	else
	{
		StringCopy(gStringVar4, sText_Empty);
	}

	return 0;
}

void PrintQuestState(u8 windowId, u8 y, u8 colorIndex)
{
	QuestMenu_AddTextPrinterParameterized(windowId, 0, gStringVar4, 200, y, 0,
	                                      0, 0xFF, colorIndex);
}

static void GenerateAndPrintHeader(void)
{
	GenerateDenominatorNumQuests();
	GenerateNumeratorNumQuests();
	GenerateMenuContext();

	PrintNumQuests();
	PrintMenuContext();

	if (!IsSubquestMode())
	{
		PrintTypeFilterButton();
	}
}
static void GenerateDenominatorNumQuests(void)
{
	ConvertIntToDecimalStringN(gStringVar2, QUEST_COUNT,
	                           STR_CONV_MODE_LEFT_ALIGN, 6);
}

static void GenerateNumeratorNumQuests(void)
{
	u8 mode = sStateDataPtr->filterMode % 10;
	u8 parentQuest = sStateDataPtr->parentQuest;

	switch (mode)
	{
		case SORT_DEFAULT:
			ConvertIntToDecimalStringN(gStringVar1, CountUnlockedQuests(),
			                           STR_CONV_MODE_LEFT_ALIGN,
			                           6);
			break;
		case SORT_INACTIVE:
			ConvertIntToDecimalStringN(gStringVar1, CountInactiveQuests(),
			                           STR_CONV_MODE_LEFT_ALIGN,
			                           6);
			break;
		case SORT_ACTIVE:
			ConvertIntToDecimalStringN(gStringVar1, CountActiveQuests(),
			                           STR_CONV_MODE_LEFT_ALIGN, 6);
			break;
		case SORT_REWARD:
			ConvertIntToDecimalStringN(gStringVar1, CountRewardQuests(),
			                           STR_CONV_MODE_LEFT_ALIGN, 6);
			break;
		case SORT_DONE:
			ConvertIntToDecimalStringN(gStringVar1, CountCompletedQuests(),
			                           STR_CONV_MODE_LEFT_ALIGN,
			                           6);
			break;
	}

	if (IsSubquestMode())
	{
		ConvertIntToDecimalStringN(gStringVar2,
		                           sSideQuests[parentQuest].numSubquests,
		                           STR_CONV_MODE_LEFT_ALIGN, 6);
		ConvertIntToDecimalStringN(gStringVar1, CountCompletedQuests(),
		                           STR_CONV_MODE_LEFT_ALIGN,
		                           6);
	}
}

static void GenerateMenuContext(void)
{
	u8 mode = sStateDataPtr->filterMode % 10;
	u8 parentQuest = sStateDataPtr->parentQuest;

	switch (mode)
	{
		case SORT_DEFAULT:
			questNamePointer = StringCopy(questNameArray[QUEST_ARRAY_COUNT],
			                              sText_AllHeader);
			break;
		case SORT_INACTIVE:
			questNamePointer = StringCopy(questNameArray[QUEST_ARRAY_COUNT],
			                              sText_InactiveHeader);
			break;
		case SORT_ACTIVE:
			questNamePointer = StringCopy(questNameArray[QUEST_ARRAY_COUNT],
			                              sText_ActiveHeader);
			break;
		case SORT_REWARD:
			questNamePointer = StringCopy(questNameArray[QUEST_ARRAY_COUNT],
			                              sText_RewardHeader);
			break;
		case SORT_DONE:
			questNamePointer = StringCopy(questNameArray[QUEST_ARRAY_COUNT],
			                              sText_CompletedHeader);
			break;
	}

	if (IsAlphaMode())
	{
		questNamePointer = StringAppend(questNameArray[QUEST_ARRAY_COUNT],
		                                sText_AZ);
	}
	if (IsSubquestMode())
	{
		questNamePointer = StringCopy(questNameArray[QUEST_ARRAY_COUNT],
		                              sSideQuests[parentQuest].name);

	}
}

static void PrintNumQuests(void)
{
	StringExpandPlaceholders(gStringVar4, sText_QuestNumberDisplay);
	QuestMenu_AddTextPrinterParameterized(2, 0, gStringVar4, 167, 1, 0, 1, 0,
	                                      0);
}
static void PrintMenuContext(void)
{
	QuestMenu_AddTextPrinterParameterized(2, 0,
	                                      questNameArray[QUEST_ARRAY_COUNT], 10, 1, 0, 1, 0, 0);
}
static void PrintTypeFilterButton(void)
{
	QuestMenu_AddTextPrinterParameterized(2, 0, sText_Type, 198, 1,
	                                      0, 1, 0, 0);

}

static void Task_Main(u8 taskId)
{
	s16 *data = gTasks[taskId].data;
	s32 input = ListMenu_ProcessInput(data[0]);

	u8 selectedQuestId = sListMenuItems[GetCursorPosition()].id;

	if (!gPaletteFade.active)
	{
		ListMenuGetScrollAndRow(data[0], &sListMenuState.scroll,
		                        &sListMenuState.row);

		switch (input)
		{
			case LIST_NOTHING_CHOSEN:
				if (JOY_NEW(R_BUTTON))
				{
					ChangeModeAndCleanUp(taskId);
				}
				if (JOY_NEW(START_BUTTON))
				{
					ToggleAlphaModeAndCleanUp(taskId);
				}
				if (JOY_NEW(SELECT_BUTTON))
				{
					ToggleFavoriteAndCleanUp(taskId, selectedQuestId);
				}
				break;

			case LIST_CANCEL:
				if (IsSubquestMode())
				{
					ReturnFromSubquestAndCleanUp(taskId);
				}
				else
				{
					TurnOffQuestMenu(taskId);
				}
				break;

			default:
				if (!IsSubquestMode())
				{
					EnterSubquestModeAndCleanUp(taskId, data, input);
				}
				break;
		}
	}
}

void ManageFavorites(u8 selectedQuestId)
{
	if (QuestMenu_GetSetQuestState(selectedQuestId, FLAG_GET_FAVORITE))
	{
		QuestMenu_GetSetQuestState(selectedQuestId, FLAG_REMOVE_FAVORITE);
	}
	else
	{
		QuestMenu_GetSetQuestState(selectedQuestId, FLAG_SET_FAVORITE);
	}
}

static void Task_QuestMenuCleanUp(u8 taskId)
{
	s16 *data = gTasks[taskId].data;

	QuestMenu_RemoveScrollIndicatorArrowPair();
	DestroyListMenuTask(data[0], &sListMenuState.scroll, &sListMenuState.row);
	ClearStdWindowAndFrameToTransparent(2, FALSE);

	InitItems();
	GenerateAndPrintHeader();
	AllocateResourcesForListMenu();
	BuildMenuTemplate();
	PlaceTopMenuScrollIndicatorArrows();

	if (sStateDataPtr->restoreCursor == TRUE)
	{
		RestoreSavedScrollAndRow(data);
	}
	else
	{
		ResetCursorToTop(data);
	}

}

static void RestoreSavedScrollAndRow(s16 *data)
{
	data[0] = ListMenuInit(&gMultiuseListMenuTemplate,
	                       sListMenuState.storedScrollOffset,
	                       sListMenuState.storedRowPosition);
}
static void ResetCursorToTop(s16 *data)
{
	sListMenuState.row = 0;
	sListMenuState.scroll = 0;
	data[0] = ListMenuInit(&gMultiuseListMenuTemplate, sListMenuState.scroll,
	                       sListMenuState.row);
}

static void QuestMenu_RemoveScrollIndicatorArrowPair(void)
{
	if (sStateDataPtr->scrollIndicatorArrowPairId != 0xFF)
	{
		RemoveScrollIndicatorArrowPair(sStateDataPtr->scrollIndicatorArrowPairId);
		sStateDataPtr->scrollIndicatorArrowPairId = 0xFF;
	}
}


void EnterSubquestModeAndCleanUp(u8 taskId, s16 *data,
                                 s32 input)
{
	if (DoesQuestHaveChildrenAndNotInactive(input))
	{
		PrepareFadeOut(taskId);

		PlaySE(SE_SELECT);
		sStateDataPtr->parentQuest = input;
		sStateDataPtr->filterMode = ManageMode(SUB);
		SaveScrollAndRow(data);
		gTasks[taskId].func = Task_FadeOut;
	}
}
void ChangeModeAndCleanUp(u8 taskId)
{
	if (!IsSubquestMode())
	{
		PlaySE(SE_SELECT);
		sStateDataPtr->filterMode = ManageMode(INCREMENT);
		Task_QuestMenuCleanUp(taskId);
	}
}
void ToggleAlphaModeAndCleanUp(u8 taskId)
{
	if (!IsSubquestMode())
	{
		PlaySE(SE_SELECT);
		sStateDataPtr->filterMode = ManageMode(ALPHA);
		Task_QuestMenuCleanUp(taskId);
	}
}
void ToggleFavoriteAndCleanUp(u8 taskId, u8 selectedQuestId)
{
	if (!IsSubquestMode()
	            && !CheckSelectedIsCancel(selectedQuestId))
	{
		PlaySE(SE_SELECT);
		ManageFavorites(selectedQuestId);
		sStateDataPtr->restoreCursor = FALSE;
		Task_QuestMenuCleanUp(taskId);
	}
}
bool8 CheckSelectedIsCancel(u8 selectedQuestId)
{
	if (selectedQuestId == (0xFF - 1))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
void ReturnFromSubquestAndCleanUp(u8 taskId)
{
	PrepareFadeOut(taskId);

	PlaySE(SE_SELECT);
	sStateDataPtr->filterMode = ManageMode(SUB);
	gTasks[taskId].func = Task_FadeOut;
}

static void SetGpuRegBaseForFade()
{
	//Sets the GPU registers to prepare for a hardware fade
	SetGpuReg(REG_OFFSET_BLDCNT,
	          BLDCNT_TGT1_OBJ | BLDCNT_TGT1_BG0 | BLDCNT_TGT2_BG1 |
	          BLDCNT_EFFECT_BLEND);      //Blend Sprites and BG0 into BG1
	SetGpuReg(REG_OFFSET_BLDY, 0);
}

#define MAX_FADE_INTENSITY 16
#define MIN_FADE_INTENSITY 0

void InitFadeVariables(u8 taskId, u8 blendWeight, u8 frameDelay,
                       u8 frameTimerBase, u8 delta)
{
	gTasks[taskId].data[1] = blendWeight;
	gTasks[taskId].data[2] = frameDelay;
	gTasks[taskId].data[3] = gTasks[taskId].data[frameTimerBase];
	gTasks[taskId].data[4] = delta;
}


static void PrepareFadeOut(u8 taskId)
{
	SetGpuRegBaseForFade();
	SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(MAX_FADE_INTENSITY, 0));
	InitFadeVariables(taskId, MAX_FADE_INTENSITY, 0, 2, 2);
}

static bool8 HandleFadeOut(u8 taskId)
{
	if (gTasks[taskId].data[3]-- != 0)
	{
		return FALSE;
	}

	//Set the timer, decrease the fade weight by the delta, increase the delta by the timer
	gTasks[taskId].data[3] = gTasks[taskId].data[2];
	gTasks[taskId].data[1] -= gTasks[taskId].data[4];
	gTasks[taskId].data[2] += gTasks[taskId].data[3];

	//When blend weight runs out, set final blend and quit
	if (gTasks[taskId].data[1] <= 0)
	{
		SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(0, gTasks[taskId].data[1]));
		return TRUE;
	}
	//Set intermediate blend state
	SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(gTasks[taskId].data[1],
	            MAX_FADE_INTENSITY - gTasks[taskId].data[1]));
	return FALSE;
}

static void PrepareFadeIn(u8 taskId)
{
	SetGpuRegBaseForFade();
	SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(0,
	            MAX_FADE_INTENSITY));
	InitFadeVariables(taskId, MIN_FADE_INTENSITY, 0, 1, 2);
}

static bool8 HandleFadeIn(u8 taskId)
{
	//Set the timer, ncrease the fade weight by the delta,
	gTasks[taskId].data[3] = gTasks[taskId].data[2];
	gTasks[taskId].data[1] += gTasks[taskId].data[4];

	//When blend weight reaches max, set final blend and quit
	if (gTasks[taskId].data[1] >= MAX_FADE_INTENSITY)
	{
		SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(MAX_FADE_INTENSITY,
		            MIN_FADE_INTENSITY));
		return TRUE;
	}
	//Set intermediate blend state
	SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(gTasks[taskId].data[1],
	            MAX_FADE_INTENSITY - gTasks[taskId].data[1]));
	return FALSE;
}

static void Task_FadeOut(u8 taskId)
{
	if (HandleFadeOut(taskId))
	{
		PrepareFadeIn(taskId);
		Task_QuestMenuCleanUp(taskId);
		gTasks[taskId].func = Task_FadeIn;
	}
}

static void Task_FadeIn(u8 taskId)
{
	if (HandleFadeIn(taskId))
	{
		gTasks[taskId].func = Task_Main;
	}
}

static void Task_QuestMenuWaitFadeAndBail(u8 taskId)
{
	if (!gPaletteFade.active)
	{
		SetMainCallback2(sListMenuState.savedCallback);
		FreeResources();
		DestroyTask(taskId);
	}
}

static void FadeAndBail(void)
{
	BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
	CreateTask(Task_QuestMenuWaitFadeAndBail, 0);
	SetVBlankCallback(VBlankCB);
	SetMainCallback2(MainCB);
}


#define try_free(ptr) ({        \
		void ** ptr__ = (void **)&(ptr);   \
		if (*ptr__ != NULL)                \
			Free(*ptr__);                  \
	})

static void FreeResources(void)
{
	int i;

	try_free(sStateDataPtr);
	try_free(sBg1TilemapBuffer);
	try_free(sListMenuItems);

	for (i = QUEST_ARRAY_COUNT; i > -1; i--)
	{
		try_free(questNameArray[i]);
	}

	try_free(questNameArray);
	FreeAllWindowBuffers();
}

void TurnOffQuestMenu(u8 taskId)
{
	SetInitializedFlag(0);
	gTasks[taskId].func = Task_QuestMenuTurnOff1;
}
static void Task_QuestMenuTurnOff1(u8 taskId)
{
	BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
	gTasks[taskId].func = Task_QuestMenuTurnOff2;
}

static void Task_QuestMenuTurnOff2(u8 taskId)
{
	s16 *data = gTasks[taskId].data;

	if (!gPaletteFade.active)
	{
		DestroyListMenuTask(data[0], &sListMenuState.scroll, &sListMenuState.row);
		if (sStateDataPtr->savedCallback != NULL)
		{
			SetMainCallback2(sStateDataPtr->savedCallback);
		}
		else
		{
			SetMainCallback2(sListMenuState.savedCallback);
		}

		QuestMenu_RemoveScrollIndicatorArrowPair();
		FreeResources();
		DestroyTask(taskId);
	}
}

void Task_QuestMenu_OpenFromStartMenu(u8 taskId)
{
	s16 *data = gTasks[taskId].data;
	if (!gPaletteFade.active)
	{
		CleanupOverworldWindowsAndTilemaps();
		QuestMenu_Init(tItemPcParam, CB2_ReturnToFieldWithOpenMenu);
		DestroyTask(taskId);
	}
}

void QuestMenu_CopyQuestName(u8 *dst, u8 questId)
{
	StringCopy(dst, sSideQuests[questId].name);
}

void QuestMenu_CopySubquestName(u8 *dst, u8 parentId, u8 childId)
{
	StringCopy(dst, sSideQuests[parentId].subquests[childId].name);
}

void QuestMenu_ResetMenuSaveData(void)
{
	memset(&gSaveBlock2Ptr->questData, 0,
	       sizeof(gSaveBlock2Ptr->questData));
	memset(&gSaveBlock2Ptr->subQuests, 0,
	       sizeof(gSaveBlock2Ptr->subQuests));
}
