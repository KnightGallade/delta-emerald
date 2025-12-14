#include "global.h"
#include "pwt.h"
#include "battle.h"
#include "battle_ai_util.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "battle_message.h"
#include "event_data.h"
#include "overworld.h"
#include "util.h"
#include "malloc.h"
#include "string_util.h"
#include "random.h"
#include "task.h"
#include "main.h"
#include "gpu_regs.h"
#include "text.h"
#include "bg.h"
#include "window.h"
#include "strings.h"
#include "palette.h"
#include "decompress.h"
#include "party_menu.h"
#include "menu.h"
#include "sound.h"
#include "pokemon_icon.h"
#include "data.h"
#include "item.h"
#include "international_string_util.h"
#include "trainer_pokemon_sprites.h"
#include "scanline_effect.h"
#include "script_pokemon_util.h"
#include "graphics.h"
#include "outfit_menu.h"
#include "constants/pwt.h"
#include "constants/battle_move_effects.h"
#include "constants/moves.h"
#include "constants/trainers.h"
#include "constants/abilities.h"
#include "constants/songs.h"
#include "constants/rgb.h"

#include "field_message_box.h"
#include "constants/event_objects.h"
#include "constants/pwt_trainers.h"
#include "constants/pwt_mons.h"
#include "data/pwt/pwt_trainers.h"
#include "data/pwt/pwt_mons.h"

#define TAG_BUTTONS 0

// Enough space to hold 2 match info cards worth of trainers and their parties
#define NUM_PWT_INFOCARD_SPRITES ((PARTY_SIZE + 1) * 4)
#define NUM_PWT_INFOCARD_TRAINERS 2

// An 'Info Card' is a trainer or match information page that can be viewed on the Tourney Tree
struct PWTTourneyTreeInfoCard
{
    u8 spriteIds[NUM_PWT_INFOCARD_SPRITES];
    u8 pos;
    u8 tournamentIds[NUM_PWT_INFOCARD_TRAINERS];
};

struct PWTTourneyTreeLineSection
{
    u8 x;
    u8 y;
    u16 tile;
};

#define PWT_TRAINERS gSaveBlock2Ptr->pwt.pwtTrainers
#define PWT_MONS     gSaveBlock2Ptr->pwt.pwtMonIds

#define tState              data[0]

// Task data for Task_ShowPWTTourneyTree
#define tNotInteractive     data[1]
#define tIsPrevTourneyTree  data[4]

// Task data for Task_ShowPWTTourneyInfoCard
#define tTournamentId       data[1]
#define tMode               data[2]
#define tPrevTaskId         data[3]

// Window IDs for the tourney tree
enum {
    PWT_TOURNEYWIN_NAMES_LEFT,
    PWT_TOURNEYWIN_NAMES_RIGHT,
    PWT_TOURNEYWIN_TITLE,
};

// Window IDs for the trainer (WIN_TRAINER_*) and match (WIN_MATCH_*) info cards.
// All 9 have a duplicate window at WIN + NUM_PWT_INFO_CARD_WINDOWS used by the alternate info card
enum {
    WIN_PWT_TRAINER_NAME = 0,
    WIN_PWT_TRAINER_FLAVOR_TEXT,
    WIN_PWT_MATCH_NUMBER,
    WIN_PWT_MATCH_TRAINER_NAME_LEFT,
    WIN_PWT_MATCH_TRAINER_NAME_RIGHT,
    WIN_PWT_MATCH_WIN_TEXT,
    NUM_PWT_INFO_CARD_WINDOWS
};

// The different pools of Pokémon for each trainer
enum {
    PWT_TRAINER_POOL_REGULAR = 0,
    PWT_TRAINER_POOL_MEGA,
    PWT_TRAINER_POOL_LEGEND,
};

// Define all functions
static void CreatePWTOpponentMons(u16);
static void Task_ShowPWTTourneyInfoCard(u8);
static void Task_HandlePWTInfoCardInput(u8);
static u8 Task_GetInfoCardInput(u8);
static void SetPWTTrainerAndMonPtrs(void);
static int TrainerIdToPWTTournamentId(u16);
static u16 TrainerIdOfPWTPlayerOpponent(void);
static void Task_ShowPWTTourneyTree(u8);
static void Task_HandleStaticPWTTourneyTreeInput(u8);
static void CB2_PWTTourneyTree(void);
static void VblankCb_PWTTourneyInfoCard(void);
static void DisplayPWTMatchInfoOnCard(u8, u8);
static void DisplayPWTTrainerInfoOnCard(u8, u8);
static int BufferPWTWinString(u8, u8 *);
static void HblankCb_PWTTourneyTree(void);
static void VblankCb_PWTTourneyTree(void);
static u8 UpdatePWTTourneyTreeCursor(u8);
// static void DecidePWTRoundWinners(u8);
// static u8 GetOpposingNPCPWTTournamentIdByRound(u8, u8);
static void DrawPWTTourneyAdvancementLine(u8, u8);
static void SpriteCB_PWTHorizontalScrollArrow(struct Sprite *);
static void SpriteCB_PWTVerticalScrollArrow(struct Sprite *);
static void PWTInit(void);
static void GetPWTData(void);
static void SetPWTData(void);
static void BufferPWTRoundText(void);
static void BufferNextPWTBattleStrings(void);
static void SetPWTOpponentParty(void);
static void ShowPWTOpponentInfo(void);
static void ShowPWTTourneyTree(void);
static void ShowPreviousPWTTourneyTree(void);
static void SetPWTOpponentId(void);
static void SetPWTOpponentGFX(void);
static void ShowNonInteractivePWTTourneyTree(void);
// static void ResolvePWTRoundWinners(void);
static void SetPWTPlayerParty(void);
// static void BufferLastPWTWinnerName(void);
// static void InitRandomPWTTourneyTreeResults(void);
static void PWTInitTrainers(void);
// NEW FUNCTIONS:
static void CheckPWTIneligibility(void);
static void GetPWTOpponentIntro(void);
u16 GetRandomPWTTrainerId(u8);
u16 GetRandomPWTMonFromSet(u16, u8);
u8 GetPWTMonBall(u8);


static EWRAM_DATA struct PWTTourneyTreeInfoCard *sPWTInfoCard = {0};
static EWRAM_DATA u8 *sTilemapBuffer = NULL;
EWRAM_DATA const struct PWTTrainerData *gPWTFacilityTrainers = NULL;
EWRAM_DATA const struct TrainerMon *gPWTFacilityTrainerMons = NULL;

const u8 *const gPWTRoundsStringTable[PWT_ROUNDS_COUNT] =
{
    [PWT_ROUND1]    = COMPOUND_STRING("Round 1"),
    [PWT_ROUND2]    = COMPOUND_STRING("Round 2"),
    [PWT_SEMIFINAL] = COMPOUND_STRING("Semifinal"),
    [PWT_FINAL]     = COMPOUND_STRING("Final"),
};

// 1st array is for cursor position (sprite id): cursor can be on a trainer info button, a match info button, or the exit/cancel button
// 2nd array is for round count. For some reason this array contains an inaccessible Round 5 which is identical to Round 4
// 3rd array is movement direction (see the MOVE_DIR_* constants in UpdatePWTTourneyTreeCursor)
// The values are sprite IDs for the cursor position to move to, with 0xFF being an invalid move
static const u8 sPWTTourneyTreeCursorMovementMap[PWT_TOURNAMENT_SIZE + PWT_MATCHES_COUNT + 1][PWT_ROUNDS_COUNT + 1][4]=
{
    [0]  = {{   7,    1,    8,   16}, {   7,    1,    8,   16}, {   7,    1,    8,   16}, {   7,    1,    8,   16}, {   7,    1,    8,   16}},
    [1]  = {{   0,    2,    9,   16}, {   0,    2,    9,   16}, {   0,    2,    9,   16}, {   0,    2,    9,   16}, {   0,    2,    9,   16}},
    [2]  = {{   1,    3,   10,   17}, {   1,    3,   10,   17}, {   1,    3,   10,   17}, {   1,    3,   10,   17}, {   1,    3,   10,   17}},
    [3]  = {{   2,    4,   11,   17}, {   2,    4,   11,   17}, {   2,    4,   11,   17}, {   2,    4,   11,   17}, {   2,    4,   11,   17}},
    [4]  = {{   3,    5,   12,   18}, {   3,    5,   12,   18}, {   3,    5,   12,   18}, {   3,    5,   12,   18}, {   3,    5,   12,   18}},
    [5]  = {{   4,    6,   13,   18}, {   4,    6,   13,   18}, {   4,    6,   13,   18}, {   4,    6,   13,   18}, {   4,    6,   13,   18}},
    [6]  = {{   5,    7,   14,   19}, {   5,    7,   14,   19}, {   5,    7,   14,   19}, {   5,    7,   14,   19}, {   5,    7,   14,   19}},
    [7]  = {{   6,    0,   15,   19}, {   6,    0,   15,   19}, {   6,    0,   15,   19}, {   6,    0,   15,   19}, {   6,    0,   15,   19}},
    [8]  = {{  31,    9,   20,   31}, {  31,    9,   20,   31}, {  31,    9,   20,   31}, {  31,    9,   20,   31}, {  31,    9,   20,   31}},
    [9]  = {{   8,   10,   20,    1}, {   8,   10,   20,    1}, {   8,   10,   20,    1}, {   8,   10,   20,    1}, {   8,   10,   20,    1}},
    [10] = {{   9,   11,   21,    2}, {   9,   11,   21,    2}, {   9,   11,   21,    2}, {   9,   11,   21,    2}, {   9,   11,   21,    2}},
    [11] = {{  10,   12,   21,    3}, {  10,   12,   21,    3}, {  10,   12,   21,    3}, {  10,   12,   21,    3}, {  10,   12,   21,    3}},
    [12] = {{  11,   13,   22,    4}, {  11,   13,   22,    4}, {  11,   13,   22,    4}, {  11,   13,   22,    4}, {  11,   13,   22,    4}},
    [13] = {{  12,   14,   22,    5}, {  12,   14,   22,    5}, {  12,   14,   22,    5}, {  12,   14,   22,    5}, {  12,   14,   22,    5}},
    [14] = {{  13,   15,   23,    6}, {  13,   15,   23,    6}, {  13,   15,   23,    6}, {  13,   15,   23,    6}, {  13,   15,   23,    6}},
    [15] = {{  14,   31,   23,    7}, {  14,   31,   23,    7}, {  14,   31,   23,    7}, {  14,   31,   23,    7}, {  14,   31,   23,    7}},
    [16] = {{  19,   17,    0,   20}, {  19,   17,    0,   24}, {  19,   17,    0,   24}, {  19,   17,    0,   24}, {  19,   17,    0,   24}},
    [17] = {{  16,   18,    2,   21}, {  16,   18,    2,   24}, {  16,   18,    2,   24}, {  16,   18,    2,   24}, {  16,   18,    2,   24}},
    [18] = {{  17,   19,    4,   22}, {  17,   19,    4,   25}, {  17,   19,    4,   25}, {  17,   19,    4,   25}, {  17,   19,    4,   25}},
    [19] = {{  18,   16,    6,   23}, {  18,   16,    6,   25}, {  18,   16,    6,   25}, {  18,   16,    6,   25}, {  18,   16,    6,   25}},
    [20] = {{  23,   21,   16,    8}, {  23,   21,   26,    8}, {  23,   21,   26,    8}, {  23,   21,   26,    8}, {  23,   21,   26,    8}},
    [21] = {{  20,   22,   17,   10}, {  20,   22,   26,   10}, {  20,   22,   26,   10}, {  20,   22,   26,   10}, {  20,   22,   26,   10}},
    [22] = {{  21,   23,   18,   12}, {  21,   23,   27,   12}, {  21,   23,   27,   12}, {  21,   23,   27,   12}, {  21,   23,   27,   12}},
    [23] = {{  22,   20,   19,   14}, {  22,   20,   27,   14}, {  22,   20,   27,   14}, {  22,   20,   27,   14}, {  22,   20,   27,   14}},
    [24] = {{0xFF, 0xFF, 0xFF, 0xFF}, {  25,   25,   16,   26}, {  25,   25,   16,   28}, {  25,   25,   16,   28}, {  25,   25,   16,   28}},
    [25] = {{0xFF, 0xFF, 0xFF, 0xFF}, {  24,   24,   18,   27}, {  24,   24,   18,   28}, {  24,   24,   18,   28}, {  24,   24,   18,   28}},
    [26] = {{0xFF, 0xFF, 0xFF, 0xFF}, {  27,   27,   24,   20}, {  27,   27,   29,   20}, {  27,   27,   29,   20}, {  27,   27,   29,   20}},
    [27] = {{0xFF, 0xFF, 0xFF, 0xFF}, {  26,   26,   25,   22}, {  26,   26,   29,   22}, {  26,   26,   29,   22}, {  26,   26,   29,   22}},
    [28] = {{0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF,   24,   29}, {0xFF, 0xFF,   24,   30}, {0xFF, 0xFF,   24,   30}},
    [29] = {{0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF,   28,   26}, {0xFF, 0xFF,   30,   26}, {0xFF, 0xFF,   30,   26}},
    [30] = {{0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF,   28,   29}, {0xFF, 0xFF,   28,   29}},
    [31] = {{  15,    8,    8,    0}, {  15,    8,    8,    0}, {  15,    8,    8,    0}, {  15,    8,    8,    0}, {  15,    8,    8,    0}}, // PWT_TOURNEY_TREE_CLOSE_BUTTON
};

static const struct BgTemplate sPWTTourneyTreeBgTemplates[4] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    },
    {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    },
    {
        .bg = 3,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    },
};

static const struct BgTemplate sPWTInfoCardBgTemplates[4] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 20,
        .screenSize = 3,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 24,
        .screenSize = 3,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 28,
        .screenSize = 3,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    },
    {
        .bg = 3,
        .charBaseIndex = 2,
        .mapBaseIndex = 7,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    },
};

static const struct WindowTemplate sPWTTourneyTreeWindowTemplates[] =
{
    [PWT_TOURNEYWIN_NAMES_LEFT] = {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 3,
        .width = 8,
        .height = 16,
        .paletteNum = 15,
        .baseBlock = 16,
    },
    [PWT_TOURNEYWIN_NAMES_RIGHT] = {
        .bg = 0,
        .tilemapLeft = 22,
        .tilemapTop = 3,
        .width = 8,
        .height = 16,
        .paletteNum = 15,
        .baseBlock = 144,
    },
    [PWT_TOURNEYWIN_TITLE] = {
        .bg = 0,
        .tilemapLeft = 8,
        .tilemapTop = 1,
        .width = 14,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 272,
    },
    DUMMY_WIN_TEMPLATE,
};

static const struct WindowTemplate sPWTInfoCardWindowTemplates[] =
{
    [WIN_PWT_TRAINER_NAME] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 2,
        .width = 26,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    [WIN_PWT_TRAINER_FLAVOR_TEXT] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 12,
        .width = 26,
        .height = 7,
        .paletteNum = 15,
        .baseBlock = 112,
    },
    [WIN_PWT_MATCH_NUMBER] = {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 2,
        .width = 23,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 294,
    },
    [WIN_PWT_MATCH_TRAINER_NAME_LEFT] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 5,
        .width = 8,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 340,
    },
    [WIN_PWT_MATCH_TRAINER_NAME_RIGHT] = {
        .bg = 0,
        .tilemapLeft = 20,
        .tilemapTop = 5,
        .width = 8,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 356,
    },
    [WIN_PWT_MATCH_WIN_TEXT] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 16,
        .width = 26,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 372,
    },
    // Duplicate windows used by the alternate info card
    // Same as above but on bg 1 instead of bg 0
    [WIN_PWT_TRAINER_NAME + NUM_PWT_INFO_CARD_WINDOWS] = {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 2,
        .width = 26,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    [WIN_PWT_TRAINER_FLAVOR_TEXT + NUM_PWT_INFO_CARD_WINDOWS] = {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 12,
        .width = 26,
        .height = 7,
        .paletteNum = 15,
        .baseBlock = 112,
    },
    [WIN_PWT_MATCH_NUMBER + NUM_PWT_INFO_CARD_WINDOWS] = {
        .bg = 1,
        .tilemapLeft = 5,
        .tilemapTop = 2,
        .width = 23,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 294,
    },
    [WIN_PWT_MATCH_TRAINER_NAME_LEFT + NUM_PWT_INFO_CARD_WINDOWS] = {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 5,
        .width = 8,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 340,
    },
    [WIN_PWT_MATCH_TRAINER_NAME_RIGHT + NUM_PWT_INFO_CARD_WINDOWS] = {
        .bg = 1,
        .tilemapLeft = 20,
        .tilemapTop = 5,
        .width = 8,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 356,
    },
    [WIN_PWT_MATCH_WIN_TEXT + NUM_PWT_INFO_CARD_WINDOWS] = {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 16,
        .width = 26,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 372,
    },
    #ifdef UBFIX
    DUMMY_WIN_TEMPLATE,
    #endif
};

static const struct ScanlineEffectParams sPWTTourneyTreeScanlineEffectParams =
{
    .dmaDest = &REG_BG3CNT,
    .dmaControl = SCANLINE_EFFECT_DMACNT_16BIT,
    .initState = 1,
};

static const struct CompressedSpriteSheet sPWTTourneyTreeButtonsSpriteSheet[] =
{
    {.data = gPWTTourneyTreeButtons_Gfx, .size = 0x0600, .tag = TAG_BUTTONS},
    {},
};

static const struct OamData sOamData_PWTTourneyTreePokeball =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

// For Exit/Cancel buttons
static const struct OamData sOamData_PWTTourneyTreeCloseButton =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x16),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 1,
    .affineParam = 0,
};

static const struct OamData sOamData_PWTVerticalScrollArrow =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x8),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 2,
    .affineParam = 0,
};

static const struct OamData sOamData_PWTHorizontalScrollArrow =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x16),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 2,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_PWTTourneyTreePokeballNormal[] =
{
    ANIMCMD_FRAME(20, 1),
    ANIMCMD_END,
};
static const union AnimCmd sSpriteAnim_PWTTourneyTreePokeballSelected[] =
{
    ANIMCMD_FRAME(24, 1),
    ANIMCMD_END,
};

static const union AnimCmd *const sSpriteAnimTable_PWTTourneyTreePokeball[] =
{
    sSpriteAnim_PWTTourneyTreePokeballNormal,
    sSpriteAnim_PWTTourneyTreePokeballSelected,
};

// Sprite template for the pokeballs on the tourney tree that act as buttons to view a trainer/match info card
static const struct SpriteTemplate sPWTTourneyTreePokeballSpriteTemplate =
{
    .tileTag = TAG_BUTTONS,
    .paletteTag = TAG_NONE,
    .oam = &sOamData_PWTTourneyTreePokeball,
    .anims = sSpriteAnimTable_PWTTourneyTreePokeball,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const union AnimCmd sSpriteAnim_PWTTourneyTreeCancelButtonNormal[] =
{
    ANIMCMD_FRAME(8, 1),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_PWTTourneyTreeCancelButtonSelected[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd *const sSpriteAnimTable_PWTTourneyTreeCancelButton[] =
{
    sSpriteAnim_PWTTourneyTreeCancelButtonNormal,
    sSpriteAnim_PWTTourneyTreeCancelButtonSelected,
};

static const struct SpriteTemplate sPWTCancelButtonSpriteTemplate =
{
    .tileTag = TAG_BUTTONS,
    .paletteTag = TAG_NONE,
    .oam = &sOamData_PWTTourneyTreeCloseButton,
    .anims = sSpriteAnimTable_PWTTourneyTreeCancelButton,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const union AnimCmd sSpriteAnim_TourneyTreeExitButtonNormal[] =
{
    ANIMCMD_FRAME(40, 1),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_TourneyTreeExitButtonSelected[] =
{
    ANIMCMD_FRAME(32, 1),
    ANIMCMD_END,
 };

static const union AnimCmd *const sSpriteAnimTable_PWTTourneyTreeExitButton[] =
{
    sSpriteAnim_TourneyTreeExitButtonNormal,
    sSpriteAnim_TourneyTreeExitButtonSelected,
};

static const struct SpriteTemplate sPWTExitButtonSpriteTemplate =
{
    .tileTag = TAG_BUTTONS,
    .paletteTag = TAG_NONE,
    .oam = &sOamData_PWTTourneyTreeCloseButton,
    .anims = sSpriteAnimTable_PWTTourneyTreeExitButton,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const union AnimCmd sSpriteAnim_PWTUpArrow[] =
{
    ANIMCMD_FRAME(18, 1),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_PWTDownArrow[] =
{
    ANIMCMD_FRAME(18, 1, .vFlip = TRUE),
    ANIMCMD_END,
 };

static const union AnimCmd sSpriteAnim_PWTLeftArrow[] =
{
    ANIMCMD_FRAME(16, 1, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_PWTRightArrow[] =
{
    ANIMCMD_FRAME(16, 1),
    ANIMCMD_END,
};

static const union AnimCmd *const sSpriteAnimTable_PWTVerticalScrollArrow[] =
{
    sSpriteAnim_PWTUpArrow,
    sSpriteAnim_PWTDownArrow,
};

static const union AnimCmd *const sSpriteAnimTable_PWTHorizontalScrollArrow[] =
{
    sSpriteAnim_PWTLeftArrow,
    sSpriteAnim_PWTRightArrow,
};

static const struct SpriteTemplate sPWTHorizontalScrollArrowSpriteTemplate =
{
    .tileTag = TAG_BUTTONS,
    .paletteTag = TAG_NONE,
    .oam = &sOamData_PWTHorizontalScrollArrow,
    .anims = sSpriteAnimTable_PWTHorizontalScrollArrow,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_PWTHorizontalScrollArrow
};

static const struct SpriteTemplate sPWTVerticalScrollArrowSpriteTemplate =
{
    .tileTag = TAG_BUTTONS,
    .paletteTag = TAG_NONE,
    .oam = &sOamData_PWTVerticalScrollArrow,
    .anims = sSpriteAnimTable_PWTVerticalScrollArrow,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_PWTVerticalScrollArrow
};

static const u8 sPWTTourneyTreeTrainerIds[PWT_TOURNAMENT_SIZE] = {0, 8, 12, 4, 7, 15, 11, 3, 2, 10, 14, 6, 5, 13, 9, 1};

static void (*const sPWTFunctions[])(void) =
{
    [PWT_FUNC_CHECK_INELIGIBLE]                 = CheckPWTIneligibility,
    [PWT_FUNC_SET_DATA]                         = SetPWTData,
    [PWT_FUNC_GET_DATA]                         = GetPWTData,
    [PWT_FUNC_INIT]                             = PWTInit,
    [PWT_FUNC_INIT_TRAINERS]                    = PWTInitTrainers,
    [PWT_FUNC_SET_TRAINERS]                     = SetPWTTrainerAndMonPtrs,
    [PWT_FUNC_SHOW_OPPONENT_CARD]               = ShowPWTOpponentInfo,
    [PWT_FUNC_SHOW_TOURNEY_TREE]                = ShowPWTTourneyTree,
    [PWT_FUNC_SET_PLAYER_PARTY]                 = SetPWTPlayerParty,
    [PWT_FUNC_SET_OPPONENT_ID]                  = SetPWTOpponentId,
    [PWT_FUNC_SET_OPPONENT_GFX]                 = SetPWTOpponentGFX,
    [PWT_FUNC_BUFFER_BATTLE_STRINGS]            = BufferNextPWTBattleStrings,
    [PWT_FUNC_SHOW_OPPONENT_INTRO]              = GetPWTOpponentIntro,
    [PWT_FUNC_SET_OPPONENT_PARTY]               = SetPWTOpponentParty,
};

// TODO: The below two arrays probably need better names. The one below for example is only true of sIdToPWTOpponentId[i][0]
static const u8 sIdToPWTOpponentId[PWT_TOURNAMENT_SIZE][PWT_ROUNDS_COUNT] =
{
    [0]  = { 8,  0,  4,  8},
    [1]  = { 9, 12,  8,  0},
    [2]  = {10,  8, 12,  0},
    [3]  = {11,  4,  0,  8},
    [4]  = {12,  0,  4,  8},
    [5]  = {13, 12,  8,  0},
    [6]  = {14,  8, 12,  0},
    [7]  = {15,  4,  0,  8},
    [8]  = { 0,  0,  4,  8},
    [9]  = { 1, 12,  8,  0},
    [10] = { 2,  8, 12,  0},
    [11] = { 3,  4,  0,  8},
    [12] = { 4,  0,  4,  8},
    [13] = { 5, 12,  8,  0},
    [14] = { 6,  8, 12,  0},
    [15] = { 7,  4,  0,  8},
};

// sPWTTourneyTreeTrainerIds with every other pair swapped
static const u8 sPWTTourneyTreeTrainerOpponentIds[PWT_TOURNAMENT_SIZE] = { 0, 8, 4, 12, 7, 15, 3, 11, 2, 10, 6, 14, 5, 13, 1, 9 };

// The match number - 1 that a given tournament trainer will participate in for a given round
static const u8 sIdToPWTMatchNumber[PWT_TOURNAMENT_SIZE][PWT_ROUNDS_COUNT] =
{
    { 0,  8, 12, 14},
    { 0,  8, 12, 14},
    { 1,  8, 12, 14},
    { 1,  8, 12, 14},
    { 2,  9, 12, 14},
    { 2,  9, 12, 14},
    { 3,  9, 12, 14},
    { 3,  9, 12, 14},
    { 4, 10, 13, 14},
    { 4, 10, 13, 14},
    { 5, 10, 13, 14},
    { 5, 10, 13, 14},
    { 6, 11, 13, 14},
    { 6, 11, 13, 14},
    { 7, 11, 13, 14},
    { 7, 11, 13, 14},
};

static const u8 sLastPWTMatchCardNum[PWT_ROUNDS_COUNT] =
{
    [PWT_ROUND1]    = 23,
    [PWT_ROUND2]    = 27,
    [PWT_SEMIFINAL] = 29,
    [PWT_FINAL]     = 30
};

static const u8 sTrainerAndRoundToLastPWTMatchCardNum[PWT_TOURNAMENT_SIZE / 2][PWT_ROUNDS_COUNT] =
{
    {16, 24, 28, 30},
    {17, 24, 28, 30},
    {18, 25, 28, 30},
    {19, 25, 28, 30},
    {20, 26, 29, 30},
    {21, 26, 29, 30},
    {22, 27, 29, 30},
    {23, 27, 29, 30},
};

static const u8 sPWTTournamentIdToPairedTrainerIds[PWT_TOURNAMENT_SIZE] = {0, 15, 8, 7, 3, 12, 11, 4, 1, 14, 9, 6, 2, 13, 10, 5};

static const u8 sInfoPWTTrainerMonX[PARTY_SIZE] = {104, 136, 104, 168, 200, 168};
static const u8 sInfoPWTTrainerMonY[PARTY_SIZE] = { 38,  62,  78,  38,  62,  78};

static const u8 *const sPWTMatchNumberTexts[PWT_MATCHES_COUNT] =
{
    COMPOUND_STRING("Round 1, Match 1$"),
    COMPOUND_STRING("Round 1, Match 2$"),
    COMPOUND_STRING("Round 1, Match 3$"),
    COMPOUND_STRING("Round 1, Match 4$"),
    COMPOUND_STRING("Round 1, Match 5$"),
    COMPOUND_STRING("Round 1, Match 6$"),
    COMPOUND_STRING("Round 1, Match 7$"),
    COMPOUND_STRING("Round 1, Match 8$"),
    COMPOUND_STRING("Round 2, Match 1$"),
    COMPOUND_STRING("Round 2, Match 2$"),
    COMPOUND_STRING("Round 2, Match 3$"),
    COMPOUND_STRING("Round 2, Match 4$"),
    COMPOUND_STRING("Semifinal Match 1$"),
    COMPOUND_STRING("Semifinal Match 2$"),
    COMPOUND_STRING("Final Match$"),
};

// TODO - figure out how I want to handle these
static const u8 *const sBattlePWTWinTexts[] =
{
    [PWT_TEXT_NO_WINNER_YET]    = COMPOUND_STRING("Let the battle begin!$"),
    [PWT_TEXT_WON_USING_MOVE]   = COMPOUND_STRING("{STR_VAR_1} won using {STR_VAR_2}!$"),
    [PWT_TEXT_CHAMP_USING_MOVE] = COMPOUND_STRING("{STR_VAR_1} became the champ!$"),
    [PWT_TEXT_WON_ON_FORFEIT]   = COMPOUND_STRING("{STR_VAR_1} won by default!$"),
    [PWT_TEXT_CHAMP_ON_FORFEIT] = COMPOUND_STRING("{STR_VAR_1} won outright by default!$"),
    [PWT_TEXT_WON_NO_MOVES]     = COMPOUND_STRING("{STR_VAR_1} won without using a move!$"),
    [PWT_TEXT_CHAMP_NO_MOVES]   = COMPOUND_STRING("{STR_VAR_1} won outright with no moves!$"),
};

static const u8 sLeftPWTTrainerMonX[PARTY_SIZE]  = { 86,  86,  86, 106, 106, 106};
static const u8 sLeftPWTTrainerMonY[PARTY_SIZE]  = { 56,  80, 104,  56,  80, 104};
static const u8 sRightPWTTrainerMonX[PARTY_SIZE] = {134, 134, 134, 154, 154, 154};
static const u8 sRightPWTTrainerMonY[PARTY_SIZE] = { 56,  80, 104,  56,  80, 104};

// Duplicate of sPWTTourneyTreeTrainerIds
// TODO - see if I can remove this to avoid a useless repeat
static const u8 sPWTTourneyTreeTrainerIds2[PWT_TOURNAMENT_SIZE] = {0, 8, 12, 4, 7, 15, 11, 3, 2, 10, 14, 6, 5, 13, 9, 1};

// The number of possible trainers that could be competing in a given match
#define NUM_POSSIBLE_PWT_MATCH_TRAINERS(round) (PWT_TOURNAMENT_SIZE / (1 << (PWT_ROUNDS_COUNT - round - 1)))

// The range of tournament trainers to check as possible participants in a given match
// Given by the offset in sPWTCompetitorRangeByMatch[][0], the number of trainers in sPWTCompetitorRangeByMatch[][1], and the round
static const u8 sPWTCompetitorRangeByMatch[PWT_MATCHES_COUNT][3] =
{
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1) * 0,    NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1),    PWT_ROUND1},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1) * 1,    NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1),    PWT_ROUND1},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1) * 2,    NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1),    PWT_ROUND1},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1) * 3,    NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1),    PWT_ROUND1},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1) * 4,    NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1),    PWT_ROUND1},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1) * 5,    NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1),    PWT_ROUND1},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1) * 6,    NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1),    PWT_ROUND1},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1) * 7,    NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND1),    PWT_ROUND1},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND2) * 0,    NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND2),    PWT_ROUND2},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND2) * 1,    NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND2),    PWT_ROUND2},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND2) * 2,    NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND2),    PWT_ROUND2},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND2) * 3,    NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_ROUND2),    PWT_ROUND2},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_SEMIFINAL) * 0, NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_SEMIFINAL), PWT_SEMIFINAL},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_SEMIFINAL) * 1, NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_SEMIFINAL), PWT_SEMIFINAL},
    { NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_FINAL) * 0,     NUM_POSSIBLE_PWT_MATCH_TRAINERS(PWT_FINAL),     PWT_FINAL},
};

#define NAME_ROW_HEIGHT 16

// 1st value is the windowId, 2nd value is the y coord
static const u8 sPWTTrainerNamePositions[PWT_TOURNAMENT_SIZE][2] =
{
    { PWT_TOURNEYWIN_NAMES_LEFT,  0 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_RIGHT, 7 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_RIGHT, 0 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_LEFT,  7 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_LEFT,  3 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_RIGHT, 4 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_RIGHT, 3 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_LEFT,  4 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_LEFT,  1 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_RIGHT, 6 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_RIGHT, 1 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_LEFT,  6 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_LEFT,  2 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_RIGHT, 5 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_RIGHT, 2 * NAME_ROW_HEIGHT},
    { PWT_TOURNEYWIN_NAMES_LEFT,  5 * NAME_ROW_HEIGHT},
};

// Coords for the pokeballs on the tourney tree that act as buttons to view trainer/match info
static const u8 sPWTTourneyTreePokeballCoords[PWT_TOURNAMENT_SIZE + PWT_MATCHES_COUNT][2] =
{
    { 68,  33}, // Left side trainers
    { 68,  49},
    { 68,  65},
    { 68,  81},
    { 68,  97},
    { 68, 113},
    { 68, 129},
    { 68, 145},
    {172,  33}, // Right side trainers
    {172,  49},
    {172,  65},
    {172,  81},
    {172,  97},
    {172, 113},
    {172, 129},
    {172, 145},
    { 87,  41}, // Left side Round 1 matches
    { 87,  73},
    { 87, 105},
    { 87, 137},
    {153,  41}, // Right side Round 1 matches
    {153,  73},
    {153, 105},
    {153, 137},
    { 95,  57}, // Left side Round 2 matches
    { 95, 121},
    {145,  57}, // Right side Round 2 matches
    {145, 121},
    {103,  89}, // Left side semifinal match
    {137,  89}, // Right side semifinal match
    {120,  89}, // Final match
};

// Tile values from tourney_tree.png for the highlighted lines of the tourney tree.
// These tiles will be used to replace the existing, unhighlighted line tiles on the tourney tree tilemap.
#define LINE_PAL           (6 << 12)
#define LINE_H             (LINE_PAL | 0x21) // Horizontal
#define LINE_CORNER_R      (LINE_PAL | 0x23) // Horizontal into a right-side vertical
#define LINE_CORNER_L      (LINE_PAL | 0x25) // Horizontal into a left-side vertical
#define LINE_V_R           (LINE_PAL | 0x27) // Right-side vertical
#define LINE_V_L           (LINE_PAL | 0x29) // Left-side vertical
#define LINE_H_BOTTOM      (LINE_PAL | 0x2B) // Horizontal on the bottom of the tree
#define LINE_H_LOGO1       (LINE_PAL | 0x2C) // Horizontal, logo behind
#define LINE_H_LOGO2       (LINE_PAL | 0x2D) // Horizontal, logo behind
#define LINE_H_LOGO3       (LINE_PAL | 0x2E) // Horizontal, logo behind
#define LINE_H_LOGO4       (LINE_PAL | 0x2F) // Horizontal, logo behind
#define LINE_V_R_LOGO1     (LINE_PAL | 0x30) // Right-side vertical, logo behind
#define LINE_V_R_LOGO2     (LINE_PAL | 0x31) // Right-side vertical, logo behind
#define LINE_V_R_LOGO3     (LINE_PAL | 0x32) // Right-side vertical, logo behind
#define LINE_V_R_LOGO4     (LINE_PAL | 0x33) // Right-side vertical, logo behind
#define LINE_V_L_LOGO1     (LINE_PAL | 0x35) // Left-side vertical, logo behind
#define LINE_V_L_LOGO2     (LINE_PAL | 0x36) // Left-side vertical, logo behind
#define LINE_V_L_LOGO3     (LINE_PAL | 0x37) // Left-side vertical, logo behind
#define LINE_V_L_LOGO4     (LINE_PAL | 0x38) // Left-side vertical, logo behind
#define LINE_V_R_HALF_LOGO (LINE_PAL | 0x3B) // Right-side vertical, half lit from the top, logo behind
#define LINE_V_L_HALF_LOGO (LINE_PAL | 0x3C) // Left-side vertical, half lit from the top, logo behind
#define LINE_CORNER_R_HALF (LINE_PAL | 0x43) // Lit horizontal, unlit right-side vertical
#define LINE_CORNER_L_HALF (LINE_PAL | 0x45) // Lit horizontal, unlit left-side vertical
#define LINE_V_R_HALF      (LINE_PAL | 0x47) // Right-side vertical, half lit from the top
#define LINE_V_L_HALF      (LINE_PAL | 0x49) // Left-side vertical, half lit from the top

// Each of these line sections define the position of the advancement line on the tourney tree for the victor of that round
// The trainers here are numbered by tourney ID (rank/seed) and ordered according to where they start on the tourney tree
#define LINESECTION_PWT_ROUND1_TRAINER1(lastTile) \
    {.tile = LINE_H,        .y =  4, .x =  9}, \
    {.tile = LINE_CORNER_R, .y =  4, .x = 10}, \
    {.tile = LINE_V_R_HALF, .y =  5, .x = 10}, \
    {.tile = lastTile,      .y =  5, .x = 11},

#define LINESECTION_PWT_ROUND1_TRAINER9(lastTile) \
    {.tile = LINE_H,   .y =  6, .x =  9}, \
    {.tile = LINE_H,   .y =  6, .x = 10}, \
    {.tile = LINE_V_R, .y =  5, .x = 10}, \
    {.tile = lastTile, .y =  5, .x = 11},

#define LINESECTION_PWT_ROUND1_TRAINER13(lastTile) \
    {.tile = LINE_H,        .y =  8, .x =  9}, \
    {.tile = LINE_CORNER_R, .y =  8, .x = 10}, \
    {.tile = LINE_V_R_HALF, .y =  9, .x = 10}, \
    {.tile = lastTile,      .y =  9, .x = 11},

#define LINESECTION_PWT_ROUND1_TRAINER5(lastTile) \
    {.tile = LINE_H,   .y = 10, .x =  9}, \
    {.tile = LINE_H,   .y = 10, .x = 10}, \
    {.tile = LINE_V_R, .y =  9, .x = 10}, \
    {.tile = lastTile, .y =  9, .x = 11},

#define LINESECTION_PWT_ROUND1_TRAINER8(lastTile) \
    {.tile = LINE_H,        .y = 12, .x =  9}, \
    {.tile = LINE_CORNER_R, .y = 12, .x = 10}, \
    {.tile = LINE_V_R_HALF, .y = 13, .x = 10}, \
    {.tile = lastTile,      .y = 13, .x = 11},

#define LINESECTION_PWT_ROUND1_TRAINER16(lastTile) \
    {.tile = LINE_H,   .y = 14, .x =  9}, \
    {.tile = LINE_H,   .y = 14, .x = 10}, \
    {.tile = LINE_V_R, .y = 13, .x = 10}, \
    {.tile = lastTile, .y = 13, .x = 11},

#define LINESECTION_PWT_ROUND1_TRAINER12(lastTile) \
    {.tile = LINE_H,        .y = 16, .x =  9}, \
    {.tile = LINE_CORNER_R, .y = 16, .x = 10}, \
    {.tile = LINE_V_R_HALF, .y = 17, .x = 10}, \
    {.tile = lastTile,      .y = 17, .x = 11},

#define LINESECTION_PWT_ROUND1_TRAINER4(lastTile) \
    {.tile = LINE_H_BOTTOM, .y = 18, .x =  9}, \
    {.tile = LINE_H_BOTTOM, .y = 18, .x = 10}, \
    {.tile = LINE_V_R,      .y = 17, .x = 10}, \
    {.tile = lastTile,      .y = 17, .x = 11},

#define LINESECTION_PWT_ROUND1_TRAINER3(lastTile) \
    {.tile = LINE_H,        .y =  4, .x = 20}, \
    {.tile = LINE_CORNER_L, .y =  4, .x = 19}, \
    {.tile = LINE_V_L_HALF, .y =  5, .x = 19}, \
    {.tile = lastTile,      .y =  5, .x = 18},

#define LINESECTION_PWT_ROUND1_TRAINER11(lastTile) \
    {.tile = LINE_H,   .y =  6, .x = 20}, \
    {.tile = LINE_H,   .y =  6, .x = 19}, \
    {.tile = LINE_V_L, .y =  5, .x = 19}, \
    {.tile = lastTile, .y =  5, .x = 18},

#define LINESECTION_PWT_ROUND1_TRAINER15(lastTile) \
    {.tile = LINE_H,        .y =  8, .x = 20}, \
    {.tile = LINE_CORNER_L, .y =  8, .x = 19}, \
    {.tile = LINE_V_L_HALF, .y =  9, .x = 19}, \
    {.tile = lastTile,      .y =  9, .x = 18},

#define LINESECTION_PWT_ROUND1_TRAINER7(lastTile) \
    {.tile = LINE_H,   .y = 10, .x = 20}, \
    {.tile = LINE_H,   .y = 10, .x = 19}, \
    {.tile = LINE_V_L, .y =  9, .x = 19}, \
    {.tile = lastTile, .y =  9, .x = 18},

#define LINESECTION_PWT_ROUND1_TRAINER6(lastTile) \
    {.tile = LINE_H,        .y = 12, .x = 20}, \
    {.tile = LINE_CORNER_L, .y = 12, .x = 19}, \
    {.tile = LINE_V_L_HALF, .y = 13, .x = 19}, \
    {.tile = lastTile,      .y = 13, .x = 18},

#define LINESECTION_PWT_ROUND1_TRAINER14(lastTile) \
    {.tile = LINE_H,   .y = 14, .x = 20}, \
    {.tile = LINE_H,   .y = 14, .x = 19}, \
    {.tile = LINE_V_L, .y = 13, .x = 19}, \
    {.tile = lastTile, .y = 13, .x = 18},

#define LINESECTION_PWT_ROUND1_TRAINER10(lastTile) \
    {.tile = LINE_H,        .y = 16, .x = 20}, \
    {.tile = LINE_CORNER_L, .y = 16, .x = 19}, \
    {.tile = LINE_V_L_HALF, .y = 17, .x = 19}, \
    {.tile = lastTile,      .y = 17, .x = 18},

#define LINESECTION_PWT_ROUND1_TRAINER2(lastTile) \
    {.tile = LINE_H_BOTTOM, .y = 18, .x = 20}, \
    {.tile = LINE_H_BOTTOM, .y = 18, .x = 19}, \
    {.tile = LINE_V_L,      .y = 17, .x = 19}, \
    {.tile = lastTile,      .y = 17, .x = 18},

#define LINESECTION_PWT_ROUND2_MATCH1(lastTile) \
    {.tile = LINE_V_R,      .y =  6, .x = 11}, \
    {.tile = LINE_V_R_HALF, .y =  7, .x = 11}, \
    {.tile = lastTile,      .y =  7, .x = 12},

#define LINESECTION_PWT_ROUND2_MATCH2(lastTile) \
    {.tile = LINE_V_R, .y =  8, .x = 11}, \
    {.tile = LINE_V_R, .y =  7, .x = 11}, \
    {.tile = lastTile, .y =  7, .x = 12},

#define LINESECTION_PWT_ROUND2_MATCH3(lastTile) \
    {.tile = LINE_V_R,      .y = 14, .x = 11}, \
    {.tile = LINE_V_R_HALF, .y = 15, .x = 11}, \
    {.tile = lastTile,      .y = 15, .x = 12},

#define LINESECTION_PWT_ROUND2_MATCH4(lastTile) \
    {.tile = LINE_V_R, .y = 16, .x = 11}, \
    {.tile = LINE_V_R, .y = 15, .x = 11}, \
    {.tile = lastTile, .y = 15, .x = 12},

#define LINESECTION_PWT_ROUND2_MATCH5(lastTile) \
    {.tile = LINE_V_L,      .y =  6, .x = 18}, \
    {.tile = LINE_V_L_HALF, .y =  7, .x = 18}, \
    {.tile = lastTile,      .y =  7, .x = 17},

#define LINESECTION_PWT_ROUND2_MATCH6(lastTile) \
    {.tile = LINE_V_L, .y =  8, .x = 18}, \
    {.tile = LINE_V_L, .y =  7, .x = 18}, \
    {.tile = lastTile, .y =  7, .x = 17},

#define LINESECTION_PWT_ROUND2_MATCH7(lastTile) \
    {.tile = LINE_V_L,      .y = 14, .x = 18}, \
    {.tile = LINE_V_L_HALF, .y = 15, .x = 18}, \
    {.tile = lastTile,      .y = 15, .x = 17},

#define LINESECTION_PWT_ROUND2_MATCH8(lastTile) \
    {.tile = LINE_V_L, .y = 16, .x = 18}, \
    {.tile = LINE_V_L, .y = 15, .x = 18}, \
    {.tile = lastTile, .y = 15, .x = 17},

#define LINESECTION_PWT_SEMIFINAL_TOP_LEFT \
    {.tile = LINE_V_R,           .y =  8, .x = 12}, \
    {.tile = LINE_V_R,           .y =  9, .x = 12}, \
    {.tile = LINE_V_R,           .y = 10, .x = 12}, \
    {.tile = LINE_V_R_HALF_LOGO, .y = 11, .x = 12},

#define LINESECTION_PWT_SEMIFINAL_BOTTOM_LEFT \
    {.tile = LINE_V_R_LOGO4, .y = 14, .x = 12}, \
    {.tile = LINE_V_R_LOGO3, .y = 13, .x = 12}, \
    {.tile = LINE_V_R_LOGO2, .y = 12, .x = 12}, \
    {.tile = LINE_V_R_LOGO1, .y = 11, .x = 12},

#define LINESECTION_PWT_SEMIFINAL_TOP_RIGHT \
    {.tile = LINE_V_L,           .y =  8, .x = 17}, \
    {.tile = LINE_V_L,           .y =  9, .x = 17}, \
    {.tile = LINE_V_L,           .y = 10, .x = 17}, \
    {.tile = LINE_V_L_HALF_LOGO, .y = 11, .x = 17},

#define LINESECTION_PWT_SEMIFINAL_BOTTOM_RIGHT \
    {.tile = LINE_V_L_LOGO4, .y = 14, .x = 17}, \
    {.tile = LINE_V_L_LOGO3, .y = 13, .x = 17}, \
    {.tile = LINE_V_L_LOGO2, .y = 12, .x = 17}, \
    {.tile = LINE_V_L_LOGO1, .y = 11, .x = 17},

#define LINESECTION_PWT_FINAL_LEFT \
    {.tile = LINE_H_LOGO1, .y = 11, .x = 13}, \
    {.tile = LINE_H_LOGO2, .y = 11, .x = 14},

#define LINESECTION_PWT_FINAL_RIGHT \
    {.tile = LINE_H_LOGO4, .y = 11, .x = 16}, \
    {.tile = LINE_H_LOGO3, .y = 11, .x = 15},


static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer1Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER1(LINE_CORNER_R_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer1Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER1(LINE_CORNER_R)
    LINESECTION_PWT_ROUND2_MATCH1(LINE_CORNER_R_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer1Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER1(LINE_CORNER_R)
    LINESECTION_PWT_ROUND2_MATCH1(LINE_CORNER_R)
    LINESECTION_PWT_SEMIFINAL_TOP_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer1Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER1(LINE_CORNER_R)
    LINESECTION_PWT_ROUND2_MATCH1(LINE_CORNER_R)
    LINESECTION_PWT_SEMIFINAL_TOP_LEFT
    LINESECTION_PWT_FINAL_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer9Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER9(LINE_CORNER_R_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer9Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER9(LINE_CORNER_R)
    LINESECTION_PWT_ROUND2_MATCH1(LINE_CORNER_R_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer9Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER9(LINE_CORNER_R)
    LINESECTION_PWT_ROUND2_MATCH1(LINE_CORNER_R)
    LINESECTION_PWT_SEMIFINAL_TOP_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer9Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER9(LINE_CORNER_R)
    LINESECTION_PWT_ROUND2_MATCH1(LINE_CORNER_R)
    LINESECTION_PWT_SEMIFINAL_TOP_LEFT
    LINESECTION_PWT_FINAL_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer13Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER13(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer13Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER13(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH2(LINE_CORNER_R_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer13Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER13(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH2(LINE_CORNER_R)
    LINESECTION_PWT_SEMIFINAL_TOP_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer13Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER13(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH2(LINE_CORNER_R)
    LINESECTION_PWT_SEMIFINAL_TOP_LEFT
    LINESECTION_PWT_FINAL_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer5Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER5(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer5Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER5(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH2(LINE_CORNER_R_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer5Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER5(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH2(LINE_CORNER_R)
    LINESECTION_PWT_SEMIFINAL_TOP_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer5Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER5(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH2(LINE_CORNER_R)
    LINESECTION_PWT_SEMIFINAL_TOP_LEFT
    LINESECTION_PWT_FINAL_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer8Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER8(LINE_CORNER_R_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer8Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER8(LINE_CORNER_R)
    LINESECTION_PWT_ROUND2_MATCH3(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer8Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER8(LINE_CORNER_R)
    LINESECTION_PWT_ROUND2_MATCH3(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer8Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER8(LINE_CORNER_R)
    LINESECTION_PWT_ROUND2_MATCH3(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_LEFT
    LINESECTION_PWT_FINAL_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer16Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER16(LINE_CORNER_R_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer16Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER16(LINE_CORNER_R)
    LINESECTION_PWT_ROUND2_MATCH3(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer16Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER16(LINE_CORNER_R)
    LINESECTION_PWT_ROUND2_MATCH3(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer16Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER16(LINE_CORNER_R)
    LINESECTION_PWT_ROUND2_MATCH3(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_LEFT
    LINESECTION_PWT_FINAL_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer12Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER12(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer12Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER12(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH4(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer12Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER12(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH4(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer12Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER12(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH4(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_LEFT
    LINESECTION_PWT_FINAL_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer4Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER4(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer4Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER4(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH4(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer4Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER4(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH4(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer4Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER4(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH4(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_LEFT
    LINESECTION_PWT_FINAL_LEFT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer3Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER3(LINE_CORNER_L_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer3Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER3(LINE_CORNER_L)
    LINESECTION_PWT_ROUND2_MATCH5(LINE_CORNER_L_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer3Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER3(LINE_CORNER_L)
    LINESECTION_PWT_ROUND2_MATCH5(LINE_CORNER_L)
    LINESECTION_PWT_SEMIFINAL_TOP_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer3Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER3(LINE_CORNER_L)
    LINESECTION_PWT_ROUND2_MATCH5(LINE_CORNER_L)
    LINESECTION_PWT_SEMIFINAL_TOP_RIGHT
    LINESECTION_PWT_FINAL_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer11Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER11(LINE_CORNER_L_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer11Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER11(LINE_CORNER_L)
    LINESECTION_PWT_ROUND2_MATCH5(LINE_CORNER_L_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer11Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER11(LINE_CORNER_L)
    LINESECTION_PWT_ROUND2_MATCH5(LINE_CORNER_L)
    LINESECTION_PWT_SEMIFINAL_TOP_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer11Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER11(LINE_CORNER_L)
    LINESECTION_PWT_ROUND2_MATCH5(LINE_CORNER_L)
    LINESECTION_PWT_SEMIFINAL_TOP_RIGHT
    LINESECTION_PWT_FINAL_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer15Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER15(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer15Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER15(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH6(LINE_CORNER_L_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer15Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER15(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH6(LINE_CORNER_L)
    LINESECTION_PWT_SEMIFINAL_TOP_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer15Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER15(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH6(LINE_CORNER_L)
    LINESECTION_PWT_SEMIFINAL_TOP_RIGHT
    LINESECTION_PWT_FINAL_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer7Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER7(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer7Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER7(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH6(LINE_CORNER_L_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer7Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER7(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH6(LINE_CORNER_L)
    LINESECTION_PWT_SEMIFINAL_TOP_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer7Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER7(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH6(LINE_CORNER_L)
    LINESECTION_PWT_SEMIFINAL_TOP_RIGHT
    LINESECTION_PWT_FINAL_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer6Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER6(LINE_CORNER_L_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer6Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER6(LINE_CORNER_L)
    LINESECTION_PWT_ROUND2_MATCH7(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer6Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER6(LINE_CORNER_L)
    LINESECTION_PWT_ROUND2_MATCH7(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer6Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER6(LINE_CORNER_L)
    LINESECTION_PWT_ROUND2_MATCH7(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_RIGHT
    LINESECTION_PWT_FINAL_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer14Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER14(LINE_CORNER_L_HALF)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer14Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER14(LINE_CORNER_L)
    LINESECTION_PWT_ROUND2_MATCH7(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer14Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER14(LINE_CORNER_L)
    LINESECTION_PWT_ROUND2_MATCH7(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer14Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER14(LINE_CORNER_L)
    LINESECTION_PWT_ROUND2_MATCH7(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_RIGHT
    LINESECTION_PWT_FINAL_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer10Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER10(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer10Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER10(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH8(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer10Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER10(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH8(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer10Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER10(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH8(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_RIGHT
    LINESECTION_PWT_FINAL_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer2Round1[] =
{
    LINESECTION_PWT_ROUND1_TRAINER2(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer2Round2[] =
{
    LINESECTION_PWT_ROUND1_TRAINER2(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH8(LINE_H)
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer2Semifinal[] =
{
    LINESECTION_PWT_ROUND1_TRAINER2(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH8(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_RIGHT
};

static const struct PWTTourneyTreeLineSection sLineSectionPWTTrainer2Final[] =
{
    LINESECTION_PWT_ROUND1_TRAINER2(LINE_H)
    LINESECTION_PWT_ROUND2_MATCH8(LINE_H)
    LINESECTION_PWT_SEMIFINAL_BOTTOM_RIGHT
    LINESECTION_PWT_FINAL_RIGHT
};

static const struct PWTTourneyTreeLineSection *const sPWTTourneyTreeLineSections[PWT_TOURNAMENT_SIZE][PWT_ROUNDS_COUNT] =
{
    {sLineSectionPWTTrainer1Round1,  sLineSectionPWTTrainer1Round2,  sLineSectionPWTTrainer1Semifinal,  sLineSectionPWTTrainer1Final},
    {sLineSectionPWTTrainer2Round1,  sLineSectionPWTTrainer2Round2,  sLineSectionPWTTrainer2Semifinal,  sLineSectionPWTTrainer2Final},
    {sLineSectionPWTTrainer3Round1,  sLineSectionPWTTrainer3Round2,  sLineSectionPWTTrainer3Semifinal,  sLineSectionPWTTrainer3Final},
    {sLineSectionPWTTrainer4Round1,  sLineSectionPWTTrainer4Round2,  sLineSectionPWTTrainer4Semifinal,  sLineSectionPWTTrainer4Final},
    {sLineSectionPWTTrainer5Round1,  sLineSectionPWTTrainer5Round2,  sLineSectionPWTTrainer5Semifinal,  sLineSectionPWTTrainer5Final},
    {sLineSectionPWTTrainer6Round1,  sLineSectionPWTTrainer6Round2,  sLineSectionPWTTrainer6Semifinal,  sLineSectionPWTTrainer6Final},
    {sLineSectionPWTTrainer7Round1,  sLineSectionPWTTrainer7Round2,  sLineSectionPWTTrainer7Semifinal,  sLineSectionPWTTrainer7Final},
    {sLineSectionPWTTrainer8Round1,  sLineSectionPWTTrainer8Round2,  sLineSectionPWTTrainer8Semifinal,  sLineSectionPWTTrainer8Final},
    {sLineSectionPWTTrainer9Round1,  sLineSectionPWTTrainer9Round2,  sLineSectionPWTTrainer9Semifinal,  sLineSectionPWTTrainer9Final},
    {sLineSectionPWTTrainer10Round1, sLineSectionPWTTrainer10Round2, sLineSectionPWTTrainer10Semifinal, sLineSectionPWTTrainer10Final},
    {sLineSectionPWTTrainer11Round1, sLineSectionPWTTrainer11Round2, sLineSectionPWTTrainer11Semifinal, sLineSectionPWTTrainer11Final},
    {sLineSectionPWTTrainer12Round1, sLineSectionPWTTrainer12Round2, sLineSectionPWTTrainer12Semifinal, sLineSectionPWTTrainer12Final},
    {sLineSectionPWTTrainer13Round1, sLineSectionPWTTrainer13Round2, sLineSectionPWTTrainer13Semifinal, sLineSectionPWTTrainer13Final},
    {sLineSectionPWTTrainer14Round1, sLineSectionPWTTrainer14Round2, sLineSectionPWTTrainer14Semifinal, sLineSectionPWTTrainer14Final},
    {sLineSectionPWTTrainer15Round1, sLineSectionPWTTrainer15Round2, sLineSectionPWTTrainer15Semifinal, sLineSectionPWTTrainer15Final},
    {sLineSectionPWTTrainer16Round1, sLineSectionPWTTrainer16Round2, sLineSectionPWTTrainer16Semifinal, sLineSectionPWTTrainer16Final},
};

static const u8 sPWTTourneyTreeLineSectionArrayCounts[PWT_TOURNAMENT_SIZE][PWT_ROUNDS_COUNT] =
{
    {ARRAY_COUNT(sLineSectionPWTTrainer1Round1),  ARRAY_COUNT(sLineSectionPWTTrainer1Round2),  ARRAY_COUNT(sLineSectionPWTTrainer1Semifinal),  ARRAY_COUNT(sLineSectionPWTTrainer1Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer2Round1),  ARRAY_COUNT(sLineSectionPWTTrainer2Round2),  ARRAY_COUNT(sLineSectionPWTTrainer2Semifinal),  ARRAY_COUNT(sLineSectionPWTTrainer2Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer3Round1),  ARRAY_COUNT(sLineSectionPWTTrainer3Round2),  ARRAY_COUNT(sLineSectionPWTTrainer3Semifinal),  ARRAY_COUNT(sLineSectionPWTTrainer3Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer4Round1),  ARRAY_COUNT(sLineSectionPWTTrainer4Round2),  ARRAY_COUNT(sLineSectionPWTTrainer4Semifinal),  ARRAY_COUNT(sLineSectionPWTTrainer4Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer5Round1),  ARRAY_COUNT(sLineSectionPWTTrainer5Round2),  ARRAY_COUNT(sLineSectionPWTTrainer5Semifinal),  ARRAY_COUNT(sLineSectionPWTTrainer5Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer6Round1),  ARRAY_COUNT(sLineSectionPWTTrainer6Round2),  ARRAY_COUNT(sLineSectionPWTTrainer6Semifinal),  ARRAY_COUNT(sLineSectionPWTTrainer6Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer7Round1),  ARRAY_COUNT(sLineSectionPWTTrainer7Round2),  ARRAY_COUNT(sLineSectionPWTTrainer7Semifinal),  ARRAY_COUNT(sLineSectionPWTTrainer7Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer8Round1),  ARRAY_COUNT(sLineSectionPWTTrainer8Round2),  ARRAY_COUNT(sLineSectionPWTTrainer8Semifinal),  ARRAY_COUNT(sLineSectionPWTTrainer8Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer9Round1),  ARRAY_COUNT(sLineSectionPWTTrainer9Round2),  ARRAY_COUNT(sLineSectionPWTTrainer9Semifinal),  ARRAY_COUNT(sLineSectionPWTTrainer9Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer10Round1), ARRAY_COUNT(sLineSectionPWTTrainer10Round2), ARRAY_COUNT(sLineSectionPWTTrainer10Semifinal), ARRAY_COUNT(sLineSectionPWTTrainer10Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer11Round1), ARRAY_COUNT(sLineSectionPWTTrainer11Round2), ARRAY_COUNT(sLineSectionPWTTrainer11Semifinal), ARRAY_COUNT(sLineSectionPWTTrainer11Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer12Round1), ARRAY_COUNT(sLineSectionPWTTrainer12Round2), ARRAY_COUNT(sLineSectionPWTTrainer12Semifinal), ARRAY_COUNT(sLineSectionPWTTrainer12Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer13Round1), ARRAY_COUNT(sLineSectionPWTTrainer13Round2), ARRAY_COUNT(sLineSectionPWTTrainer13Semifinal), ARRAY_COUNT(sLineSectionPWTTrainer13Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer14Round1), ARRAY_COUNT(sLineSectionPWTTrainer14Round2), ARRAY_COUNT(sLineSectionPWTTrainer14Semifinal), ARRAY_COUNT(sLineSectionPWTTrainer14Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer15Round1), ARRAY_COUNT(sLineSectionPWTTrainer15Round2), ARRAY_COUNT(sLineSectionPWTTrainer15Semifinal), ARRAY_COUNT(sLineSectionPWTTrainer15Final)},
    {ARRAY_COUNT(sLineSectionPWTTrainer16Round1), ARRAY_COUNT(sLineSectionPWTTrainer16Round2), ARRAY_COUNT(sLineSectionPWTTrainer16Semifinal), ARRAY_COUNT(sLineSectionPWTTrainer16Final)},
};

void CallPWTFunction(void)
{
    sPWTFunctions[gSpecialVar_0x8004]();
}

// Initialize the tournament
static void PWTInit(void)
{
    gSaveBlock2Ptr->pwt.isActive = TRUE;
    SetDynamicWarp(0, gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum, WARP_ID_NONE);
    TRAINER_BATTLE_PARAM.opponentA = 0;
}

static void GetPWTData(void)
{
    switch (gSpecialVar_0x8005)
    {
    case PWT_DATA_TOURNAMENT_TYPE:
        gSpecialVar_Result = gSaveBlock2Ptr->pwt.tournament;
        break;
    case PWT_DATA_BATTLE_STYLE:
        gSpecialVar_Result = gSaveBlock2Ptr->pwt.battleStyle;
        break;
    case PWT_DATA_ROUND_NUM:
        gSpecialVar_Result = gSaveBlock2Ptr->pwt.roundNum;
        break;
    }
}

static void SetPWTData(void)
{
    switch (gSpecialVar_0x8005)
    {
    case PWT_DATA_TOURNAMENT_TYPE:
        gSaveBlock2Ptr->pwt.tournament = gSpecialVar_0x8006;
        break;
    case PWT_DATA_BATTLE_STYLE:
        gSaveBlock2Ptr->pwt.battleStyle = gSpecialVar_0x8006;
        break;
    case PWT_DATA_ROUND_NUM:
        gSaveBlock2Ptr->pwt.roundNum = gSpecialVar_0x8006;
        break;
    }
}

// Initialize the trainers
static void PWTInitTrainers(void)
{
    u8 i, j, k;
    u16 trainerId;
    u16 monId;

    u8 tournamentType = gSaveBlock2Ptr->pwt.tournament;

    // ---------- STORE PLAYER DATA ----------
    PWT_TRAINERS[0].trainerId = TRAINER_PLAYER;
    PWT_TRAINERS[0].isEliminated = FALSE;
    PWT_TRAINERS[0].eliminatedAt = 0;
    PWT_TRAINERS[0].forfeited = FALSE;
    // Get player tourney page's data
    for (i = 0; i < PARTY_SIZE; i++)
        PWT_MONS[0][i] = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, NULL);

    // ---------- GET TRAINERS ----------
    // For every tournament slot
    for (i = 1; i < PWT_TOURNAMENT_SIZE; i++)
    {
        DebugPrintf("Trainer %d", i);
        // Find a new unique trainerId
        do
        {
            trainerId = GetRandomPWTTrainerId(tournamentType);
            for (j = 1; j < i; j++)
            {
                if (PWT_TRAINERS[j].trainerId == trainerId)
                    break;
            }
        } while (j != i);
        PWT_TRAINERS[i].trainerId = trainerId;
        DebugPrintf("Trainer %d: ID # %d", i, trainerId);

        // Generate their full teams
        // Assume their team pools are already valid
        for (j = 0; j < PARTY_SIZE; j++)
        {
            do
            {
                // Get a Pokémon according to Pool
                // First, select their megas
                if (j == 0)
                    monId = GetRandomPWTMonFromSet(trainerId, PWT_TRAINER_POOL_MEGA);
                // For their last Pokémon, 50/50 chance of choosing legendary instead
                else if (j == PARTY_SIZE - 1)
                {
                    if (Random() % 2 == 0)
                        monId = GetRandomPWTMonFromSet(trainerId, PWT_TRAINER_POOL_LEGEND);
                    else
                        monId = GetRandomPWTMonFromSet(trainerId, PWT_TRAINER_POOL_REGULAR);
                }
                else
                    monId = GetRandomPWTMonFromSet(trainerId, PWT_TRAINER_POOL_REGULAR);
                // Now, verify that the mon is not already on the party
                for (k = 0; k < j; k++)
                {
                    if (PWT_MONS[i][k] == monId)
                        break;
                }
            } while (k != j);
            PWT_MONS[i][j] = monId;
            DebugPrintf("Trainer %d: monId # %d", i, monId);
        }

        // Now initialize their data
        PWT_TRAINERS[i].isEliminated = FALSE;
        PWT_TRAINERS[i].eliminatedAt = 0;
        PWT_TRAINERS[i].forfeited = FALSE;
    }
}

static void BufferPWTRoundText(void)
{
    StringCopy(gStringVar1, gPWTRoundsStringTable[gSaveBlock2Ptr->pwt.roundNum]);
}

static void BufferNextPWTBattleStrings(void)
{
    StringCopy(gStringVar1, gPWTRoundsStringTable[gSaveBlock2Ptr->pwt.roundNum]);
    CopyPWTTrainerName(gStringVar2, TRAINER_BATTLE_PARAM.opponentA);
}

static void SetPWTOpponentParty(void)
{
    CalculatePlayerPartyCount();
    CreatePWTOpponentMons(TrainerIdToPWTTournamentId(TRAINER_BATTLE_PARAM.opponentA));
}

// TODO - rewrite this fucntion?
static void CreatePWTOpponentMon(u8 monPartyId, u16 tournamentTrainerId, u8 tournamentMonId, u32 otId)
{
    // Predfine variables
    u8 ball, fixedIVs;
    u32 personality, friendship, j, ability;
    u8 level = 50;

    SetPWTTrainerAndMonPtrs(); // Always 50, but still need to set pointers
    // Simplify variable calls
    const struct TrainerMon *fmon = &gPWTFacilityTrainerMons[PWT_MONS[tournamentTrainerId][tournamentMonId]];
    struct Pokemon *dst = &gEnemyParty[monPartyId];
    // Determine stats
    fixedIVs = gPWTFacilityTrainerMons[PWT_MONS[tournamentTrainerId][tournamentMonId]].iv; // PWT have identical IVs for all stats
    // Generate a personality based on gender
    // TODO - pretty sure this is wrong, and Pokémon should be set with MON_MALE and MON_FEMALE, but error is simply Pokémon with wrong genders so not priority
    switch (fmon->gender)
    {
        case TRAINER_MON_MALE:
            personality = GeneratePersonalityForGender(MON_MALE, fmon->species);
        case TRAINER_MON_FEMALE:
            personality = GeneratePersonalityForGender(MON_FEMALE, fmon->species);
        case TRAINER_MON_RANDOM_GENDER:
        default:
            personality = Random() % 2 ? GeneratePersonalityForGender(MON_MALE, fmon->species) : GeneratePersonalityForGender(MON_FEMALE, fmon->species);
    }
    // Set nature if defined
    if (fmon->nature != NUM_NATURES)
        ModifyPersonalityForNature(&personality, fmon->nature);
    // Generate the base mon
    CreateMon(dst, fmon->species, level, fixedIVs, TRUE, personality, OT_ID_PRESET, otId);
    // Set remaining values
    for (j = 0; j < MAX_MON_MOVES; j++)
        SetMonMoveSlot(dst, fmon->moves[j], j);
    friendship = MAX_FRIENDSHIP;
    SetMonData(dst, MON_DATA_FRIENDSHIP, &friendship);
    SetMonData(dst, MON_DATA_HELD_ITEM, &fmon->heldItem);
    // Set ability if defined
    // TODO - test if the Pokémon still has an ability naturally if not defined
        // try to set ability. Otherwise, random of non-hidden as per vanilla
    if (fmon->ability != ABILITY_NONE)
    {
        const struct SpeciesInfo *speciesInfo = &gSpeciesInfo[fmon->species];
        u32 maxAbilities = ARRAY_COUNT(speciesInfo->abilities);
        for (ability = 0; ability < maxAbilities; ++ability)
        {
            if (speciesInfo->abilities[ability] == fmon->ability)
                break;
        }
        if (ability >= maxAbilities)
            ability = 0;
        SetMonData(dst, MON_DATA_ABILITY_NUM, &ability);
    }
    // if (fmon->ability != ABILITY_NONE)
    //     SetMonData(dst, MON_DATA_ABILITY_NUM, &(fmon->ability));
    if (fmon->ev != NULL)
    {
        SetMonData(dst, MON_DATA_HP_EV, &(fmon->ev[0]));
        SetMonData(dst, MON_DATA_ATK_EV, &(fmon->ev[1]));
        SetMonData(dst, MON_DATA_DEF_EV, &(fmon->ev[2]));
        SetMonData(dst, MON_DATA_SPATK_EV, &(fmon->ev[3]));
        SetMonData(dst, MON_DATA_SPDEF_EV, &(fmon->ev[4]));
        SetMonData(dst, MON_DATA_SPEED_EV, &(fmon->ev[5]));
    }
    if (fmon->isShiny)
    {
        u32 data = TRUE;
        SetMonData(dst, MON_DATA_IS_SHINY, &data);
    }
    ball = GetPWTMonBall(gPWTFacilityTrainerMons[PWT_MONS[tournamentTrainerId][tournamentMonId]].species);
    SetMonData(dst, MON_DATA_POKEBALL, &ball);

    CalculateMonStats(dst);
}

static void CreatePWTOpponentMons(u16 tournamentTrainerId)
{
    u8 partyIdx;
    u16 tournamentMonId;
    u16 selectedIds[PWT_PARTY_SIZE];
    u8 j;
    // Initialize the party to 0
    ZeroEnemyPartyMons();
    // For every slot in the party
    for (partyIdx = 0; partyIdx < PWT_PARTY_SIZE; partyIdx++)
    {
        do
        {
            // Select a random Pokémon
            tournamentMonId = Random() % PARTY_SIZE;
            // Check that it hasn't been picked yet
            for (j = 0; j < partyIdx; j++)
            {
                if (selectedIds[j] == tournamentMonId)
                    break;
            }
        } while (j != partyIdx);
        // New Pokémon selected
        selectedIds[partyIdx] = tournamentMonId;
        // Now create the Pokémon
        CreatePWTOpponentMon(partyIdx, tournamentTrainerId, tournamentMonId, Random32());
    }
}

static int TournamentIdOfPWTOpponent(int roundId, int trainerId)
{
    int i, j, opponentMax;

    // Get trainer's tournament id
    for (i = 0; i < PWT_TOURNAMENT_SIZE; i++)
    {
        if (PWT_TRAINERS[i].trainerId == trainerId)
            break;
    }

    // Get trainer's opponent's tournament id
    if (roundId != PWT_ROUND1)
    {
        if (roundId == PWT_FINAL)
            opponentMax = sIdToPWTOpponentId[i][roundId] + 8;
        else
            opponentMax = sIdToPWTOpponentId[i][roundId] + 4;

        // Get first non-eliminated trainer in range of possible opponents
        for (j = sIdToPWTOpponentId[i][roundId]; j < opponentMax; j++)
        {
            if (sPWTTourneyTreeTrainerOpponentIds[j] != i && !PWT_TRAINERS[sPWTTourneyTreeTrainerOpponentIds[j]].isEliminated)
                break;
        }

        if (j != opponentMax)
            return sPWTTourneyTreeTrainerOpponentIds[j];
        else
            return 0xFF; // Already eliminated
    }
    else
    {
        if (!PWT_TRAINERS[sIdToPWTOpponentId[i][roundId]].isEliminated)
            return sIdToPWTOpponentId[i][roundId];
        else
            return 0xFF; // Already eliminated
    }
}

static void SetPWTOpponentId(void)
{
    TRAINER_BATTLE_PARAM.opponentA = TrainerIdOfPWTPlayerOpponent();
}

// While not an issue in-game, this will overflow if called after the player's opponent for the current round has been eliminated
static u16 TrainerIdOfPWTPlayerOpponent(void)
{
    return PWT_TRAINERS[TournamentIdOfPWTOpponent(gSaveBlock2Ptr->pwt.roundNum, TRAINER_PLAYER)].trainerId;
}

static void SetPWTOpponentGFX(void)
{
    u16 trainerId = TRAINER_BATTLE_PARAM.opponentA;
    SetPWTTrainerAndMonPtrs();
    VarSet(VAR_OBJ_GFX_ID_0, gPWTFacilityTrainers[trainerId].trainerObjectGfxId);
}

// For showing the opponent info card of the upcoming trainer
static void ShowPWTOpponentInfo(void)
{
    u8 taskId = CreateTask(Task_ShowPWTTourneyInfoCard, 0);
    gTasks[taskId].tState = 0;
    gTasks[taskId].tTournamentId = TrainerIdToPWTTournamentId(TrainerIdOfPWTPlayerOpponent());
    gTasks[taskId].tMode = PWT_INFOCARD_NEXT_OPPONENT;
    gTasks[taskId].tPrevTaskId = 0;

    SetMainCallback2(CB2_PWTTourneyTree);
}

// For showing the opponent info card or the match info card
static void Task_ShowPWTTourneyInfoCard(u8 taskId)
{
    int i;
    int tournamentId = gTasks[taskId].tTournamentId;
    int mode = gTasks[taskId].tMode;
    int id = gTasks[taskId].tPrevTaskId;

    switch (gTasks[taskId].tState)
    {
    case 0:
        SetHBlankCallback(NULL);
        SetVBlankCallback(NULL);
        EnableInterrupts(INTR_FLAG_VBLANK);
        CpuFill32(0, (void *)VRAM, VRAM_SIZE);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sPWTInfoCardBgTemplates, ARRAY_COUNT(sPWTInfoCardBgTemplates));
        InitWindows(sPWTInfoCardWindowTemplates);
        DeactivateAllTextPrinters();
        gBattle_BG0_X = 0;
        gBattle_BG0_Y = 0;
        gBattle_BG1_X = 0;
        gBattle_BG1_Y = 0;
        gBattle_BG3_X = 0;
        gBattle_BG3_Y = 0;
        if (mode == PWT_INFOCARD_MATCH)
            gBattle_BG2_X = 0, gBattle_BG2_Y = 0;
        else
            gBattle_BG2_X = 0, gBattle_BG2_Y = DISPLAY_HEIGHT;

        gTasks[taskId].tState++;
        break;
    case 1:
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        SetGpuReg(REG_OFFSET_MOSAIC, 0);
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WIN1H, 0);
        SetGpuReg(REG_OFFSET_WIN1V, 0);
        SetGpuReg(REG_OFFSET_WININ, 0);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR);
        ResetPaletteFade();
        ResetSpriteData();
        FreeAllSpritePalettes();
        gReservedSpritePaletteCount = 4;
        gTasks[taskId].tState++;
        break;
    case 2:
        DecompressAndLoadBgGfxUsingHeap(2, gPWTTourneyInfoCard_Gfx, 0x2000, 0, 0);
        DecompressAndLoadBgGfxUsingHeap(2, gPWTTourneyInfoCard_Tilemap, 0x2000, 0, 1);
        DecompressAndLoadBgGfxUsingHeap(3, gPWTTourneyInfoCardBg_Tilemap, 0x800, 0, 1);
        LoadCompressedSpriteSheet(sPWTTourneyTreeButtonsSpriteSheet);
        LoadPalette(gPWTTourneyTree_Pal, BG_PLTT_OFFSET, BG_PLTT_SIZE);
        LoadPalette(gPWTTourneyTreeButtons_Pal, OBJ_PLTT_OFFSET, OBJ_PLTT_SIZE);
        LoadPalette(gBattleWindowTextPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        if (mode == PWT_INFOCARD_MATCH)
            LoadPalette(gPWTTourneyMatchCardBg_Pal, BG_PLTT_ID(5), PLTT_SIZE_4BPP); // Changes the moving info card bg to orange when in match card mode
        CpuFill32(0, gPlttBufferFaded, PLTT_SIZE);
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        ShowBg(3);
        gTasks[taskId].tState++;
        break;
    case 3:
        SetVBlankCallback(VblankCb_PWTTourneyInfoCard);
        sPWTInfoCard = AllocZeroed(sizeof(*sPWTInfoCard));
        for (i = 0; i < NUM_PWT_INFOCARD_SPRITES; i++)
            sPWTInfoCard->spriteIds[i] = SPRITE_NONE;
        LoadMonIconPalettes();
        i = CreateTask(Task_HandlePWTInfoCardInput, 0);
        gTasks[i].data[0] = 0;
        gTasks[i].data[2] = 0;
        gTasks[i].data[3] = mode;
        gTasks[i].data[4] = id;
        if (mode == PWT_INFOCARD_MATCH)
        {
            DisplayPWTMatchInfoOnCard(0, tournamentId);
            sPWTInfoCard->pos = 1;
        }
        else
        {
            DisplayPWTTrainerInfoOnCard(0, tournamentId);
        }
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_1D_MAP);
        if (mode != PWT_INFOCARD_NEXT_OPPONENT)
        {
            // Scroll up arrow
            id = CreateSprite(&sPWTVerticalScrollArrowSpriteTemplate, 120, 4, 0);
            StartSpriteAnim(&gSprites[id], 0);
            gSprites[id].data[0] = i;

            // Scroll down arrow
            id = CreateSprite(&sPWTVerticalScrollArrowSpriteTemplate, 120, 156, 0);
            StartSpriteAnim(&gSprites[id], 1);
            gSprites[id].data[0] = i;

            // Scroll left arrow
            id = CreateSprite(&sPWTHorizontalScrollArrowSpriteTemplate, 6, 80, 0);
            StartSpriteAnim(&gSprites[id], 0);
            gSprites[id].data[0] = i;
            gSprites[id].data[1] = 0;
            if (mode == PWT_INFOCARD_TRAINER)
                gSprites[id].invisible = TRUE;

            // Scroll right arrow
            id = CreateSprite(&sPWTHorizontalScrollArrowSpriteTemplate, 234, 80, 0);
            StartSpriteAnim(&gSprites[id], 1);
            gSprites[id].data[0] = i;
            gSprites[id].data[1] = 1;
        }
        DestroyTask(taskId);
        break;
    }
}


// Note: Card scrolling up means the current card goes down and another one appears from top.
// The same is true for scrolling left.
// That means that the sprite needs to move with the moving card in the opposite scrolling direction.
static void SpriteCB_PWTTrainerIconCardScrollUp(struct Sprite *sprite)
{
    sprite->y += 4;
    if (sprite->data[0] != 0)
    {
        if (sprite->y >= -32)
            sprite->invisible = FALSE;
        if (++sprite->data[1] == 40)
            sprite->callback = SpriteCallbackDummy;
    }
    else
    {
        if (sprite->y >= 192)
        {
            sPWTInfoCard->spriteIds[sprite->data[2]] = SPRITE_NONE;
            FreeAndDestroyTrainerPicSprite(sprite->data[3]);
        }
    }
}

static void SpriteCB_PWTTrainerIconCardScrollDown(struct Sprite *sprite)
{
    sprite->y -= 4;
    if (sprite->data[0] != 0)
    {
        if (sprite->y <= 192)
            sprite->invisible = FALSE;
        if (++sprite->data[1] == 40)
            sprite->callback = SpriteCallbackDummy;
    }
    else
    {
        if (sprite->y <= -32)
        {
            sPWTInfoCard->spriteIds[sprite->data[2]] = SPRITE_NONE;
            FreeAndDestroyTrainerPicSprite(sprite->data[3]);
        }
    }
}

static void SpriteCB_PWTTrainerIconCardScrollLeft(struct Sprite *sprite)
{
    sprite->x += 4;
    if (sprite->data[0] != 0)
    {
        if (sprite->x >= -32)
            sprite->invisible = FALSE;
        if (++sprite->data[1] == 64)
            sprite->callback = SpriteCallbackDummy;
    }
    else
    {
        if (sprite->x >= DISPLAY_WIDTH + 32)
        {
            sPWTInfoCard->spriteIds[sprite->data[2]] = SPRITE_NONE;
            FreeAndDestroyTrainerPicSprite(sprite->data[3]);
        }
    }
}

static void SpriteCB_PWTTrainerIconCardScrollRight(struct Sprite *sprite)
{
    sprite->x -= 4;
    if (sprite->data[0] != 0)
    {
        if (sprite->x <= DISPLAY_WIDTH + 32)
            sprite->invisible = FALSE;
        if (++sprite->data[1] == 64)
            sprite->callback = SpriteCallbackDummy;
    }
    else
    {
        if (sprite->x <= -32)
        {
            sPWTInfoCard->spriteIds[sprite->data[2]] = SPRITE_NONE;
            FreeAndDestroyTrainerPicSprite(sprite->data[3]);
        }
    }
}

#define sPWTMonIconStill data[3]

static void SpriteCB_MonIconPWTInfo(struct Sprite *sprite)
{
    if (!sprite->sPWTMonIconStill)
        UpdateMonIconFrame(sprite);
}

static void SpriteCB_PWTMonIconCardScrollUp(struct Sprite *sprite)
{
    if (!sprite->sPWTMonIconStill)
        UpdateMonIconFrame(sprite);
    sprite->y += 4;
    if (sprite->data[0] != 0)
    {
        if (sprite->y >= -16)
            sprite->invisible = FALSE;
        if (++sprite->data[1] == 40)
            sprite->callback = SpriteCB_MonIconPWTInfo;
    }
    else
    {
        if (sprite->y >= 176)
        {
            sPWTInfoCard->spriteIds[sprite->data[2]] = SPRITE_NONE;
            FreeAndDestroyMonIconSprite(sprite);
        }
    }
}

static void SpriteCB_PWTMonIconCardScrollDown(struct Sprite *sprite)
{
    if (!sprite->sPWTMonIconStill)
        UpdateMonIconFrame(sprite);
    sprite->y -= 4;
    if (sprite->data[0] != 0)
    {
        if (sprite->y <= 176)
            sprite->invisible = FALSE;
        if (++sprite->data[1] == 40)
            sprite->callback = SpriteCB_MonIconPWTInfo;
    }
    else
    {
        if (sprite->y <= -16)
        {
            sPWTInfoCard->spriteIds[sprite->data[2]] = SPRITE_NONE;
            FreeAndDestroyMonIconSprite(sprite);
        }
    }
}

static void SpriteCB_PWTMonIconCardScrollLeft(struct Sprite *sprite)
{
    if (!sprite->sPWTMonIconStill)
        UpdateMonIconFrame(sprite);
    sprite->x += 4;
    if (sprite->data[0] != 0)
    {
        if (sprite->x >= -16)
            sprite->invisible = FALSE;
        if (++sprite->data[1] == 64)
            sprite->callback = SpriteCB_MonIconPWTInfo;
    }
    else
    {
        if (sprite->x >= DISPLAY_WIDTH + 16)
        {
            sPWTInfoCard->spriteIds[sprite->data[2]] = SPRITE_NONE;
            FreeAndDestroyMonIconSprite(sprite);
        }
    }
}

static void SpriteCB_PWTMonIconCardScrollRight(struct Sprite *sprite)
{
    if (!sprite->sPWTMonIconStill)
        UpdateMonIconFrame(sprite);
    sprite->x -= 4;
    if (sprite->data[0] != 0)
    {
        if (sprite->x <= DISPLAY_WIDTH + 16)
            sprite->invisible = FALSE;
        if (++sprite->data[1] == 64)
            sprite->callback = SpriteCB_MonIconPWTInfo;
    }
    else
    {
        if (sprite->x <= -16)
        {
            sPWTInfoCard->spriteIds[sprite->data[2]] = SPRITE_NONE;
            FreeAndDestroyMonIconSprite(sprite);
        }
    }
}

static void SpriteCB_PWTHorizontalScrollArrow(struct Sprite *sprite)
{
    int taskId1 = sprite->data[0];
    int arrId = gTasks[gTasks[taskId1].data[4]].data[1];
    int tournmanetTrainerId = sPWTTourneyTreeTrainerIds[arrId];
    int roundId = gSaveBlock2Ptr->frontier.curChallengeBattleNum;

    if (gTasks[taskId1].data[3] == 1)
    {
        if (sprite->data[1])
        {
            if ((PWT_TRAINERS[tournmanetTrainerId].isEliminated
                && sPWTInfoCard->pos - 1 < PWT_TRAINERS[tournmanetTrainerId].eliminatedAt))
            {
                sprite->invisible = FALSE;
            }
            else if (!PWT_TRAINERS[tournmanetTrainerId].isEliminated
                     && sPWTInfoCard->pos - 1 < roundId)
            {
                sprite->invisible = FALSE;
            }
            else
            {
                if (gTasks[taskId1].data[0] == 2)
                    sprite->invisible = TRUE;
            }
        }
        else
        {
            if (sPWTInfoCard->pos != 0)
            {
                sprite->invisible = FALSE;
            }
            else
            {
                if (gTasks[taskId1].data[0] == 2)
                    sprite->invisible = TRUE;
            }
        }
    }
    else
    {
        if (sprite->data[1])
        {
            if (sPWTInfoCard->pos > 1)
            {
                if (gTasks[taskId1].data[0] == 2)
                    sprite->invisible = TRUE;
            }
            else
            {
                sprite->invisible = FALSE;
            }
        }
        else
        {
            if (sPWTInfoCard->pos != 0)
            {
                sprite->invisible = FALSE;
            }
            else
            {
                if (gTasks[taskId1].data[0] == 2)
                    sprite->invisible = TRUE;
            }
        }
    }
}

static void SpriteCB_PWTVerticalScrollArrow(struct Sprite *sprite)
{
    int taskId1 = sprite->data[0];

    if (gTasks[taskId1].data[3] == 1)
    {
        if (sPWTInfoCard->pos != 0)
        {
            if (gTasks[taskId1].data[0] == 2)
                sprite->invisible = TRUE;
        }
        else
        {
            sprite->invisible = FALSE;
        }
    }
    else
    {
        if (sPWTInfoCard->pos != 1)
        {
            if (gTasks[taskId1].data[0] == 2)
                sprite->invisible = TRUE;
        }
        else
        {
            sprite->invisible = FALSE;
        }
    }
}

// Task states for Task_HandlePWTInfoCardInput
#define STATE_FADE_IN      0
#define STATE_WAIT_FADE    1
#define STATE_GET_INPUT    2
#define STATE_REACT_INPUT  3
#define STATE_MOVE_UP      4
#define STATE_MOVE_DOWN    5
#define STATE_MOVE_LEFT    6
#define STATE_MOVE_RIGHT   7
#define STATE_CLOSE_CARD   8

#define tUsingAlternateSlot data[2] // PWT_CARD_ALTERNATE_SLOT

static void Task_HandlePWTInfoCardInput(u8 taskId)
{
    int i;
    int windowId = 0;
    int mode = gTasks[taskId].data[3];
    int taskId2 = gTasks[taskId].data[4];
    int trainerTourneyId = 0;
    int matchNo = 0;

    switch (gTasks[taskId].tState)
    {
    case STATE_FADE_IN:
        if (!gPaletteFade.active)
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
            gTasks[taskId].tState = STATE_WAIT_FADE;
        }
        break;
    case STATE_WAIT_FADE:
        if (!gPaletteFade.active)
            gTasks[taskId].tState = STATE_GET_INPUT;
        break;
    case STATE_GET_INPUT:
        i = Task_GetInfoCardInput(taskId);
        switch (i)
        {
        case PWT_INFOCARD_INPUT_AB:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
            gTasks[taskId].tState = STATE_CLOSE_CARD;
            break;
        case PWT_TRAINERCARD_INPUT_UP ... PWT_TRAINERCARD_INPUT_RIGHT:
        case PWT_MATCHCARD_INPUT_UP ... PWT_MATCHCARD_INPUT_RIGHT:
            gTasks[taskId].data[5] = i;
            if (gTasks[taskId].tUsingAlternateSlot)
                windowId = NUM_PWT_INFO_CARD_WINDOWS;
            else
                windowId = 0;

            for (i = windowId; i < windowId + NUM_PWT_INFO_CARD_WINDOWS; i++)
            {
                CopyWindowToVram(i, COPYWIN_GFX);
                FillWindowPixelBuffer(i, PIXEL_FILL(0));
            }
            gTasks[taskId].tState = STATE_REACT_INPUT;
            break;
        case PWT_INFOCARD_INPUT_NONE:
            break;
        }
        break;
    case STATE_REACT_INPUT:
        i = gTasks[taskId].data[5];
        switch (i)
        {
        case PWT_TRAINERCARD_INPUT_UP:
        case PWT_MATCHCARD_INPUT_UP:
            if (gTasks[taskId].tUsingAlternateSlot)
            {
                gBattle_BG0_X = 0;
                gBattle_BG0_Y = 0;
                gBattle_BG1_X = 0;
                gBattle_BG1_Y = DISPLAY_HEIGHT;
            }
            else
            {
                gBattle_BG0_X = 0;
                gBattle_BG0_Y = DISPLAY_HEIGHT;
                gBattle_BG1_X = 0;
                gBattle_BG1_Y = 0;
            }

            if (i == PWT_TRAINERCARD_INPUT_UP)
            {
                if (sPWTInfoCard->pos == 0)
                {
                    gBattle_BG2_X = 0;
                    gBattle_BG2_Y = DISPLAY_HEIGHT * 2;
                    trainerTourneyId = sPWTTourneyTreeTrainerIds[gTasks[taskId2].data[1]];
                    DisplayPWTTrainerInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_UP, trainerTourneyId);
                }
                else
                {
                    gBattle_BG2_X = DISPLAY_WIDTH + 16;
                    gBattle_BG2_Y = 0;
                    trainerTourneyId = sPWTTourneyTreeTrainerIds[gTasks[taskId2].data[1]];
                    DisplayPWTTrainerInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_UP, trainerTourneyId);
                    sPWTInfoCard->pos = 0;
                }
            }
            else // i == PWT_MATCHCARD_INPUT_UP
            {
                if (sPWTInfoCard->pos == 0)
                {
                    matchNo = gTasks[taskId2].data[1] - 16;
                    BufferPWTWinString(matchNo, sPWTInfoCard->tournamentIds);
                    gBattle_BG2_X = 0;
                    gBattle_BG2_Y = DISPLAY_HEIGHT * 2;
                    trainerTourneyId = sPWTInfoCard->tournamentIds[0];
                    DisplayPWTTrainerInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_UP, trainerTourneyId);
                }
                else if (sPWTInfoCard->pos == 2)
                {
                    matchNo = gTasks[taskId2].data[1] - 16;
                    BufferPWTWinString(matchNo, sPWTInfoCard->tournamentIds);
                    gBattle_BG2_X = 0;
                    gBattle_BG2_Y = DISPLAY_HEIGHT * 2;
                    trainerTourneyId = sPWTInfoCard->tournamentIds[1];
                    DisplayPWTTrainerInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_UP, trainerTourneyId);
                }
                else
                {
                    gBattle_BG2_X = DISPLAY_WIDTH + 16;
                    gBattle_BG2_Y = DISPLAY_HEIGHT;
                    matchNo = gTasks[taskId2].data[1] - 16;
                    DisplayPWTMatchInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_UP, matchNo);
                }
            }

            for (i = 0; i < NUM_PWT_INFOCARD_SPRITES / 2; i++)
            {
                if (i < 2)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTTrainerIconCardScrollUp;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot ^ 1;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[3] = sPWTInfoCard->spriteIds[i];
                    }
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTMonIconCardScrollUp;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot ^ 1;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                    }
                }
            }
            for (i = NUM_PWT_INFOCARD_SPRITES / 2; i < NUM_PWT_INFOCARD_SPRITES; i++)
            {
                if (i < 16)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTTrainerIconCardScrollUp;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[3] = sPWTInfoCard->spriteIds[i];
                    }
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTMonIconCardScrollUp;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                    }
                }
            }

            gTasks[taskId].tState = STATE_MOVE_UP;
            gTasks[taskId].data[5] = 0;
            break;
        case PWT_TRAINERCARD_INPUT_DOWN:
        case PWT_MATCHCARD_INPUT_DOWN:
            if (gTasks[taskId].tUsingAlternateSlot)
            {
                gBattle_BG0_X = 0;
                gBattle_BG0_Y = 0;
                gBattle_BG1_X = 0;
                gBattle_BG1_Y = -DISPLAY_HEIGHT;
            }
            else
            {
                gBattle_BG0_X = 0;
                gBattle_BG0_Y = -DISPLAY_HEIGHT;
                gBattle_BG1_X = 0;
                gBattle_BG1_Y = 0;
            }

            if (i == PWT_TRAINERCARD_INPUT_DOWN)
            {
                if (sPWTInfoCard->pos == 0)
                {
                    gBattle_BG2_X = 0;
                    gBattle_BG2_Y = DISPLAY_HEIGHT;
                    trainerTourneyId = sPWTTourneyTreeTrainerIds[gTasks[taskId2].data[1]];
                    DisplayPWTTrainerInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_DOWN, trainerTourneyId);
                }
                else
                {
                    gBattle_BG2_X = 0;
                    gBattle_BG2_Y = 0;
                    trainerTourneyId = sPWTTourneyTreeTrainerIds[gTasks[taskId2].data[1]];
                    DisplayPWTTrainerInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_DOWN, trainerTourneyId);
                    sPWTInfoCard->pos = 0;
                }
            }
            else // i == PWT_MATCHCARD_INPUT_DOWN
            {
                if (sPWTInfoCard->pos == 0)
                {
                    matchNo = gTasks[taskId2].data[1] - 16;
                    BufferPWTWinString(matchNo, sPWTInfoCard->tournamentIds);
                    gBattle_BG2_X = 0;
                    gBattle_BG2_Y = DISPLAY_HEIGHT;
                    trainerTourneyId = sPWTInfoCard->tournamentIds[0];
                    DisplayPWTTrainerInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_DOWN, trainerTourneyId);
                }
                else if (sPWTInfoCard->pos == 2)
                {
                    matchNo = gTasks[taskId2].data[1] - 16;
                    BufferPWTWinString(matchNo, sPWTInfoCard->tournamentIds);
                    gBattle_BG2_X = 0;
                    gBattle_BG2_Y = DISPLAY_HEIGHT;
                    trainerTourneyId = sPWTInfoCard->tournamentIds[1];
                    DisplayPWTTrainerInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_DOWN, trainerTourneyId);
                }
                else
                {
                    gBattle_BG2_X = DISPLAY_WIDTH + 16;
                    gBattle_BG2_Y = 0;
                    matchNo = gTasks[taskId2].data[1] - 16;
                    DisplayPWTMatchInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_DOWN, matchNo);
                }
            }

            for (i = 0; i < NUM_PWT_INFOCARD_SPRITES / 2; i++)
            {
                if (i < 2)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTTrainerIconCardScrollDown;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot ^ 1;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[3] = sPWTInfoCard->spriteIds[i];
                    }
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTMonIconCardScrollDown;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot ^ 1;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                    }
                }
            }
            for (i = NUM_PWT_INFOCARD_SPRITES / 2; i < NUM_PWT_INFOCARD_SPRITES; i++)
            {
                if (i < 16)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTTrainerIconCardScrollDown;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[3] = sPWTInfoCard->spriteIds[i];
                    }
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTMonIconCardScrollDown;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                    }
                }
            }

            gTasks[taskId].tState = STATE_MOVE_DOWN;
            gTasks[taskId].data[5] = 0;
            break;
        case PWT_TRAINERCARD_INPUT_LEFT:
            if (gTasks[taskId].tUsingAlternateSlot)
            {
                gBattle_BG0_X = 0;
                gBattle_BG0_Y = 0;
                gBattle_BG1_X = DISPLAY_WIDTH + 16;
                gBattle_BG1_Y = 0;
            }
            else
            {
                gBattle_BG0_X = DISPLAY_WIDTH + 16;
                gBattle_BG0_Y = 0;
                gBattle_BG1_X = 0;
                gBattle_BG1_Y = 0;
            }

            if (sPWTInfoCard->pos == 0)
            {
                gBattle_BG2_X = DISPLAY_WIDTH + 16;
                gBattle_BG2_Y = DISPLAY_HEIGHT;
                trainerTourneyId = sPWTTourneyTreeTrainerIds[gTasks[taskId2].data[1]];
                DisplayPWTTrainerInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_LEFT, trainerTourneyId);
            }
            else
            {
                gBattle_BG2_X = DISPLAY_WIDTH + 16;
                gBattle_BG2_Y = 0;
                matchNo = sIdToPWTMatchNumber[gTasks[taskId2].data[1]][sPWTInfoCard->pos - 1];
                DisplayPWTMatchInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_LEFT, matchNo);
            }

            for (i = 0; i < NUM_PWT_INFOCARD_SPRITES / 2; i++)
            {
                if (i < 2)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTTrainerIconCardScrollLeft;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot ^ 1;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[3] = sPWTInfoCard->spriteIds[i];
                    }
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTMonIconCardScrollLeft;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot ^ 1;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                    }
                }
            }
            for (i = NUM_PWT_INFOCARD_SPRITES / 2; i < NUM_PWT_INFOCARD_SPRITES; i++)
            {
                if (i < 16)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTTrainerIconCardScrollLeft;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[3] = sPWTInfoCard->spriteIds[i];
                    }
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTMonIconCardScrollLeft;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                    }
                }
            }

            gTasks[taskId].tState = STATE_MOVE_LEFT;
            gTasks[taskId].data[5] = 0;
            break;
        case PWT_MATCHCARD_INPUT_LEFT:
            if (gTasks[taskId].tUsingAlternateSlot)
            {
                gBattle_BG0_X = 0;
                gBattle_BG0_Y = 0;
                gBattle_BG1_X = DISPLAY_WIDTH + 16;
                gBattle_BG1_Y = 0;
            }
            else
            {
                gBattle_BG0_X = DISPLAY_WIDTH + 16;
                gBattle_BG0_Y = 0;
                gBattle_BG1_X = 0;
                gBattle_BG1_Y = 0;
            }

            if (sPWTInfoCard->pos == 0)
            {
                gBattle_BG2_X = DISPLAY_WIDTH + 16;
                gBattle_BG2_Y = DISPLAY_HEIGHT;
                trainerTourneyId = sPWTInfoCard->tournamentIds[0];
                DisplayPWTTrainerInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_LEFT, trainerTourneyId);
            }
            else
            {
                gBattle_BG2_X = 0;
                gBattle_BG2_Y = DISPLAY_HEIGHT;
                matchNo = gTasks[taskId2].data[1] - 16;
                DisplayPWTMatchInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_LEFT, matchNo);
            }

            for (i = 0; i < NUM_PWT_INFOCARD_SPRITES / 2; i++)
            {
                if (i < 2)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTTrainerIconCardScrollLeft;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot ^ 1;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[3] = sPWTInfoCard->spriteIds[i];
                    }
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTMonIconCardScrollLeft;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot ^ 1;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                    }
                }
            }
            for (i = NUM_PWT_INFOCARD_SPRITES / 2; i < NUM_PWT_INFOCARD_SPRITES; i++)
            {
                if (i < 16)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTTrainerIconCardScrollLeft;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[3] = sPWTInfoCard->spriteIds[i];
                    }
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTMonIconCardScrollLeft;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                    }
                }
            }

            gTasks[taskId].tState = STATE_MOVE_LEFT;
            gTasks[taskId].data[5] = 0;
            break;
        case PWT_TRAINERCARD_INPUT_RIGHT:
            if (gTasks[taskId].tUsingAlternateSlot)
            {
                gBattle_BG0_X = 0;
                gBattle_BG0_Y = 0;
                gBattle_BG1_X = -(DISPLAY_WIDTH + 16);
                gBattle_BG1_Y = 0;
            }
            else
            {
                gBattle_BG0_X = -(DISPLAY_WIDTH + 16);
                gBattle_BG0_Y = 0;
                gBattle_BG1_X = 0;
                gBattle_BG1_Y = 0;
            }

            if (sPWTInfoCard->pos == 1)
            {
                gBattle_BG2_X = 0;
                gBattle_BG2_Y = DISPLAY_HEIGHT;
            }
            else
            {
                gBattle_BG2_X = 0;
                gBattle_BG2_Y = 0;
            }
            matchNo = sIdToPWTMatchNumber[gTasks[taskId2].data[1]][sPWTInfoCard->pos - 1];
            DisplayPWTMatchInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_RIGHT, matchNo);

            for (i = 0; i < NUM_PWT_INFOCARD_SPRITES / 2; i++)
            {
                if (i < 2)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTTrainerIconCardScrollRight;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot ^ 1;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[3] = sPWTInfoCard->spriteIds[i];
                    }
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTMonIconCardScrollRight;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot ^ 1;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                    }
                }
            }
            for (i = NUM_PWT_INFOCARD_SPRITES / 2; i < NUM_PWT_INFOCARD_SPRITES; i++)
            {
                if (i < 16)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTTrainerIconCardScrollRight;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[3] = sPWTInfoCard->spriteIds[i];
                    }
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTMonIconCardScrollRight;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                    }
                }
            }

            gTasks[taskId].tState = STATE_MOVE_RIGHT;
            gTasks[taskId].data[5] = 0;
            break;
        case PWT_MATCHCARD_INPUT_RIGHT:
            if (gTasks[taskId].tUsingAlternateSlot)
            {
                gBattle_BG0_X = 0;
                gBattle_BG0_Y = 0;
                gBattle_BG1_X = -(DISPLAY_WIDTH + 16);
                gBattle_BG1_Y = 0;
            }
            else
            {
                gBattle_BG0_X = -(DISPLAY_WIDTH + 16);
                gBattle_BG0_Y = 0;
                gBattle_BG1_X = 0;
                gBattle_BG1_Y = 0;
            }

            if (sPWTInfoCard->pos == 2)
            {
                gBattle_BG2_X = DISPLAY_WIDTH + 16;
                gBattle_BG2_Y = DISPLAY_HEIGHT;
                trainerTourneyId = sPWTInfoCard->tournamentIds[1];
                DisplayPWTTrainerInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_RIGHT, trainerTourneyId);
            }
            else
            {
                gBattle_BG2_X = 0;
                gBattle_BG2_Y = DISPLAY_HEIGHT;
                matchNo = gTasks[taskId2].data[1] - 16;
                DisplayPWTMatchInfoOnCard(gTasks[taskId].tUsingAlternateSlot | PWT_MOVE_CARD_RIGHT, matchNo);
            }

            for (i = 0; i < NUM_PWT_INFOCARD_SPRITES / 2; i++)
            {
                if (i < 2)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTTrainerIconCardScrollRight;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot ^ 1;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[3] = sPWTInfoCard->spriteIds[i];
                    }
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTMonIconCardScrollRight;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot ^ 1;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                    }
                }
            }
            for (i = NUM_PWT_INFOCARD_SPRITES / 2; i < NUM_PWT_INFOCARD_SPRITES; i++)
            {
                if (i < 16)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTTrainerIconCardScrollRight;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[3] = sPWTInfoCard->spriteIds[i];
                    }
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                    {
                        gSprites[sPWTInfoCard->spriteIds[i]].callback = SpriteCB_PWTMonIconCardScrollRight;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[0] = gTasks[taskId].tUsingAlternateSlot;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[1] = 0;
                        gSprites[sPWTInfoCard->spriteIds[i]].data[2] = i;
                    }
                }
            }

            gTasks[taskId].tState = STATE_MOVE_RIGHT;
            gTasks[taskId].data[5] = 0;
            break;
        }
        break;
    case STATE_MOVE_UP:
        if (++gTasks[taskId].data[5] != 41)
        {
            gBattle_BG0_Y -= 4;
            gBattle_BG1_Y -= 4;
            gBattle_BG2_Y -= 4;
        }
        else
        {
            gTasks[taskId].tState = STATE_GET_INPUT;
        }
        break;
    case STATE_MOVE_DOWN:
        if (++gTasks[taskId].data[5] != 41)
        {
            gBattle_BG0_Y += 4;
            gBattle_BG1_Y += 4;
            gBattle_BG2_Y += 4;
        }
        else
        {
            gTasks[taskId].tState = STATE_GET_INPUT;
        }
        break;
    case STATE_MOVE_LEFT:
        if (++gTasks[taskId].data[5] != 65)
        {
            gBattle_BG0_X -= 4;
            gBattle_BG1_X -= 4;
            gBattle_BG2_X -= 4;
        }
        else
        {
            gTasks[taskId].tState = STATE_GET_INPUT;
        }
        break;
    case STATE_MOVE_RIGHT:
        if (++gTasks[taskId].data[5] != 65)
        {
            gBattle_BG0_X += 4;
            gBattle_BG1_X += 4;
            gBattle_BG2_X += 4;
        }
        else
        {
            gTasks[taskId].tState = STATE_GET_INPUT;
        }
        break;
    case STATE_CLOSE_CARD:
        if (!gPaletteFade.active)
        {
            for (i = 0; i < NUM_PWT_INFOCARD_SPRITES / 2; i++)
            {
                if (i < 2)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                        FreeAndDestroyTrainerPicSprite(sPWTInfoCard->spriteIds[i]);
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                        FreeAndDestroyMonIconSprite(&gSprites[sPWTInfoCard->spriteIds[i]]);
                }
            }
            for (i = NUM_PWT_INFOCARD_SPRITES / 2; i < NUM_PWT_INFOCARD_SPRITES; i++)
            {
                if (i < 16)
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                        FreeAndDestroyTrainerPicSprite(sPWTInfoCard->spriteIds[i]);
                }
                else
                {
                    if (sPWTInfoCard->spriteIds[i] != SPRITE_NONE)
                        FreeAndDestroyMonIconSprite(&gSprites[sPWTInfoCard->spriteIds[i]]);
                }
            }

            FreeMonIconPalettes();
            FREE_AND_SET_NULL(sPWTInfoCard);
            FreeAllWindowBuffers();

            if (mode == PWT_INFOCARD_NEXT_OPPONENT)
            {
                SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
            }
            else
            {
                i = CreateTask(Task_ShowPWTTourneyTree, 0);
                gTasks[i].data[0] = 0;
                gTasks[i].tNotInteractive = FALSE;
                gTasks[i].data[2] = 3;
                gTasks[i].data[3] = gTasks[taskId].data[4];
                gTasks[i].tIsPrevTourneyTree = gTasks[taskId2].data[6];
            }
            DestroyTask(taskId);
        }
        break;
    }
}

// undefine task states for Task_HandlePWTInfoCardInput
#undef STATE_FADE_IN
#undef STATE_WAIT_FADE
#undef STATE_GET_INPUT
#undef STATE_REACT_INPUT
#undef STATE_MOVE_UP
#undef STATE_MOVE_DOWN
#undef STATE_MOVE_LEFT
#undef STATE_MOVE_RIGHT
#undef STATE_CLOSE_CARD

static u8 Task_GetInfoCardInput(u8 taskId)
{
    u8 input = PWT_INFOCARD_INPUT_NONE;
    int taskId2 = gTasks[taskId].data[4];
    int position = gTasks[taskId2].data[1];
    u8 tourneyId = sPWTTourneyTreeTrainerIds[position];
    u16 roundId = gSaveBlock2Ptr->frontier.curChallengeBattleNum;

    if (JOY_NEW(A_BUTTON | B_BUTTON))
        input = PWT_INFOCARD_INPUT_AB;

    // Next opponent card cant scroll
    if (gTasks[taskId].data[3] == PWT_INFOCARD_NEXT_OPPONENT)
        return input;

    if (gTasks[taskId].data[3] == PWT_INFOCARD_TRAINER)
    {
        // For trainer info cards, pos is 0 when on a trainer info card (not viewing that trainer's match progression)
        // Scrolling up/down from a trainer info card goes to other trainer info cards
        if (JOY_NEW(DPAD_UP) && sPWTInfoCard->pos == 0)
        {
            if (position == 0)
                position = PWT_TOURNAMENT_SIZE - 1;
            else
                position--;
            input = PWT_TRAINERCARD_INPUT_UP;
        }
        else if (JOY_NEW(DPAD_DOWN) && sPWTInfoCard->pos == 0)
        {
            if (position == PWT_TOURNAMENT_SIZE - 1)
                position = 0;
            else
                position++;
            input = PWT_TRAINERCARD_INPUT_DOWN;
        }
        // Scrolling left can only be done after scrolling right
        else if (JOY_NEW(DPAD_LEFT) && sPWTInfoCard->pos != 0)
        {
            sPWTInfoCard->pos--;
            input = PWT_TRAINERCARD_INPUT_LEFT;
        }
        // Scrolling right from a trainer info card shows their match progression
        else if (JOY_NEW(DPAD_RIGHT))
        {
            // Can only scroll right from a trainer card until the round they were eliminated
            if (PWT_TRAINERS[tourneyId].isEliminated && sPWTInfoCard->pos - 1 < PWT_TRAINERS[tourneyId].eliminatedAt)
            {
                sPWTInfoCard->pos++;
                input = PWT_TRAINERCARD_INPUT_RIGHT;
            }
            // otherwise can scroll as far right as the current round allows
            if (!PWT_TRAINERS[tourneyId].isEliminated && sPWTInfoCard->pos - 1 < roundId)
            {
                sPWTInfoCard->pos++;
                input = PWT_TRAINERCARD_INPUT_RIGHT;
            }
        }

        if (input == PWT_INFOCARD_INPUT_AB)
        {
            if (sPWTInfoCard->pos != 0)
                gTasks[taskId2].data[1] = sTrainerAndRoundToLastPWTMatchCardNum[position / 2][sPWTInfoCard->pos - 1];
            else
                gTasks[taskId2].data[1] = position;
        }
    }
    else // gTasks[taskId].data[3] == PWT_INFOCARD_MATCH
    {
        // For match info cards, pos is 1 when on the match card, 0 when on the left trainer, and 1 when on the right trainer
        // Scrolling up/down from a match info card goes to the next/previous match
        if (JOY_NEW(DPAD_UP) && sPWTInfoCard->pos == 1)
        {
            if (position == PWT_TOURNAMENT_SIZE)
                position = sLastPWTMatchCardNum[roundId];
            else
                position--;
            input = PWT_MATCHCARD_INPUT_UP;
        }
        else if (JOY_NEW(DPAD_DOWN) && sPWTInfoCard->pos == 1)
        {
            if (position == sLastPWTMatchCardNum[roundId])
                position = PWT_TOURNAMENT_SIZE;
            else
                position++;
            input = PWT_MATCHCARD_INPUT_DOWN;
        }
        // Scrolling left/right from a match info card shows the trainer info card of the competitors for that match
        else if (JOY_NEW(DPAD_LEFT) && sPWTInfoCard->pos != 0)
        {
            input = PWT_MATCHCARD_INPUT_LEFT;
            sPWTInfoCard->pos--;
        }
        else if (JOY_NEW(DPAD_RIGHT) && (sPWTInfoCard->pos == 0 || sPWTInfoCard->pos == 1))
        {
            input = PWT_MATCHCARD_INPUT_RIGHT;
            sPWTInfoCard->pos++;
        }

        if (input == PWT_INFOCARD_INPUT_AB)
        {
            if (sPWTInfoCard->pos == 0) // On left trainer info card
                gTasks[taskId2].data[1] = sPWTTournamentIdToPairedTrainerIds[sPWTInfoCard->tournamentIds[0]];
            else if (sPWTInfoCard->pos == 2) // On right trainer info card
                gTasks[taskId2].data[1] = sPWTTournamentIdToPairedTrainerIds[sPWTInfoCard->tournamentIds[1]];
            else // On match info card
                gTasks[taskId2].data[1] = position;
        }
    }

    if (input != PWT_INFOCARD_INPUT_NONE && input != PWT_INFOCARD_INPUT_AB)
    {
        PlaySE(SE_SELECT);
        gTasks[taskId2].data[1] = position;
        gTasks[taskId].tUsingAlternateSlot ^= 1;
    }

    return input;
}

#undef tUsingAlternateSlot

static void DisplayPWTTrainerInfoOnCard(u8 flags, u8 trainerTourneyId)
{
    struct TextPrinterTemplate textPrinter;
    int i, j;
    int trainerId = 0;
    int arrId = 0;
    int windowId = WIN_PWT_TRAINER_NAME;
    int x = 0, y = 0;
    u8 palSlot = 0;
    u32 trainerPic = GetPlayerTrainerPicIdByOutfitGenderType(gSaveBlock2Ptr->currOutfitId, gSaveBlock2Ptr->playerGender, 0);
    trainerId = PWT_TRAINERS[trainerTourneyId].trainerId;

    if (flags & PWT_CARD_ALTERNATE_SLOT)
        arrId = 2 * (PARTY_SIZE + 1), windowId = WIN_PWT_TRAINER_NAME + NUM_PWT_INFO_CARD_WINDOWS, palSlot = 2;
    if (flags & PWT_MOVE_CARD_RIGHT)
        x = DISPLAY_WIDTH + 16;
    if (flags & PWT_MOVE_CARD_DOWN)
        y = DISPLAY_HEIGHT;
    if (flags & PWT_MOVE_CARD_LEFT)
        x = -(DISPLAY_WIDTH + 16);
    if (flags & PWT_MOVE_CARD_UP)
        y = -DISPLAY_HEIGHT;

    // Create trainer pic sprite
    if (trainerId == TRAINER_PLAYER)
        sPWTInfoCard->spriteIds[arrId] = CreateTrainerPicSprite(trainerPic, TRUE, x + 48, y + 64, palSlot + 12, TAG_NONE);
    else
        sPWTInfoCard->spriteIds[arrId] = CreateTrainerPicSprite(GetPWTTrainerFrontSpriteId(trainerId), TRUE, x + 48, y + 64, palSlot + 12, TAG_NONE);

    if (flags & PWT_MOVE_CARD)
        gSprites[sPWTInfoCard->spriteIds[arrId]].invisible = TRUE;

    // Create party mon icons
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (trainerId == TRAINER_PLAYER)
        {
            sPWTInfoCard->spriteIds[2 + i + arrId] = CreateMonIcon(PWT_MONS[trainerTourneyId][i],
                                                                  SpriteCB_MonIconPWTInfo,
                                                                  x | sInfoPWTTrainerMonX[i],
                                                                  y + sInfoPWTTrainerMonY[i],
                                                                  0, 0);
            gSprites[sPWTInfoCard->spriteIds[2 + i + arrId]].oam.priority = 0;
        }
        else
        {
            sPWTInfoCard->spriteIds[2 + i + arrId] = CreateMonIcon(gPWTFacilityTrainerMons[PWT_MONS[trainerTourneyId][i]].species,
                                                                  SpriteCB_MonIconPWTInfo,
                                                                  x | sInfoPWTTrainerMonX[i],
                                                                  y + sInfoPWTTrainerMonY[i],
                                                                  0, 0);
            gSprites[sPWTInfoCard->spriteIds[2 + i + arrId]].oam.priority = 0;
        }

        if (flags & PWT_MOVE_CARD)
            gSprites[sPWTInfoCard->spriteIds[2 + i + arrId]].invisible = TRUE;
    }

    // Initialize the text printer
    textPrinter.fontId = FONT_SHORT;
    textPrinter.x = 0;
    textPrinter.y = 0;
    textPrinter.currentX = textPrinter.x;
    textPrinter.currentY = textPrinter.y;
    textPrinter.letterSpacing = 2;
    textPrinter.lineSpacing = 0;
    textPrinter.unk = 0;
    textPrinter.fgColor = TEXT_DYNAMIC_COLOR_5;
    textPrinter.bgColor = TEXT_COLOR_TRANSPARENT;
    textPrinter.shadowColor = TEXT_DYNAMIC_COLOR_4;

    // Get class and trainer name
    i = 0;
    if (trainerId == TRAINER_PLAYER)
        j = gFacilityClassToTrainerClass[FACILITY_CLASS_BRENDAN];
    else
        j = GetPWTOpponentClass(trainerId);

    for (;gTrainerClasses[j].name[i] != EOS; i++)
        gStringVar1[i] = gTrainerClasses[j].name[i];
    gStringVar1[i] = CHAR_SPACE;
    gStringVar1[i + 1] = EOS;

    if (trainerId == TRAINER_PLAYER)
    {
        StringAppend(gStringVar1, gSaveBlock2Ptr->playerName);
    }
    else
    {
        CopyPWTTrainerName(gStringVar2, trainerId);
        StringAppend(gStringVar1, gStringVar2);
    }

    // Print class and trainer name
    textPrinter.currentX = GetStringCenterAlignXOffsetWithLetterSpacing(textPrinter.fontId, gStringVar1, 0xD0, textPrinter.letterSpacing);
    textPrinter.currentChar = gStringVar1;
    textPrinter.windowId = windowId;
    PutWindowTilemap(windowId);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    AddTextPrinter(&textPrinter, 0, NULL);
    textPrinter.letterSpacing = 0;

    // Removed printing species
    textPrinter.fontId = FONT_SHORT;

    PutWindowTilemap(windowId + WIN_PWT_TRAINER_FLAVOR_TEXT);
    CopyWindowToVram(windowId + WIN_PWT_TRAINER_FLAVOR_TEXT, COPYWIN_FULL);

    // Print text about trainers potential in the tourney
    if (trainerId == TRAINER_PLAYER)
        textPrinter.currentChar = COMPOUND_STRING("Player Line 1");
    else
        textPrinter.currentChar = gPWTFacilityTrainers[trainerId].battleData1;

    textPrinter.fontId = FONT_NORMAL;
    textPrinter.windowId = windowId + WIN_PWT_TRAINER_FLAVOR_TEXT;
    textPrinter.currentX = 0;
    textPrinter.y = 4;
    textPrinter.currentY = 4;
    AddTextPrinter(&textPrinter, 0, NULL);

    // Print the trainers battle style
    if (trainerId == TRAINER_PLAYER)
        textPrinter.currentChar = COMPOUND_STRING("Player Line 2");
    else
        textPrinter.currentChar = gPWTFacilityTrainers[trainerId].battleData2;
    textPrinter.y = 20;
    textPrinter.currentY = 20;
    AddTextPrinter(&textPrinter, 0, NULL);

    // Print the stat text
    if (trainerId == TRAINER_PLAYER)
        textPrinter.currentChar = COMPOUND_STRING("Player Line 3");
    else
        textPrinter.currentChar = gPWTFacilityTrainers[trainerId].battleData3;
    textPrinter.y = 36;
    textPrinter.currentY = 36;
    AddTextPrinter(&textPrinter, 0, NULL);
}

static int BufferPWTWinString(u8 matchNum, u8 *tournamentIds)
{
    int i;
    u8 tournamentId;
    int winStringId = 0;
    int count = 0;

    // Get winners name
    for (i = sPWTCompetitorRangeByMatch[matchNum][0]; i < sPWTCompetitorRangeByMatch[matchNum][0] + sPWTCompetitorRangeByMatch[matchNum][1]; i++)
    {
        tournamentId = sPWTTourneyTreeTrainerIds2[i];
        if (!PWT_TRAINERS[tournamentId].isEliminated)
        {
            tournamentIds[count] = tournamentId;
            if (PWT_TRAINERS[tournamentId].trainerId == TRAINER_PLAYER)
                StringCopy(gStringVar1, gSaveBlock2Ptr->playerName);
            else
                CopyPWTTrainerName(gStringVar1, PWT_TRAINERS[tournamentId].trainerId);
            count++;
        }
    }

    // Neither trainer has been eliminated, battle hasn't occurred yet
    if (count == 2)
        return PWT_TEXT_NO_WINNER_YET;

    for (i = sPWTCompetitorRangeByMatch[matchNum][0]; i < sPWTCompetitorRangeByMatch[matchNum][0] + sPWTCompetitorRangeByMatch[matchNum][1]; i++)
    {
        tournamentId = sPWTTourneyTreeTrainerIds2[i];

        if (PWT_TRAINERS[tournamentId].isEliminated
            && PWT_TRAINERS[tournamentId].eliminatedAt >= sPWTCompetitorRangeByMatch[matchNum][2])
        {
            tournamentIds[count] = tournamentId;
            count++;

            if (PWT_TRAINERS[tournamentId].eliminatedAt == sPWTCompetitorRangeByMatch[matchNum][2])
            {
                // Set initial winStringId offset
                StringCopy(gStringVar2, GetMoveName(gSaveBlock2Ptr->frontier.domeWinningMoves[tournamentId]));
                winStringId = PWT_TRAINERS[tournamentId].forfeited * 2; // (DOME_TEXT_WON_USING_MOVE - 1) or (DOME_TEXT_WON_ON_FORFEIT - 1)

                if (gSaveBlock2Ptr->frontier.domeWinningMoves[tournamentId] == MOVE_NONE && PWT_TRAINERS[tournamentId].forfeited == FALSE)
                    winStringId = PWT_TEXT_WON_NO_MOVES - 1;
            }
            else
            {
                if (PWT_TRAINERS[tournamentId].trainerId == TRAINER_PLAYER)
                    StringCopy(gStringVar1, gSaveBlock2Ptr->playerName);
                else
                    CopyPWTTrainerName(gStringVar1, PWT_TRAINERS[tournamentId].trainerId);
            }
        }

        if (count == 2)
            break;
    }

    if (matchNum == PWT_MATCHES_COUNT - 1)
        return winStringId + 2; // use DOME_TEXT_CHAMP_*
    else
        return winStringId + 1; // use DOME_TEXT_WON_*
}

static void DisplayPWTMatchInfoOnCard(u8 flags, u8 matchNo)
{
    struct TextPrinterTemplate textPrinter;
    int tournamentIds[2];
    int trainerIds[2];
    bool32 lost[2];
    int i;
    int winStringId = 0;
    int arrId = 0;
    int windowId = 0;
    int x = 0, y = 0;
    u8 palSlot = 0;
    u16 picId = GetPlayerTrainerPicIdByOutfitGenderType(gSaveBlock2Ptr->currOutfitId, gSaveBlock2Ptr->playerGender, 0);

    if (flags & PWT_CARD_ALTERNATE_SLOT)
        arrId = 2 * (PARTY_SIZE + 1), windowId = NUM_PWT_INFO_CARD_WINDOWS, palSlot = 2;
    if (flags & PWT_MOVE_CARD_RIGHT)
        x = DISPLAY_WIDTH + 16;
    if (flags & PWT_MOVE_CARD_DOWN)
        y = DISPLAY_HEIGHT;
    if (flags & PWT_MOVE_CARD_LEFT)
        x = -(DISPLAY_WIDTH + 16);
    if (flags & PWT_MOVE_CARD_UP)
        y = -DISPLAY_HEIGHT;

    // Copy trainers information to handy arrays.
    winStringId = BufferPWTWinString(matchNo, sPWTInfoCard->tournamentIds);
    for (i = 0; i < NUM_PWT_INFOCARD_TRAINERS; i++)
    {
        tournamentIds[i] = sPWTInfoCard->tournamentIds[i];
        trainerIds[i] = PWT_TRAINERS[tournamentIds[i]].trainerId;
        if (PWT_TRAINERS[tournamentIds[i]].eliminatedAt <= sPWTCompetitorRangeByMatch[matchNo][2]
            && PWT_TRAINERS[tournamentIds[i]].isEliminated)
            lost[i] = TRUE;
        else
            lost[i] = FALSE;
    }

    // Draw left trainer sprite.
    if (trainerIds[0] == TRAINER_PLAYER)
        sPWTInfoCard->spriteIds[arrId] = CreateTrainerPicSprite(picId, TRUE, x + 48, y + 88, palSlot + 12, TAG_NONE);
    else
        sPWTInfoCard->spriteIds[arrId] = CreateTrainerPicSprite(GetPWTTrainerFrontSpriteId(trainerIds[0]), TRUE, x + 48, y + 88, palSlot + 12, TAG_NONE);

    if (flags & PWT_MOVE_CARD)
        gSprites[sPWTInfoCard->spriteIds[arrId]].invisible = TRUE;
    if (lost[0])
        gSprites[sPWTInfoCard->spriteIds[arrId]].oam.paletteNum = 3;

    // Draw right trainer sprite.
    if (trainerIds[1] == TRAINER_PLAYER)
        sPWTInfoCard->spriteIds[1 + arrId] = CreateTrainerPicSprite(picId, TRUE, x + 192, y + 88, palSlot + 13, TAG_NONE);
    else
        sPWTInfoCard->spriteIds[1 + arrId] = CreateTrainerPicSprite(GetPWTTrainerFrontSpriteId(trainerIds[1]), TRUE, x + 192, y + 88, palSlot + 13, TAG_NONE);

    if (flags & PWT_MOVE_CARD)
        gSprites[sPWTInfoCard->spriteIds[1 + arrId]].invisible = TRUE;
    if (lost[1])
        gSprites[sPWTInfoCard->spriteIds[1 + arrId]].oam.paletteNum = 3;

    // Draw left trainer's Pokémon icons.
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (trainerIds[0] == TRAINER_PLAYER)
        {
            sPWTInfoCard->spriteIds[2 + i + arrId] = CreateMonIcon(PWT_MONS[tournamentIds[0]][i],
                                                                  SpriteCB_MonIconPWTInfo,
                                                                  x | sLeftPWTTrainerMonX[i],
                                                                  y + sLeftPWTTrainerMonY[i],
                                                                  0, 0);
            gSprites[sPWTInfoCard->spriteIds[2 + i + arrId]].oam.priority = 0;
        }
        else
        {
            sPWTInfoCard->spriteIds[2 + i + arrId] = CreateMonIcon(gPWTFacilityTrainerMons[PWT_MONS[tournamentIds[0]][i]].species,
                                                                  SpriteCB_MonIconPWTInfo,
                                                                  x | sLeftPWTTrainerMonX[i],
                                                                  y + sLeftPWTTrainerMonY[i],
                                                                  0, 0);
            gSprites[sPWTInfoCard->spriteIds[2 + i + arrId]].oam.priority = 0;
        }

        if (flags & PWT_MOVE_CARD)
            gSprites[sPWTInfoCard->spriteIds[2 + i + arrId]].invisible = TRUE;
        if (lost[0])
        {
            gSprites[sPWTInfoCard->spriteIds[2 + i + arrId]].oam.paletteNum = 3;
            gSprites[sPWTInfoCard->spriteIds[2 + i + arrId]].sPWTMonIconStill = TRUE;
        }
    }

    // Draw right trainer's Pokémon icons.
    // TODO - above and below, get rid of hte +2 or +8 magic numbers
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (trainerIds[1] == TRAINER_PLAYER)
        {
            sPWTInfoCard->spriteIds[8 + i + arrId] = CreateMonIcon(PWT_MONS[tournamentIds[1]][i],
                                                                  SpriteCB_MonIconPWTInfo,
                                                                  x | sRightPWTTrainerMonX[i],
                                                                  y + sRightPWTTrainerMonY[i],
                                                                  0, 0);
            gSprites[sPWTInfoCard->spriteIds[8 + i + arrId]].oam.priority = 0;
        }
        else
        {
            sPWTInfoCard->spriteIds[8 + i + arrId] = CreateMonIcon(gPWTFacilityTrainerMons[PWT_MONS[tournamentIds[1]][i]].species,
                                                                  SpriteCB_MonIconPWTInfo,
                                                                  x | sRightPWTTrainerMonX[i],
                                                                  y + sRightPWTTrainerMonY[i],
                                                                  0, 0);
            gSprites[sPWTInfoCard->spriteIds[8 + i + arrId]].oam.priority = 0;
        }

        if (flags & PWT_MOVE_CARD)
            gSprites[sPWTInfoCard->spriteIds[8 + i + arrId]].invisible = TRUE;
        if (lost[1])
        {
            gSprites[sPWTInfoCard->spriteIds[8 + i + arrId]].oam.paletteNum = 3;
            gSprites[sPWTInfoCard->spriteIds[8 + i + arrId]].sPWTMonIconStill = TRUE;
        }
    }

    // Print the win string (or 'Let the battle begin!').
    textPrinter.x = 0;
    textPrinter.y = 2;
    textPrinter.currentX = textPrinter.x;
    textPrinter.currentY = textPrinter.y;
    textPrinter.letterSpacing = 0;
    textPrinter.lineSpacing = 0;
    textPrinter.unk = 0;
    textPrinter.fgColor = TEXT_DYNAMIC_COLOR_5;
    textPrinter.bgColor = TEXT_COLOR_TRANSPARENT;
    textPrinter.shadowColor = TEXT_DYNAMIC_COLOR_4;
    StringExpandPlaceholders(gStringVar4, sBattlePWTWinTexts[winStringId]);
    textPrinter.currentChar = gStringVar4;
    textPrinter.windowId = windowId + WIN_PWT_MATCH_WIN_TEXT;
    textPrinter.fontId = FONT_NORMAL;
    PutWindowTilemap(windowId + WIN_PWT_MATCH_WIN_TEXT);
    CopyWindowToVram(windowId + WIN_PWT_MATCH_WIN_TEXT, COPYWIN_FULL);
    textPrinter.currentX = 0;
    textPrinter.currentY = textPrinter.y = 0;
    AddTextPrinter(&textPrinter, 0, NULL);

    // Print left trainer's name.
    if (trainerIds[0] == TRAINER_PLAYER)
        StringCopy(gStringVar1, gSaveBlock2Ptr->playerName);
    else
        CopyPWTTrainerName(gStringVar1, trainerIds[0]);

    textPrinter.fontId = FONT_SHORT;
    textPrinter.letterSpacing = 2;
    textPrinter.currentChar = gStringVar1;
    textPrinter.windowId = windowId + WIN_PWT_MATCH_TRAINER_NAME_LEFT;
    textPrinter.currentX = GetStringCenterAlignXOffsetWithLetterSpacing(textPrinter.fontId, textPrinter.currentChar, 0x40, textPrinter.letterSpacing);
    textPrinter.currentY = textPrinter.y = 2;
    PutWindowTilemap(windowId + WIN_PWT_MATCH_TRAINER_NAME_LEFT);
    CopyWindowToVram(windowId + WIN_PWT_MATCH_TRAINER_NAME_LEFT, COPYWIN_FULL);
    AddTextPrinter(&textPrinter, 0, NULL);

    // Print right trainer's name.
    if (trainerIds[1] == TRAINER_PLAYER)
        StringCopy(gStringVar1, gSaveBlock2Ptr->playerName);
    else
        CopyPWTTrainerName(gStringVar1, trainerIds[1]);

    textPrinter.currentChar = gStringVar1;
    textPrinter.windowId = windowId + WIN_PWT_MATCH_TRAINER_NAME_RIGHT;
    textPrinter.currentX = GetStringCenterAlignXOffsetWithLetterSpacing(textPrinter.fontId, textPrinter.currentChar, 0x40, textPrinter.letterSpacing);
    textPrinter.currentY = textPrinter.y = 2;
    PutWindowTilemap(windowId + WIN_PWT_MATCH_TRAINER_NAME_RIGHT);
    CopyWindowToVram(windowId + WIN_PWT_MATCH_TRAINER_NAME_RIGHT, COPYWIN_FULL);
    AddTextPrinter(&textPrinter, 0, NULL);

    // Print match number.
    textPrinter.letterSpacing = 0;
    textPrinter.currentChar = sPWTMatchNumberTexts[matchNo];
    textPrinter.windowId = windowId + WIN_PWT_MATCH_NUMBER;
    textPrinter.currentX = GetStringCenterAlignXOffsetWithLetterSpacing(textPrinter.fontId, textPrinter.currentChar, 0xA0, textPrinter.letterSpacing);
    textPrinter.currentY = textPrinter.y = 2;
    PutWindowTilemap(windowId + WIN_PWT_MATCH_NUMBER);
    CopyWindowToVram(windowId + WIN_PWT_MATCH_NUMBER, COPYWIN_FULL);
    AddTextPrinter(&textPrinter, 0, NULL);
}

static void ShowPWTTourneyTree(void)
{
    u8 taskId = CreateTask(Task_ShowPWTTourneyTree, 0);
    gTasks[taskId].tState = 0;
    gTasks[taskId].tNotInteractive = FALSE;
    gTasks[taskId].data[2] = 2;
    gTasks[taskId].tIsPrevTourneyTree = FALSE;
    SetMainCallback2(CB2_PWTTourneyTree);
}

// Show the last tournament on the PC
static void ShowPreviousPWTTourneyTree(void)
{
    u8 taskId;

    SetPWTTrainerAndMonPtrs();
    gSaveBlock2Ptr->pwt.roundNum = PWT_FINAL;
    taskId = CreateTask(Task_ShowPWTTourneyTree, 0);
    gTasks[taskId].tState = 0;
    gTasks[taskId].tNotInteractive = FALSE;
    gTasks[taskId].data[2] = 2;
    gTasks[taskId].tIsPrevTourneyTree = TRUE;
    SetMainCallback2(CB2_PWTTourneyTree);
}

// Task states for Task_HandlePWTTourneyTreeInput
#define STATE_FADE_IN                   0
#define STATE_WAIT_FADE                 1
#define STATE_GET_INPUT                 2
#define STATE_SHOW_PWT_INFOCARD_TRAINER 3
#define STATE_SHOW_PWT_INFOCARD_MATCH   5
#define STATE_CLOSE_TOURNEY_TREE        7

static void Task_HandlePWTTourneyTreeInput(u8 taskId)
{
    u8 newTaskId = 0;
    int spriteId = gTasks[taskId].data[1];

    switch (gTasks[taskId].tState)
    {
    case STATE_FADE_IN:
        if (!gPaletteFade.active)
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
            gTasks[taskId].tState = STATE_WAIT_FADE;
            StartSpriteAnim(&gSprites[spriteId], 1);
        }
        break;
    case STATE_WAIT_FADE:
        if (!gPaletteFade.active)
            gTasks[taskId].tState = STATE_GET_INPUT;
        break;
    case STATE_GET_INPUT:
        switch (UpdatePWTTourneyTreeCursor(taskId))
        {
        case PWT_TOURNEY_TREE_SELECTED_CLOSE:
        default:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
            gTasks[taskId].tState = STATE_CLOSE_TOURNEY_TREE;
            break;
        case PWT_TOURNEY_TREE_NO_SELECTION:
            break;
        case PWT_TOURNEY_TREE_SELECTED_TRAINER:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
            gTasks[taskId].tState = STATE_SHOW_PWT_INFOCARD_TRAINER;
            break;
        case PWT_TOURNEY_TREE_SELECTED_MATCH:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
            gTasks[taskId].tState = STATE_SHOW_PWT_INFOCARD_MATCH;
            break;
        }
        break;
    case STATE_SHOW_PWT_INFOCARD_TRAINER:
        if (!gPaletteFade.active)
        {
            FreeAllWindowBuffers();
            ScanlineEffect_Stop();
            FREE_AND_SET_NULL(sTilemapBuffer);
            newTaskId = CreateTask(Task_ShowPWTTourneyInfoCard, 0);
            gTasks[newTaskId].tState = 0;
            gTasks[newTaskId].tTournamentId = sPWTTourneyTreeTrainerIds[spriteId];
            gTasks[newTaskId].tMode = PWT_INFOCARD_TRAINER;
            gTasks[newTaskId].tPrevTaskId = taskId;

            gTasks[taskId].tState = STATE_SHOW_PWT_INFOCARD_TRAINER + 1;
            sPWTInfoCard->pos = 0;
        }
        break;
    case STATE_SHOW_PWT_INFOCARD_TRAINER + 1:
        break;
    case STATE_SHOW_PWT_INFOCARD_MATCH:
        if (!gPaletteFade.active)
        {
            FreeAllWindowBuffers();
            ScanlineEffect_Stop();
            FREE_AND_SET_NULL(sTilemapBuffer);
            newTaskId = CreateTask(Task_ShowPWTTourneyInfoCard, 0);
            gTasks[newTaskId].tState = 0;
            gTasks[newTaskId].tTournamentId = spriteId - PWT_TOURNAMENT_SIZE;
            gTasks[newTaskId].tMode = PWT_INFOCARD_MATCH;
            gTasks[newTaskId].tPrevTaskId = taskId;

            gTasks[taskId].tState = STATE_SHOW_PWT_INFOCARD_MATCH + 1;
        }
        break;
    case STATE_SHOW_PWT_INFOCARD_MATCH + 1:
        break;
    case STATE_CLOSE_TOURNEY_TREE:
        if (!gPaletteFade.active)
        {
            FreeAllWindowBuffers();
            ScanlineEffect_Stop();
            FREE_AND_SET_NULL(sTilemapBuffer);
            SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
            DestroyTask(gTasks[taskId].data[7]);
            DestroyTask(taskId);
        }
        break;
    }
}

// undefine task states for Task_HandlePWTTourneyTreeInput
#undef STATE_FADE_IN
#undef STATE_WAIT_FADE
#undef STATE_GET_INPUT
#undef STATE_SHOW_PWT_INFOCARD_TRAINER
#undef STATE_SHOW_PWT_INFOCARD_MATCH
#undef STATE_CLOSE_TOURNEY_TREE

#define MOVE_DIR_UP    0
#define MOVE_DIR_DOWN  1
#define MOVE_DIR_LEFT  2
#define MOVE_DIR_RIGHT 3
#define MOVE_DIR_NONE  4

// Move the tourney tree cursor
// The 'cursor' is actually just which button sprite is currently doing the 'selected' animation
static u8 UpdatePWTTourneyTreeCursor(u8 taskId)
{
    u8 selection = PWT_TOURNEY_TREE_NO_SELECTION;
    int direction = MOVE_DIR_NONE;
    int tourneyTreeCursorSpriteId = gTasks[taskId].data[1];
    int roundId = gSaveBlock2Ptr->frontier.curChallengeBattleNum;

    if (gMain.newKeys == B_BUTTON || (JOY_NEW(A_BUTTON) && tourneyTreeCursorSpriteId == PWT_TOURNEY_TREE_CLOSE_BUTTON))
    {
        PlaySE(SE_SELECT);
        selection = PWT_TOURNEY_TREE_SELECTED_CLOSE;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (tourneyTreeCursorSpriteId < PWT_TOURNAMENT_SIZE)
        {
            PlaySE(SE_SELECT);
            selection = PWT_TOURNEY_TREE_SELECTED_TRAINER;
        }
        else
        {
            PlaySE(SE_SELECT);
            selection = PWT_TOURNEY_TREE_SELECTED_MATCH;
        }
    }
    else
    {
        if (gMain.newKeys == DPAD_UP && sPWTTourneyTreeCursorMovementMap[tourneyTreeCursorSpriteId][roundId][0] != 0xFF)
            direction = MOVE_DIR_UP;
        else if (gMain.newKeys == DPAD_DOWN && sPWTTourneyTreeCursorMovementMap[tourneyTreeCursorSpriteId][roundId][1] != 0xFF)
            direction = MOVE_DIR_DOWN;
        else if (gMain.newKeys == DPAD_LEFT && sPWTTourneyTreeCursorMovementMap[tourneyTreeCursorSpriteId][roundId][2] != 0xFF)
            direction = MOVE_DIR_LEFT;
        else if (gMain.newKeys == DPAD_RIGHT && sPWTTourneyTreeCursorMovementMap[tourneyTreeCursorSpriteId][roundId][3] != 0xFF)
            direction = MOVE_DIR_RIGHT;
    }

    if (direction != MOVE_DIR_NONE)
    {
        PlaySE(SE_SELECT);
        StartSpriteAnim(&gSprites[tourneyTreeCursorSpriteId], 0); // Do unselected sprite anim
        tourneyTreeCursorSpriteId = sPWTTourneyTreeCursorMovementMap[tourneyTreeCursorSpriteId][roundId][direction];
        StartSpriteAnim(&gSprites[tourneyTreeCursorSpriteId], 1); // Do selected sprite anim
        gTasks[taskId].data[1] = tourneyTreeCursorSpriteId;
    }

    return selection;
}

#undef MOVE_DIR_UP
#undef MOVE_DIR_DOWN
#undef MOVE_DIR_LEFT
#undef MOVE_DIR_RIGHT
#undef MOVE_DIR_NONE

// Shows the results of the just-completed round for the current tourney
static void ShowNonInteractivePWTTourneyTree(void)
{
    u8 taskId = CreateTask(Task_ShowPWTTourneyTree, 0);
    gTasks[taskId].tState = 0;
    gTasks[taskId].tNotInteractive = TRUE;
    gTasks[taskId].data[2] = 2;
    gTasks[taskId].tIsPrevTourneyTree = FALSE;
    SetMainCallback2(CB2_PWTTourneyTree);
}

// // TODO - implement function
// static void ResolvePWTRoundWinners(void)
// {
//     int i;

//     if (gSpecialVar_0x8005 == DOME_PLAYER_WON_MATCH)
//     {
//         PWT_TRAINERS[TrainerIdToPWTTournamentId(TRAINER_BATTLE_PARAM.opponentA)].isEliminated = TRUE;
//         PWT_TRAINERS[TrainerIdToPWTTournamentId(TRAINER_BATTLE_PARAM.opponentA)].eliminatedAt = gSaveBlock2Ptr->frontier.curChallengeBattleNum;
//         gSaveBlock2Ptr->frontier.domeWinningMoves[TrainerIdToPWTTournamentId(TRAINER_BATTLE_PARAM.opponentA)] = gBattleResults.lastUsedMovePlayer;

//         // If the player's match was the final one, no NPC vs NPC matches to decide
//         if (gSaveBlock2Ptr->frontier.curChallengeBattleNum < PWT_FINAL)
//             DecidePWTRoundWinners(gSaveBlock2Ptr->frontier.curChallengeBattleNum);
//     }
//     else // DOME_PLAYER_LOST_MATCH or DOME_PLAYER_RETIRED
//     {
//         PWT_TRAINERS[TrainerIdToPWTTournamentId(TRAINER_PLAYER)].isEliminated = TRUE;
//         PWT_TRAINERS[TrainerIdToPWTTournamentId(TRAINER_PLAYER)].eliminatedAt = gSaveBlock2Ptr->frontier.curChallengeBattleNum;
//         gSaveBlock2Ptr->frontier.domeWinningMoves[TrainerIdToPWTTournamentId(TRAINER_PLAYER)] = gBattleResults.lastUsedMoveOpponent;

//         if (gBattleOutcome == B_OUTCOME_FORFEITED || gSpecialVar_0x8005 == DOME_PLAYER_RETIRED)
//             PWT_TRAINERS[TrainerIdToPWTTournamentId(TRAINER_PLAYER)].forfeited = TRUE;

//         // Player lost, decide remaining outcome of tournament
//         for (i = gSaveBlock2Ptr->frontier.curChallengeBattleNum; i < PWT_ROUNDS_COUNT; i++)
//             DecidePWTRoundWinners(i);
//     }
// }

// // TODO - implement function
// // Decides the winning move of an NPC vs NPC match
// static u16 GetWinningMove(int winnerTournamentId, int loserTournamentId, u8 roundId)
// {
//     int i, j, k;
//     int moveScores[MAX_MON_MOVES * PARTY_SIZE];
//     u16 moves[MAX_MON_MOVES * PARTY_SIZE];
//     u16 bestScore = 0;
//     u16 bestId = 0;
//     int movePower = 0;
//     SetFacilityPtrsGetLevel();

//     // Calc move points of all 4 moves for all 3 Pokémon hitting all 3 target mons.
//     for (i = 0; i < PARTY_SIZE; i++)
//     {
//         for (j = 0; j < MAX_MON_MOVES; j++)
//         {
//             // TODO: Clean this up, looks like a different data structure (2D array)
//             moveScores[i * MAX_MON_MOVES + j] = 0;
//             else
//                 moves[i * MAX_MON_MOVES + j] = gPWTFacilityTrainerMons[PWT_MONS[winnerTournamentId][i]].moves[j];

//             movePower = GetMovePower(moves[i * MAX_MON_MOVES + j]);
//             enum BattleMoveEffects effect = GetMoveEffect(moves[i * MAX_MON_MOVES + j]);
//             if (IsBattleMoveStatus(moves[i * MAX_MON_MOVES + j]))
//                 movePower = 40;
//             else if (movePower == 1)
//                 movePower = 60;
//             else if (B_EXPLOSION_DEFENSE < GEN_5
//                 && (effect == EFFECT_EXPLOSION || EFFECT_MISTY_EXPLOSION))
//                 movePower /= 2;

//             for (k = 0; k < PARTY_SIZE; k++)
//             {
//                 u32 personality = 0;
//                 u32 targetSpecies = 0;
//                 enum Ability targetAbility = 0;
//                 uq4_12_t typeMultiplier = 0;
//                 do
//                 {
//                     personality = Random32();
//                 } while (gPWTFacilityTrainerMons[PWT_MONS[loserTournamentId][k]].nature != GetNatureFromPersonality(personality));

//                 targetSpecies = gPWTFacilityTrainerMons[PWT_MONS[loserTournamentId][k]].species;

//                 if (personality & 1)
//                     targetAbility = GetSpeciesAbility(targetSpecies, 1);
//                 else
//                     targetAbility = GetSpeciesAbility(targetSpecies, 0);

//                 typeMultiplier = CalcPartyMonTypeEffectivenessMultiplier(moves[i * 4 + j], targetSpecies, targetAbility);
//                 if (typeMultiplier == UQ_4_12(0))
//                     moveScores[i * MAX_MON_MOVES + j] += 0;
//                 else if (typeMultiplier >= UQ_4_12(2))
//                     moveScores[i * MAX_MON_MOVES + j] += movePower * 2;
//                 else if (typeMultiplier <= UQ_4_12(0.5))
//                     moveScores[i * MAX_MON_MOVES + j] += movePower / 2;
//                 else
//                     moveScores[i * MAX_MON_MOVES + j] += movePower;
//             }

//             if (bestScore < moveScores[i * MAX_MON_MOVES + j])
//             {
//                 bestId = i * MAX_MON_MOVES + j;
//                 bestScore = moveScores[i * MAX_MON_MOVES + j];
//             }
//             else if (bestScore == moveScores[i * MAX_MON_MOVES + j])
//             {
//                 if (moves[bestId] < moves[i * MAX_MON_MOVES + j]) // Why not use (Random() & 1) instead of promoting moves with a higher id?
//                     bestId = i * MAX_MON_MOVES + j;
//             }
//         }
//     }

//     j = bestId;
//     do
//     {
//         for (i = 0; i < roundId - 1; i++)
//         {
//             if (gSaveBlock2Ptr->frontier.domeWinningMoves[GetOpposingNPCPWTTournamentIdByRound(winnerTournamentId, i)] == moves[j])
//                 break;
//         }
//         if (i != roundId - 1)
//         {
//             moveScores[j] = 0;
//             bestScore = 0;
//             j = 0;
//             for (k = 0; k < MAX_MON_MOVES * PARTY_SIZE; k++)
//                 j += moveScores[k];
//             if (j == 0)
//                 break;
//             j = 0;
//             for (k = 0; k < MAX_MON_MOVES * PARTY_SIZE; k++)
//             {
//                 if (bestScore < moveScores[k])
//                 {
//                     j = k;
//                     bestScore = moveScores[k];
//                 }
//                 else if (bestScore == moveScores[k] && moves[j] < moves[k]) // Yes, these conditions are redundant
//                 {
//                     j = k;
//                     bestScore = moveScores[k];
//                 }
//             }
//         }
//     } while (i != roundId - 1);

//     if (moveScores[j] == 0)
//         j = bestId;

//     return moves[j];
// }

static void Task_ShowPWTTourneyTree(u8 taskId)
{
    int i;
    struct TextPrinterTemplate textPrinter;
    int notInteractive = gTasks[taskId].tNotInteractive;
    int r4 = gTasks[taskId].data[2];

    switch (gTasks[taskId].tState)
    {
    case 0:
        SetHBlankCallback(NULL);
        SetVBlankCallback(NULL);
        EnableInterrupts(INTR_FLAG_HBLANK | INTR_FLAG_VBLANK);
        CpuFill32(0, (void *)VRAM, VRAM_SIZE);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sPWTTourneyTreeBgTemplates, ARRAY_COUNT(sPWTTourneyTreeBgTemplates));
        InitWindows(sPWTTourneyTreeWindowTemplates);
        DeactivateAllTextPrinters();
        gBattle_BG0_X = 0;
        gBattle_BG0_Y = 0;
        gBattle_BG1_X = 0;
        gBattle_BG1_Y = 0;
        ChangeBgX(2, 0, BG_COORD_SET);
        ChangeBgY(2, 0, BG_COORD_SET);
        ChangeBgX(3, 0, BG_COORD_SET);
        ChangeBgY(3, 0xB00, BG_COORD_SET);
        gTasks[taskId].tState++;
        break;
    case 1:
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        SetGpuReg(REG_OFFSET_MOSAIC, 0);
        SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(88, 96));
        SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(0, DISPLAY_HEIGHT - 1));
        SetGpuReg(REG_OFFSET_WIN1H, WIN_RANGE(144, 152));
        SetGpuReg(REG_OFFSET_WIN1V, WIN_RANGE(0, DISPLAY_HEIGHT - 1));
        SetGpuReg(REG_OFFSET_WININ, 0);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR);
        ResetPaletteFade();
        ResetSpriteData();
        FreeAllSpritePalettes();
        gTasks[taskId].tState++;
        break;
    case 2:
        sTilemapBuffer = AllocZeroed(BG_SCREEN_SIZE);
        DecompressDataWithHeaderWram(gPWTTourneyTree_Tilemap, sTilemapBuffer);
        SetBgTilemapBuffer(1, sTilemapBuffer);
        CopyBgTilemapBufferToVram(1);
        DecompressAndLoadBgGfxUsingHeap(1, gPWTTourneyTree_Gfx, 0x2000, 0, 0);
        DecompressAndLoadBgGfxUsingHeap(2, gPWTTourneyLine_Gfx, 0x2000, 0, 0);
        DecompressAndLoadBgGfxUsingHeap(2, gPWTTourneyLineDown_Tilemap, 0x2000, 0, 1);
        DecompressAndLoadBgGfxUsingHeap(3, gPWTTourneyLineUp_Tilemap, 0x2000, 0, 1);
        LoadPalette(gPWTTourneyTree_Pal, BG_PLTT_OFFSET, BG_PLTT_SIZE);
        LoadPalette(gPWTTourneyTreeButtons_Pal, OBJ_PLTT_OFFSET, OBJ_PLTT_SIZE);
        LoadPalette(gBattleWindowTextPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        CpuFill32(0, gPlttBufferFaded, PLTT_SIZE);
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        ShowBg(3);
        gTasks[taskId].tState++;
        break;
    case 3:
        LoadCompressedSpriteSheet(sPWTTourneyTreeButtonsSpriteSheet);
        if (notInteractive == FALSE)
        {
            for (i = 0; i < ARRAY_COUNT(sPWTTourneyTreePokeballCoords); i++)
                CreateSprite(&sPWTTourneyTreePokeballSpriteTemplate, sPWTTourneyTreePokeballCoords[i][0], sPWTTourneyTreePokeballCoords[i][1], 0);

            if (gTasks[taskId].tIsPrevTourneyTree)
                CreateSprite(&sPWTExitButtonSpriteTemplate, 218, 12, 0);
            else
                CreateSprite(&sPWTCancelButtonSpriteTemplate, 218, 12, 0);
        }

        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON | DISPCNT_OBJ_1D_MAP);
        gTasks[taskId].tState++;
        break;
    case 4:
        textPrinter.fontId = FONT_SHORT;
        textPrinter.currentChar = gText_BattleTourney;
        textPrinter.windowId = PWT_TOURNEYWIN_TITLE;
        textPrinter.x = 0;
        textPrinter.y = 0;
        textPrinter.letterSpacing = 2;
        textPrinter.lineSpacing = 0;
        textPrinter.currentX = GetStringCenterAlignXOffsetWithLetterSpacing(textPrinter.fontId, textPrinter.currentChar, 0x70, textPrinter.letterSpacing);
        textPrinter.currentY = 1;
        textPrinter.unk = 0;
        textPrinter.fgColor = TEXT_DYNAMIC_COLOR_5;
        textPrinter.bgColor = TEXT_COLOR_TRANSPARENT;
        textPrinter.shadowColor = TEXT_DYNAMIC_COLOR_4;
        AddTextPrinter(&textPrinter, 0, NULL);
        for (i = 0; i < PWT_TOURNAMENT_SIZE; i++)
        {
            int roundId, var2;

            CopyPWTTrainerName(gDisplayedStringBattle, PWT_TRAINERS[i].trainerId);
            if (notInteractive == TRUE)
            {
                if (PWT_TRAINERS[i].isEliminated)
                {
                    if (PWT_TRAINERS[i].eliminatedAt != PWT_ROUND1)
                    {
                        var2 = PWT_TRAINERS[i].eliminatedAt - 1;
                        DrawPWTTourneyAdvancementLine(i, var2);
                    }
                }
                else if (gSaveBlock2Ptr->frontier.curChallengeBattleNum != PWT_ROUND2)
                {
                    DrawPWTTourneyAdvancementLine(i, gSaveBlock2Ptr->frontier.curChallengeBattleNum - 2);
                }
            }
            else if (notInteractive == FALSE)
            {
                if (PWT_TRAINERS[i].isEliminated)
                {
                    if (PWT_TRAINERS[i].eliminatedAt != PWT_ROUND1)
                    {
                        var2 = PWT_TRAINERS[i].eliminatedAt - 1;
                        DrawPWTTourneyAdvancementLine(i, var2);
                    }
                }
                else if (gSaveBlock2Ptr->frontier.curChallengeBattleNum != PWT_ROUND1)
                {
                    if (gTasks[taskId].tIsPrevTourneyTree)
                        var2 = gSaveBlock2Ptr->frontier.curChallengeBattleNum;
                    else
                        var2 = gSaveBlock2Ptr->frontier.curChallengeBattleNum - 1;
                    DrawPWTTourneyAdvancementLine(i, var2);
                }
            }

            if (gTasks[taskId].tIsPrevTourneyTree)
                roundId = gSaveBlock2Ptr->frontier.curChallengeBattleNum;
            else
                roundId = gSaveBlock2Ptr->frontier.curChallengeBattleNum - 1;

            if (    ((notInteractive == TRUE && PWT_TRAINERS[i].eliminatedAt < gSaveBlock2Ptr->frontier.curChallengeBattleNum - 1)
                  || (notInteractive == FALSE && PWT_TRAINERS[i].eliminatedAt <= roundId))
                && PWT_TRAINERS[i].isEliminated)
            {
                if (PWT_TRAINERS[i].trainerId == TRAINER_PLAYER)
                {
                    textPrinter.fgColor = TEXT_COLOR_LIGHT_GRAY;
                    textPrinter.shadowColor = TEXT_COLOR_RED;
                }
                else
                {
                    textPrinter.fgColor = TEXT_DYNAMIC_COLOR_2;
                    textPrinter.shadowColor = TEXT_DYNAMIC_COLOR_4;
                }
            }
            else
            {
                if (PWT_TRAINERS[i].trainerId == TRAINER_PLAYER)
                {
                    textPrinter.fgColor = TEXT_COLOR_LIGHT_GRAY;
                    textPrinter.shadowColor = TEXT_COLOR_RED;
                }
                else
                {
                    textPrinter.fgColor = TEXT_DYNAMIC_COLOR_5;
                    textPrinter.shadowColor = TEXT_DYNAMIC_COLOR_4;
                }
            }

            if (sPWTTrainerNamePositions[i][0] == PWT_TOURNEYWIN_NAMES_LEFT)
                textPrinter.currentX = GetStringWidthDifference(textPrinter.fontId, gDisplayedStringBattle, 0x3D, textPrinter.letterSpacing);
            else
                textPrinter.currentX = 3;
            textPrinter.currentChar = gDisplayedStringBattle;
            textPrinter.windowId = sPWTTrainerNamePositions[i][0];
            textPrinter.currentY = sPWTTrainerNamePositions[i][1];
            AddTextPrinter(&textPrinter, 0, NULL);
        }
        gTasks[taskId].tState++;
        break;
    case 5:
        PutWindowTilemap(PWT_TOURNEYWIN_NAMES_LEFT);
        PutWindowTilemap(PWT_TOURNEYWIN_NAMES_RIGHT);
        PutWindowTilemap(PWT_TOURNEYWIN_TITLE);
        CopyWindowToVram(PWT_TOURNEYWIN_NAMES_LEFT, COPYWIN_FULL);
        CopyWindowToVram(PWT_TOURNEYWIN_NAMES_RIGHT, COPYWIN_FULL);
        CopyWindowToVram(PWT_TOURNEYWIN_TITLE, COPYWIN_FULL);
        SetHBlankCallback(HblankCb_PWTTourneyTree);
        SetVBlankCallback(VblankCb_PWTTourneyTree);
        if (r4 == 2)
        {
            if (notInteractive == FALSE)
            {
                i = CreateTask(Task_HandlePWTTourneyTreeInput, 0);
                gTasks[i].data[0] = notInteractive;
                gTasks[i].data[1] = notInteractive;
                gTasks[i].data[6] = gTasks[taskId].tIsPrevTourneyTree;
            }
            else
            {
                i = CreateTask(Task_HandleStaticPWTTourneyTreeInput, 0);
                gTasks[i].data[0] = 0;
            }
        }
        else
        {
            i = gTasks[taskId].data[3];
            gTasks[i].tState = 0;
        }
        ScanlineEffect_Clear();

        i = 0;
        while (i < 91)
        {
            gScanlineEffectRegBuffers[0][i] = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(31) | BGCNT_16COLOR | BGCNT_CHARBASE(2) | BGCNT_TXT256x256;
            gScanlineEffectRegBuffers[1][i] = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(31) | BGCNT_16COLOR | BGCNT_CHARBASE(2) | BGCNT_TXT256x256;
            i++;
        }

        while (i < 160)
        {
            gScanlineEffectRegBuffers[0][i] =  BGCNT_PRIORITY(1) | BGCNT_SCREENBASE(31) | BGCNT_16COLOR | BGCNT_CHARBASE(2) | BGCNT_TXT256x256;
            gScanlineEffectRegBuffers[1][i] =  BGCNT_PRIORITY(1) | BGCNT_SCREENBASE(31) | BGCNT_16COLOR | BGCNT_CHARBASE(2) | BGCNT_TXT256x256;
            i++;
        }

        ScanlineEffect_SetParams(sPWTTourneyTreeScanlineEffectParams);
        DestroyTask(taskId);
        break;
    }
}

static void DrawPWTTourneyAdvancementLine(u8 tournamentId, u8 roundId)
{
    int i;
    const struct PWTTourneyTreeLineSection *lineSection = sPWTTourneyTreeLineSections[tournamentId][roundId];

    for (i = 0; i < sPWTTourneyTreeLineSectionArrayCounts[tournamentId][roundId]; i++)
        CopyToBgTilemapBufferRect_ChangePalette(1, &lineSection[i].tile, lineSection[i].x, lineSection[i].y, 1, 1, 17);

    CopyBgTilemapBufferToVram(1);
}

#define STATE_FADE_IN             0
#define STATE_SHOW_RESULTS        1
#define STATE_DELAY               2
#define STATE_WAIT_FOR_INPUT      3
#define STATE_CLOSE_TOURNEY_TREE  4

// The non-interactive tourney tree that's shown when a round is completed
static void Task_HandleStaticPWTTourneyTreeInput(u8 taskId)
{
    int i;
    struct TextPrinterTemplate textPrinter;

    switch (gTasks[taskId].tState)
    {
    case STATE_FADE_IN:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
        gTasks[taskId].tState = STATE_SHOW_RESULTS;
        break;
    case STATE_SHOW_RESULTS:
        if (!gPaletteFade.active)
        {
            gTasks[taskId].tState = STATE_DELAY;
            gTasks[taskId].data[3] = 64;
            textPrinter.fontId = FONT_SHORT;
            textPrinter.x = 0;
            textPrinter.y = 0;
            textPrinter.letterSpacing = 2;
            textPrinter.lineSpacing = 0;
            textPrinter.unk = 0;
            textPrinter.fgColor = TEXT_DYNAMIC_COLOR_2;
            textPrinter.bgColor = TEXT_COLOR_TRANSPARENT;
            textPrinter.shadowColor = TEXT_DYNAMIC_COLOR_4;

            // Update the advancement lines and gray out eliminated trainer names
            for (i = 0; i < PWT_TOURNAMENT_SIZE; i++)
            {
                CopyPWTTrainerName(gDisplayedStringBattle, PWT_TRAINERS[i].trainerId);
                if (PWT_TRAINERS[i].eliminatedAt == gSaveBlock2Ptr->frontier.curChallengeBattleNum - 1
                    && PWT_TRAINERS[i].isEliminated)
                {
                    if (sPWTTrainerNamePositions[i][0] == PWT_TOURNEYWIN_NAMES_LEFT)
                        textPrinter.currentX = GetStringWidthDifference(textPrinter.fontId, gDisplayedStringBattle, 0x3D, textPrinter.letterSpacing);
                    else
                        textPrinter.currentX = 3;

                    textPrinter.currentChar = gDisplayedStringBattle;
                    textPrinter.windowId = sPWTTrainerNamePositions[i][0];
                    textPrinter.currentY = sPWTTrainerNamePositions[i][1];
                    AddTextPrinter(&textPrinter, 0, NULL);
                }
                if (!PWT_TRAINERS[i].isEliminated)
                {
                    int roundId = gSaveBlock2Ptr->frontier.curChallengeBattleNum - 1;
                    DrawPWTTourneyAdvancementLine(i, roundId);
                }
            }
        }
        break;
    case STATE_DELAY:
        if (--gTasks[taskId].data[3] == 0)
            gTasks[taskId].tState = STATE_WAIT_FOR_INPUT;
        break;
    case STATE_WAIT_FOR_INPUT:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
            gTasks[taskId].tState = STATE_CLOSE_TOURNEY_TREE;
        }
        break;
    case STATE_CLOSE_TOURNEY_TREE:
        if (!gPaletteFade.active)
        {
            SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
            DestroyTask(taskId);
        }
        break;
    }
}

#undef STATE_FADE_IN
#undef STATE_SHOW_RESULTS
#undef STATE_DELAY
#undef STATE_WAIT_FOR_INPUT
#undef STATE_CLOSE_TOURNEY_TREE

static void CB2_PWTTourneyTree(void)
{
    AnimateSprites();
    BuildOamBuffer();
    RunTextPrinters();
    UpdatePaletteFade();
    RunTasks();
}

static void VblankCb_PWTTourneyInfoCard(void)
{
    ChangeBgX(3, 0x80, BG_COORD_ADD);
    ChangeBgY(3, 0x80, BG_COORD_SUB);
    SetGpuReg(REG_OFFSET_BG0HOFS, gBattle_BG0_X);
    SetGpuReg(REG_OFFSET_BG0VOFS, gBattle_BG0_Y);
    SetGpuReg(REG_OFFSET_BG1HOFS, gBattle_BG1_X);
    SetGpuReg(REG_OFFSET_BG1VOFS, gBattle_BG1_Y);
    SetGpuReg(REG_OFFSET_BG2HOFS, gBattle_BG2_X);
    SetGpuReg(REG_OFFSET_BG2VOFS, gBattle_BG2_Y);
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

#define SET_WIN0H_WIN1H(win0H, win1H)                       \
{                                                           \
    *(vu32*)(REG_ADDR_WIN0H) = ((win0H << 16) | (win1H));   \
}

static void HblankCb_PWTTourneyTree(void)
{
    u16 vCount = REG_VCOUNT;

    if (vCount < 42)
    {
        REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_CLR | WININ_WIN0_OBJ
                | WININ_WIN1_BG_ALL | WININ_WIN1_CLR | WININ_WIN1_OBJ;
        SET_WIN0H_WIN1H(0, 0);
    }
    else if (vCount < 50)
    {
        REG_WININ = WININ_WIN0_BG0 | WININ_WIN0_BG1 | WININ_WIN0_BG3 | WININ_WIN0_OBJ | WININ_WIN0_CLR
                    | WININ_WIN1_BG0 | WININ_WIN1_BG1 | WININ_WIN1_BG3 | WININ_WIN1_OBJ | WININ_WIN1_CLR;
        SET_WIN0H_WIN1H(WIN_RANGE(152, 155), WIN_RANGE(85, 88));
    }
    else if (vCount < 58)
    {
        REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_CLR | WININ_WIN0_OBJ
                | WININ_WIN1_BG_ALL | WININ_WIN1_CLR | WININ_WIN1_OBJ;
        SET_WIN0H_WIN1H(0, 0);
    }
    else if (vCount < 75)
    {
        REG_WININ = WININ_WIN0_BG0 | WININ_WIN0_BG1 | WININ_WIN0_BG3 | WININ_WIN0_OBJ | WININ_WIN0_CLR
                    | WININ_WIN1_BG0 | WININ_WIN1_BG1 | WININ_WIN1_BG3 | WININ_WIN1_OBJ | WININ_WIN1_CLR;
        SET_WIN0H_WIN1H(WIN_RANGE(144, 152), WIN_RANGE(88, 96));
    }
    else if (vCount < 82)
    {
        REG_WININ = WININ_WIN0_BG0 | WININ_WIN0_BG1 | WININ_WIN0_BG3 | WININ_WIN0_OBJ | WININ_WIN0_CLR
                    | WININ_WIN1_BG0 | WININ_WIN1_BG1 | WININ_WIN1_BG3 | WININ_WIN1_OBJ | WININ_WIN1_CLR;
        SET_WIN0H_WIN1H(WIN_RANGE(152, 155), WIN_RANGE(85, 88));
    }
    else if (vCount < 95)
    {
        REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_CLR | WININ_WIN0_OBJ
                | WININ_WIN1_BG_ALL | WININ_WIN1_CLR | WININ_WIN1_OBJ;
        SET_WIN0H_WIN1H(0, 0);
    }
    else if (vCount < 103)
    {
        REG_WININ = WININ_WIN0_BG0 | WININ_WIN0_BG1 | WININ_WIN0_BG2 | WININ_WIN0_OBJ | WININ_WIN0_CLR
                    | WININ_WIN1_BG0 | WININ_WIN1_BG1 | WININ_WIN1_BG2 | WININ_WIN1_OBJ | WININ_WIN1_CLR;
        SET_WIN0H_WIN1H(WIN_RANGE(152, 155), WIN_RANGE(85, 88));
    }
    else if (vCount < 119)
    {
        REG_WININ = WININ_WIN0_BG0 | WININ_WIN0_BG1 | WININ_WIN0_BG2 | WININ_WIN0_OBJ | WININ_WIN0_CLR
                    | WININ_WIN1_BG0 | WININ_WIN1_BG1 | WININ_WIN1_BG2 | WININ_WIN1_OBJ | WININ_WIN1_CLR;
        SET_WIN0H_WIN1H(WIN_RANGE(144, 152), WIN_RANGE(88, 96));
    }
    else if (vCount < 127)
    {
        REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_CLR | WININ_WIN0_OBJ
                | WININ_WIN1_BG_ALL | WININ_WIN1_CLR | WININ_WIN1_OBJ;
        SET_WIN0H_WIN1H(0, 0);
    }
    else if (vCount < 135)
    {
        REG_WININ = WININ_WIN0_BG0 | WININ_WIN0_BG1 | WININ_WIN0_BG2 | WININ_WIN0_OBJ | WININ_WIN0_CLR
                    | WININ_WIN1_BG0 | WININ_WIN1_BG1 | WININ_WIN1_BG2 | WININ_WIN1_OBJ | WININ_WIN1_CLR;
        SET_WIN0H_WIN1H(WIN_RANGE(152, 155), WIN_RANGE(85, 88));
    }
    else
    {
        REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_CLR | WININ_WIN0_OBJ
                | WININ_WIN1_BG_ALL | WININ_WIN1_CLR | WININ_WIN1_OBJ;
        SET_WIN0H_WIN1H(0, 0);
    }
}

static void VblankCb_PWTTourneyTree(void)
{
    SetGpuReg(REG_OFFSET_BG0HOFS, gBattle_BG0_X);
    SetGpuReg(REG_OFFSET_BG0VOFS, gBattle_BG0_Y);
    SetGpuReg(REG_OFFSET_BG1HOFS, gBattle_BG1_X);
    SetGpuReg(REG_OFFSET_BG1VOFS, gBattle_BG1_Y);
    ChangeBgY(2, 0x80, BG_COORD_SUB);
    ChangeBgY(3, 0x80, BG_COORD_ADD);
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ScanlineEffect_InitHBlankDmaTransfer();
}

static void SetPWTTrainerAndMonPtrs(void)
{
    gPWTFacilityTrainerMons = gPWTMons;
    gPWTFacilityTrainers = gPWTTrainers;
}

// TODO - check that SavePlayerParty and LoadPlayerParty handle the following cases:
// 1) Forms
// 2) Items
// 3) Sketch

static void SetPWTPlayerParty(void)
{
    for (u8 i = 0; i < PWT_PARTY_SIZE; i++)
        gSaveBlock2Ptr->pwt.selectedPartyMons[i] = gSelectedOrderFromParty[i];
    ReducePlayerPartyToSelectedMons();
}

// TODO - implement function
// static void BufferLastPWTWinnerName(void)
// {
//     int i;

//     SetPWTTrainerAndMonPtrs();
//     for (i = 0; i < PWT_TOURNAMENT_SIZE; i++)
//     {
//         if (!PWT_TRAINERS[i].isEliminated)
//             break;
//     }
//     CopyPWTTrainerName(gStringVar1, PWT_TRAINERS[i].trainerId);
// }

// TODO - implement function
// // For showing the previous tourney results before the player has entered a challenge
// static void InitRandomPWTTourneyTreeResults(void)
// {
//     int i, j, k;
//     int monLevel;
//     int species[PARTY_SIZE];
//     int monTypesBits;
//     int trainerId;
//     int monId;
//     int zero1;
//     int zero2;
//     u8 lvlMode;
//     u16 *statSums;
//     int *statValues;
//     u8 ivs = 0;

//     species[0] = 0;
//     species[1] = 0;
//     species[2] = 0;
//     if ((gSaveBlock2Ptr->frontier.domeLvlMode != -gSaveBlock2Ptr->frontier.domeBattleMode) && gSaveBlock2Ptr->frontier.challengeStatus != CHALLENGE_STATUS_SAVING)
//         return;

//     statSums = AllocZeroed(sizeof(u16) * PWT_TOURNAMENT_SIZE);
//     statValues = AllocZeroed(sizeof(int) * NUM_STATS);
//     lvlMode = gSaveBlock2Ptr->frontier.lvlMode;
//     gSaveBlock2Ptr->frontier.lvlMode = FRONTIER_LVL_50;
//     zero1 = 0;
//     zero2 = 0;

//     gSaveBlock2Ptr->frontier.domeLvlMode = zero1 + 1;
//     gSaveBlock2Ptr->frontier.domeBattleMode = zero2 + 1;

//     for (i = 0; i < PWT_TOURNAMENT_SIZE; i++)
//     {
//         do
//         {
//             if (i < 5)
//                 trainerId = Random() % 10;
//             else if (i < 15)
//                 trainerId = Random() % 20 + 10;
//             else
//                 trainerId = Random() % 10 + 30;

//             for (j = 0; j < i; j++)
//             {
//                 if (PWT_TRAINERS[j].trainerId == trainerId)
//                     break;
//             }
//         } while (j != i);

//         PWT_TRAINERS[i].trainerId = trainerId;
//         for (j = 0; j < PARTY_SIZE; j++)
//         {
//             do
//             {
//                 monId = GetRandomFrontierMonFromSet(trainerId);
//                 for (k = 0; k < j; k++)
//                 {
//                     // Make sure the mon is valid.
//                     int alreadySelectedMonId = PWT_MONS[i][k];
//                     if (alreadySelectedMonId == monId
//                         || species[0] == gPWTFacilityTrainerMons[monId].species
//                         || species[1] == gPWTFacilityTrainerMons[monId].species
//                         || gPWTFacilityTrainerMons[alreadySelectedMonId].heldItem == gPWTFacilityTrainerMons[monId].heldItem)
//                         break;
//                 }
//             } while (k != j);

//             PWT_MONS[i][j] = monId;
//             species[j] = gPWTFacilityTrainerMons[monId].species;
//         }
//         PWT_TRAINERS[i].isEliminated = FALSE;
//         PWT_TRAINERS[i].eliminatedAt = 0;
//         PWT_TRAINERS[i].forfeited = FALSE;
//     }

//     monLevel = FRONTIER_MAX_LEVEL_50;
//     for (i = 0; i < PWT_TOURNAMENT_SIZE; i++)
//     {
//         monTypesBits = 0;
//         statSums[i] = 0;
//         ivs = GetDomeTrainerMonIvs(PWT_TRAINERS[i].trainerId);
//         for (j = 0; j < PARTY_SIZE; j++)
//         {
//             CalcDomeMonStats(&gPWTFacilityTrainerMons[PWT_MONS[i][j]],
//                              monLevel, ivs, statValues);

//             statSums[i] += statValues[STAT_ATK];
//             statSums[i] += statValues[STAT_DEF];
//             statSums[i] += statValues[STAT_SPATK];
//             statSums[i] += statValues[STAT_SPDEF];
//             statSums[i] += statValues[STAT_SPEED];
//             statSums[i] += statValues[STAT_HP];
//             monTypesBits |= 1u << GetSpeciesType(gPWTFacilityTrainerMons[PWT_MONS[i][j]].species, 0);
//             monTypesBits |= 1u << GetSpeciesType(gPWTFacilityTrainerMons[PWT_MONS[i][j]].species, 1);
//         }

//         // Because GF hates temporary vars, trainerId acts like monTypesCount here.
//         for (trainerId = 0, j = 0; j < 32; j++)
//         {
//             if (monTypesBits & 1)
//                 trainerId++;
//             monTypesBits >>= 1;
//         }
//         statSums[i] += (trainerId * monLevel) / 20;
//     }

//     for (i = 0; i < PWT_TOURNAMENT_SIZE - 1; i++)
//     {
//         for (j = i + 1; j < PWT_TOURNAMENT_SIZE; j++)
//         {
//             if (statSums[i] < statSums[j])
//             {
//                 SwapDomeTrainers(i, j, statSums);
//             }
//             else if (statSums[i] == statSums[j])
//             {
//                 if (PWT_TRAINERS[i].trainerId > PWT_TRAINERS[j].trainerId)
//                     SwapDomeTrainers(i, j, statSums);
//             }
//         }
//     }

//     Free(statSums);
//     Free(statValues);

//     for (i = 0; i < PWT_ROUNDS_COUNT; i++)
//         DecidePWTRoundWinners(i);

//     gSaveBlock2Ptr->frontier.lvlMode = lvlMode;
// }

static int TrainerIdToPWTTournamentId(u16 trainerId)
{
    int i;

    for (i = 0; i < PWT_TOURNAMENT_SIZE; i++)
    {
        if (PWT_TRAINERS[i].trainerId == trainerId)
            break;
    }

    return i;
}

// TODO - implement function
// // The same as the above one, but has global scope.
// int TrainerIdToDomeTournamentId(u16 trainerId)
// {
//     int i;

//     for (i = 0; i < PWT_TOURNAMENT_SIZE; i++)
//     {
//         if (PWT_TRAINERS[i].trainerId == trainerId)
//             break;
//     }

//     return i;
// }

// TODO - imeplement function
// static u8 GetOpposingNPCPWTTournamentIdByRound(u8 tournamentId, u8 round)
// {
//     u8 tournamentIds[2];
//     BufferPWTWinString(sTrainerAndRoundToLastPWTMatchCardNum[sPWTTournamentIdToPairedTrainerIds[tournamentId] / 2][round] - 16, tournamentIds);
//     if (tournamentId == tournamentIds[0])
//         return tournamentIds[1];
//     else
//         return tournamentIds[0];
// }

// TODO - implement function
// // Determines which trainers won in the NPC vs NPC battles
// static void DecidePWTRoundWinners(u8 roundId)
// {
//     int i;
//     int moveSlot, monId1, monId2;
//     int tournamentId1, tournamentId2;
//     int species;
//     int points1 = 0, points2 = 0;

//     for (i = 0; i < PWT_TOURNAMENT_SIZE; i++)
//     {
//         if (PWT_TRAINERS[i].isEliminated || PWT_TRAINERS[i].trainerId == TRAINER_PLAYER)
//             continue;

//         tournamentId1 = i;
//         tournamentId2 = TournamentIdOfPWTOpponent(roundId, PWT_TRAINERS[tournamentId1].trainerId);
//         // Frontier Brain always wins, check tournamentId1.
//         if (PWT_TRAINERS[tournamentId1].trainerId == TRAINER_FRONTIER_BRAIN && tournamentId2 != 0xFF)
//         {
//             PWT_TRAINERS[tournamentId2].isEliminated = TRUE;
//             PWT_TRAINERS[tournamentId2].eliminatedAt = roundId;
//             gSaveBlock2Ptr->frontier.domeWinningMoves[tournamentId2] = GetWinningMove(tournamentId1, tournamentId2, roundId);
//         }
//         // Frontier Brain always wins, check tournamentId2.
//         else if (tournamentId2 != 0xFF && PWT_TRAINERS[tournamentId2].trainerId == TRAINER_FRONTIER_BRAIN && tournamentId1 != 0xFF)
//         {
//             PWT_TRAINERS[tournamentId1].isEliminated = TRUE;
//             PWT_TRAINERS[tournamentId1].eliminatedAt = roundId;
//             gSaveBlock2Ptr->frontier.domeWinningMoves[tournamentId1] = GetWinningMove(tournamentId2, tournamentId1, roundId);
//         }
//         // Decide which one of two trainers wins!
//         else if (tournamentId2 != 0xFF)
//         {
//             // BUG: points1 and points2 are not cleared at the beginning of the loop resulting in not fair results.
//             #ifdef BUGFIX
//             points1 = 0;
//             points2 = 0;
//             #endif

//             // Calculate points for both trainers.
//             for (monId1 = 0; monId1 < PARTY_SIZE; monId1++)
//             {
//                 for (moveSlot = 0; moveSlot < MAX_MON_MOVES; moveSlot++)
//                 {
//                     for (monId2 = 0; monId2 < PARTY_SIZE; monId2++)
//                     {
//                         points1 += GetTypeEffectivenessPoints(gPWTFacilityTrainerMons[PWT_MONS[tournamentId1][monId1]].moves[moveSlot],
//                                                 gPWTFacilityTrainerMons[PWT_MONS[tournamentId2][monId2]].species, EFFECTIVENESS_MODE_AI_VS_AI);
//                     }
//                 }
//                 species = gPWTFacilityTrainerMons[PWT_MONS[tournamentId1][monId1]].species;
//                 points1 += GetTotalBaseStat(species) / 10;
//             }
//             // Random part of the formula.
//             points1 += (Random() & 0x1F);
//             // Favor trainers with higher id;
//             points1 += tournamentId1;

//             for (monId1 = 0; monId1 < PARTY_SIZE; monId1++)
//             {
//                 for (moveSlot = 0; moveSlot < MAX_MON_MOVES; moveSlot++)
//                 {
//                     for (monId2 = 0; monId2 < PARTY_SIZE; monId2++)
//                     {
//                         points2 += GetTypeEffectivenessPoints(gPWTFacilityTrainerMons[PWT_MONS[tournamentId2][monId1]].moves[moveSlot],
//                                                 gPWTFacilityTrainerMons[PWT_MONS[tournamentId1][monId2]].species, EFFECTIVENESS_MODE_AI_VS_AI);
//                     }
//                 }
//                 species = gPWTFacilityTrainerMons[PWT_MONS[tournamentId2][monId1]].species;
//                 points2 += GetTotalBaseStat(species) / 10;
//             }
//             // Random part of the formula.
//             points2 += (Random() & 0x1F);
//             // Favor trainers with higher id;
//             points2 += tournamentId2;

//             if (points1 > points2)
//             {
//                 PWT_TRAINERS[tournamentId2].isEliminated = TRUE;
//                 PWT_TRAINERS[tournamentId2].eliminatedAt = roundId;
//                 gSaveBlock2Ptr->frontier.domeWinningMoves[tournamentId2] = GetWinningMove(tournamentId1, tournamentId2, roundId);
//             }
//             else if (points1 < points2)
//             {
//                 PWT_TRAINERS[tournamentId1].isEliminated = TRUE;
//                 PWT_TRAINERS[tournamentId1].eliminatedAt = roundId;
//                 gSaveBlock2Ptr->frontier.domeWinningMoves[tournamentId1] = GetWinningMove(tournamentId2, tournamentId1, roundId);
//             }
//             // Points are the same, so we favor the one with the higher id.
//             else if (tournamentId1 > tournamentId2)
//             {
//                 PWT_TRAINERS[tournamentId2].isEliminated = TRUE;
//                 PWT_TRAINERS[tournamentId2].eliminatedAt = roundId;
//                 gSaveBlock2Ptr->frontier.domeWinningMoves[tournamentId2] = GetWinningMove(tournamentId1, tournamentId2, roundId);
//             }
//             else
//             {
//                 PWT_TRAINERS[tournamentId1].isEliminated = TRUE;
//                 PWT_TRAINERS[tournamentId1].eliminatedAt = roundId;
//                 gSaveBlock2Ptr->frontier.domeWinningMoves[tournamentId1] = GetWinningMove(tournamentId2, tournamentId1, roundId);
//             }
//         }
//     }
// }

void CopyPWTTrainerName(u8 *str, u16 trainerId)
{
    int i = 0;
    SetPWTTrainerAndMonPtrs();

    if (trainerId == TRAINER_PLAYER)
    {
        for (i = 0; i < PLAYER_NAME_LENGTH; i++)
            str[i] = gSaveBlock2Ptr->playerName[i];
    }
    else if (trainerId < PWT_TRAINERS_COUNT)
    {
        for (i = 0; i < TRAINER_NAME_LENGTH; i++)
            str[i] = gPWTFacilityTrainers[trainerId].trainerName[i];
    }
    str[i] = EOS;
}

// TODO - seperate functions between taken from battle frontier util, battle tower, and personally made
// ---------- NEW FUNCTIONS - BATTLE FRONTIER UTIL OR PERSONAL ----------
static void CheckPWTIneligibility(void)
{
    // u16 speciesArray[PARTY_SIZE], itemArray[PARTY_SIZE];
    u8 monId, prevId;

    // Check each mon's validity
    for (monId = 0; monId < PARTY_SIZE; monId++)
    {
        // TODO - check how forms work, are they the same species?
        u16 species = SanitizeSpeciesId(GET_BASE_SPECIES_ID(GetMonData(&gPlayerParty[monId], MON_DATA_SPECIES_OR_EGG)));
        if ((species == SPECIES_NONE) || (species == SPECIES_EGG) || (gSpeciesInfo[species].isFrontierBanned))
            break;
        u16 heldItem = GetMonData(&gPlayerParty[monId], MON_DATA_HELD_ITEM);
        for (prevId = 0; prevId < monId; prevId++) {
            u16 prevSpecies = SanitizeSpeciesId(GET_BASE_SPECIES_ID(GetMonData(&gPlayerParty[prevId], MON_DATA_SPECIES_OR_EGG)));
            u16 prevItem = GetMonData(&gPlayerParty[prevId], MON_DATA_HELD_ITEM);
            if ((prevSpecies == species) || ((heldItem != ITEM_NONE) && (prevItem == heldItem)))
                break;
        }
        if (prevId != monId)
            break;
    }

    // TODO - handle printing ineligible species
    if (monId == PARTY_SIZE)
        gSpecialVar_Result = FALSE;
    else
        gSpecialVar_Result = TRUE;
    return;
}

u16 GetRandomPWTMonFromSet(u16 trainerId, u8 pool)
{
    SetPWTTrainerAndMonPtrs();
    u16 monId;
    switch (pool)
    {
        default:
        case PWT_TRAINER_POOL_REGULAR:
            monId = gPWTFacilityTrainers[trainerId].monPool[Random() % PWT_REGULAR_POOL_SIZE];
            break;
        case PWT_TRAINER_POOL_MEGA:
            monId = gPWTFacilityTrainers[trainerId].megaPool[Random() % PWT_MEGA_POOL_SIZE];
            break;
        case PWT_TRAINER_POOL_LEGEND:
            monId = gPWTFacilityTrainers[trainerId].legendPool[Random() % PWT_LEGEND_POOL_SIZE];
            break;
    }
    return monId;
}

static void GetPWTOpponentIntro(void)
{
    u16 trainerId;
    SetPWTTrainerAndMonPtrs();
    // TODO - handle a check for multi battles
    trainerId = TRAINER_BATTLE_PARAM.opponentA;
    ShowFieldMessage(gPWTFacilityTrainers[trainerId].speechBefore);
}

u8 GetPWTTrainerFrontSpriteId(u16 trainerId)
{
    SetPWTTrainerAndMonPtrs();
    // TODO - handle facility call to handle PWT
    return gFacilityClassToPicIndex[gPWTFacilityTrainers[trainerId].facilityClass];
}

enum TrainerClassID GetPWTOpponentClass(u16 trainerId)
{
    SetPWTTrainerAndMonPtrs();
    // TODO - handle facility call to handle PWT
    return gFacilityClassToTrainerClass[gPWTFacilityTrainers[trainerId].facilityClass];
}

// Determines which pokeball to give the opponent's Pokémon based on their type, equally weighted between the primary and secondary type
u8 GetPWTMonBall(u8 species) {
    u8 type;
    if (GetSpeciesType(species, 1) == TYPE_NONE)
        type = GetSpeciesType(species, 0);
    else
        type = Random() % 2 == 0 ? GetSpeciesType(species, 0) : GetSpeciesType(species, 1);

    switch (type) {
        case TYPE_NORMAL:
            switch (Random() % 3) {
                default:
                case 0:
                    return BALL_HEAL;
                    
                case 1:
                    return BALL_REPEAT;
                case 2:
                    return BALL_LEVEL;
            }
        case TYPE_FIGHTING:
            return BALL_SPORT;
        case TYPE_FLYING:
            return BALL_QUICK;
        case TYPE_POISON:
            return BALL_SAFARI;
        case TYPE_GROUND:
            return BALL_TIMER;
        case TYPE_ROCK:
            return BALL_NEST;
        case TYPE_BUG:
            return BALL_NET;
        case TYPE_GHOST:
            return BALL_DUSK;
        case TYPE_STEEL:
            return BALL_HEAVY;
        case TYPE_FIRE:
            return BALL_FAST;
        case TYPE_WATER:
            switch (Random() % 2) {
                default:
                case 0:
                    return BALL_DIVE;
                    
                case 1:
                    return BALL_LURE;
            }
        case TYPE_GRASS:
            return BALL_FRIEND;
        case TYPE_ELECTRIC:
            return BALL_PARK;
        case TYPE_PSYCHIC:
            return BALL_DREAM;
        case TYPE_ICE:
            return BALL_PREMIER;
        case TYPE_DRAGON:
            switch (Random() % 2) {
                default:
                case 0:
                    return BALL_LUXURY;
                    
                case 1:
                    return BALL_BEAST;
            }
        case TYPE_DARK:
            return BALL_MOON;
        case TYPE_FAIRY:
            return BALL_LOVE;
        default: // ERROR condition
            return BALL_STRANGE;
    }
}

u16 GetRandomPWTTrainerId(u8 tournamentType)
{
    u16 trainerId, startId, endId;

    switch (tournamentType)
    {
    case PWT_TOURNAMENT_TYPE_HOENN:
        startId = PWT_TRAINER_HOENN_START;
        endId = PWT_TRAINER_HOENN_END;
        break;
    default:
    case PWT_TOURNAMENT_TYPE_ROYALE:
        startId = PWT_TRAINER_KANTO_START;
        endId = PWT_TRAINER_JHOTO_END;
        break;
    }
    trainerId = (Random() % (endId - startId)) + startId;
    return trainerId;
}

static void HandlePWTTrainerBattleEnd(void)
{
    SetPWTTrainerAndMonPtrs();
    if (Random() % 20 == 0)
        UpdateGymLeaderRematch();
    // TODO - Check HandleSpecialTrainerBattleEnd in Battle Tower for specifics
    // case SPECIAL_BATTLE_MULTI:
    //     for (i = 0; i < 3; i++)
    //     {
    //         if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES))
    //             SavePlayerPartyMon(i, &gPlayerParty[i]);
    //     }
    //     break;
    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

static void Task_StartPWTBattleAfterTransition(u8 taskId)
{
    SetPWTTrainerAndMonPtrs();
    if (IsBattleTransitionDone() == TRUE)
    {
        gMain.savedCallback = HandlePWTTrainerBattleEnd;
        SetMainCallback2(CB2_InitBattle);
        DestroyTask(taskId);
    }
}

void DoPWTTrainerBattle(void)
{
    SetPWTTrainerAndMonPtrs();
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_PWT;
    // TODO - add check for double battles
    // if (VarGet(VAR_FRONTIER_BATTLE_MODE) == PWT_MODE_DOUBLES)
    //     gBattleTypeFlags |= BATTLE_TYPE_DOUBLE;
    CreateTask(Task_StartPWTBattleAfterTransition, 1);
    CreateTask_PlayMapChosenOrBattleBGM(0);
    BattleTransition_StartOnField(B_TRANSITION_PWT);
}

void CopyPWTTrainerText(u8 whichText, u16 trainerId)
{
    SetPWTTrainerAndMonPtrs();
    switch (whichText)
    {
    case PWT_BEFORE_TEXT:
        StringCopy(gStringVar4, gPWTFacilityTrainers[trainerId].speechBefore);
        break;
    case PWT_PLAYER_LOST_TEXT:
        StringCopy(gStringVar4, gPWTFacilityTrainers[trainerId].speechWin);
        break;
    case PWT_PLAYER_WON_TEXT:
        StringCopy(gStringVar4, gPWTFacilityTrainers[trainerId].speechLose);
        break;
    }
}