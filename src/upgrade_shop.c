#include "global.h"
#include "bg.h"
#include "data.h"
#include "decompress.h"
#include "decoration.h"
#include "decoration_inventory.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "international_string_util.h"
#include "item.h"
#include "item_icon.h"
#include "item_menu.h"
#include "list_menu.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "money.h"
#include "move.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "scanline_effect.h"
#include "script.h"
#include "upgrade_shop.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "text_window.h"
#include "tv.h"
#include "constants/decorations.h"
#include "constants/event_objects.h"
#include "constants/items.h"
#include "constants/metatile_behaviors.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#define TAG_SCROLL_ARROW   2100
#define TAG_ITEM_ICON_BASE 9110 // immune to time blending

#define MAX_ITEMS_SHOWN 8
#define SHOP_MENU_PALETTE_ID 12

enum {
    WIN_MONEY,
    WIN_ITEM_LIST,
    WIN_ITEM_DESCRIPTION,
    WIN_MESSAGE,
};

enum {
    COLORID_NORMAL,      // Item descriptions, quantity in bag, and quantity/price
    COLORID_ITEM_LIST,   // The text in the item list, and the cursor normally
    COLORID_GRAY_CURSOR, // When the cursor has selected an item to purchase
};

// shop view window NPC info enum
enum
{
    OBJ_EVENT_ID,
    X_COORD,
    Y_COORD,
    ANIM_NUM,
    LAYER_TYPE
};

struct UngradeShopInfo
{
    void (*callback)(void);
    const struct MenuAction *menuActions;
    const u16 *itemList;
    u16 itemCount;
    u8 windowId;
};

struct UpgradeShopData
{
    u16 tilemapBuffers[4][0x400];
    u32 totalCost;
    u16 itemsShowed;
    u16 selectedRow;
    u16 scrollOffset;
    u16 maxQuantity;
    u8 scrollIndicatorsTaskId;
    u8 iconSlot;
    u8 itemSpriteIds[2];
    s16 viewportObjects[OBJECT_EVENTS_COUNT][5];
};

static EWRAM_DATA struct UngradeShopInfo sUngradeShopInfo = {0};
static EWRAM_DATA struct UpgradeShopData *sUpgradeShopData = NULL;
static EWRAM_DATA struct ListMenuItem *sListMenuItems = NULL;
static EWRAM_DATA u8 (*sItemNames)[ITEM_NAME_LENGTH + 2] = {0};

static void Task_UpgradeShopMenu(u8 taskId);
static void Task_HandleUpgradeShopMenuQuit(u8 taskId);
static void CB2_InitBuyUpgradeMenu(void);
static void Task_GoToBuyOrSellUpgradeMenu(u8 taskId);
static void MapPostLoadHook_ReturnToUpgradeShopMenu(void);
static void Task_ReturnToUpgradeShopMenu(u8 taskId);
static void ShowUpgradeShopMenuAfterExitingBuyOrSellMenu(u8 taskId);
static void BuyUpgradeMenuDrawGraphics(void);
static void BuyUpgradeMenuAddScrollIndicatorArrows(void);
static void Task_BuyUpgradeMenu(u8 taskId);
static void BuyUpgradeMenuBuildListMenuTemplate(void);
static void BuyUpgradeMenuInitBgs(void);
static void BuyUpgradeMenuInitWindows(void);
static void BuyUpgradeMenuDecompressBgGraphics(void);
static void BuyUpgradeMenuSetListEntry(struct ListMenuItem *, u16, u8 *);
static void BuyUpgradeMenuAddItemIcon(u16, u8);
static void BuyUpgradeMenuRemoveItemIcon(u16, u8);
static void BuyUpgradeMenuPrint(u8 windowId, const u8 *text, u8 x, u8 y, s8 speed, u8 colorSet);
static void ExitBuyUpgradeMenu(u8 taskId);
static void Task_ExitBuyUpgradeMenu(u8 taskId);
static void BuyUpgradeMenuTryMakePurchase(u8 taskId);
static void BuyUpgradeMenuReturnToItemList(u8 taskId);
static void BuyUpgradeMenuConfirmPurchase(u8 taskId);
static void BuyUpgradeMenuSubtractMoney(u8 taskId);
static void Task_ReturnToUpgradeListAfterItemPurchase(u8 taskId);
static void Task_HandleUpgradeShopMenuBuy(u8 taskId);
static void BuyUpgradeMenuPrintDescriptionAndShowIcon(s32 item, bool8 onInit, struct ListMenu *list);
static void BuyUpgradeMenuPrintPriceInList(u8 windowId, u32 itemId, u8 y);

static const struct YesNoFuncTable sUpgradeShopPurchaseYesNoFuncs =
{
    BuyUpgradeMenuTryMakePurchase,
    BuyUpgradeMenuReturnToItemList
};

static const struct MenuAction sUpgradeShopMenuActions_BuyQuit[] =
{
    { gText_UpgradeShopBuy, {.void_u8=Task_HandleUpgradeShopMenuBuy} },
    { gText_UpgradeShopQuit, {.void_u8=Task_HandleUpgradeShopMenuQuit} }
};

static const struct WindowTemplate sUpgradeShopMenuWindowTemplates =
{
    .bg = 0,
    .tilemapLeft = 2,
    .tilemapTop = 1,
    .width = 9,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x0008,
};

static const struct ListMenuTemplate sShopBuyMenuListTemplate =
{
    .items = NULL,
    .moveCursorFunc = BuyUpgradeMenuPrintDescriptionAndShowIcon,
    .itemPrintFunc = BuyUpgradeMenuPrintPriceInList,
    .totalItems = 0,
    .maxShowed = 0,
    .windowId = WIN_ITEM_LIST,
    .header_X = 0,
    .item_X = 8,
    .cursor_X = 0,
    .upText_Y = 1,
    .cursorPal = 1,
    .fillValue = 0,
    .cursorShadowPal = 2,
    .lettersSpacing = 0,
    .itemVerticalPadding = 0,
    .scrollMultiple = LIST_NO_MULTIPLE_SCROLL,
    .fontId = FONT_NARROW,
    .cursorKind = CURSOR_BLACK_ARROW,
    .textNarrowWidth = 84,
};

static const struct BgTemplate sUpgradeShopBuyMenuBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    },
    {
        .bg = 2,
        .charBaseIndex = 0,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    },
    {
        .bg = 3,
        .charBaseIndex = 0,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0
    }
};

static const struct WindowTemplate sUpgradeShopBuyMenuWindowTemplates[] =
{
    [WIN_MONEY] = {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 10,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x001E,
    },
    [WIN_ITEM_LIST] = {
        .bg = 0,
        .tilemapLeft = 14,
        .tilemapTop = 2,
        .width = 15,
        .height = 16,
        .paletteNum = 15,
        .baseBlock = 0x0032,
    },
    [WIN_ITEM_DESCRIPTION] = {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 13,
        .width = 14,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 0x0122,
    },
    [WIN_MESSAGE] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 27,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x01A2,
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sShopBuyUpgradeMenuYesNoWindowTemplates =
{
    .bg = 0,
    .tilemapLeft = 21,
    .tilemapTop = 9,
    .width = 5,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x020E,
};

static const u8 sUpgradeShopBuyMenuTextColors[][3] =
{
    [COLORID_NORMAL]      = {1, 2, 3},
    [COLORID_ITEM_LIST]   = {0, 1, 2},
    [COLORID_GRAY_CURSOR] = {0, 1, 2},
};

static u8 CreateUpgradeShopMenu()
{
    int numMenuItems;
    LockPlayerFieldControls();

    struct WindowTemplate winTemplate = sUpgradeShopMenuWindowTemplates;
    winTemplate.width = GetMaxWidthInMenuTable(sUpgradeShopMenuActions_BuyQuit, ARRAY_COUNT(sUpgradeShopMenuActions_BuyQuit));

    sUngradeShopInfo.windowId = AddWindow(&winTemplate);
    sUngradeShopInfo.menuActions = sUpgradeShopMenuActions_BuyQuit;
    numMenuItems = ARRAY_COUNT(sUpgradeShopMenuActions_BuyQuit);

    SetStandardWindowBorderStyle(sUngradeShopInfo.windowId, FALSE);
    PrintMenuTable(sUngradeShopInfo.windowId, numMenuItems, sUngradeShopInfo.menuActions);
    InitMenuInUpperLeftCornerNormal(sUngradeShopInfo.windowId, numMenuItems, 0);
    PutWindowTilemap(sUngradeShopInfo.windowId);
    CopyWindowToVram(sUngradeShopInfo.windowId, COPYWIN_MAP);

    return CreateTask(Task_UpgradeShopMenu, 8);
}

static void SetUpgradeShopMenuCallback(void (*callback)(void))
{
    sUngradeShopInfo.callback = callback;
}

static void SetUpgradeShopItemsForSale(const u16 *items)
{
    u16 i = 0;

    sUngradeShopInfo.itemList = items;
    sUngradeShopInfo.itemCount = 0;

    // Read items until ITEM_NONE / DECOR_NONE is reached
    while (sUngradeShopInfo.itemList[i])
    {
        sUngradeShopInfo.itemCount++;
        i++;
    }
}

static void Task_UpgradeShopMenu(u8 taskId)
{
    s8 inputCode = Menu_ProcessInputNoWrap();
    switch (inputCode)
    {
    case MENU_NOTHING_CHOSEN:
        break;
    case MENU_B_PRESSED:
        PlaySE(SE_SELECT);
        Task_HandleUpgradeShopMenuQuit(taskId);
        break;
    default:
        sUngradeShopInfo.menuActions[inputCode].func.void_u8(taskId);
        break;
    }
}

#define tItemId     data[5]
#define tListTaskId data[7]
#define tCallbackHi data[8]
#define tCallbackLo data[9]

static void Task_HandleUpgradeShopMenuBuy(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    tCallbackHi = (u32)CB2_InitBuyUpgradeMenu >> 16;
    tCallbackLo = (u32)CB2_InitBuyUpgradeMenu;
    gTasks[taskId].func = Task_GoToBuyOrSellUpgradeMenu;
    FadeScreen(FADE_TO_BLACK, 0);
}

static void Task_HandleUpgradeShopMenuQuit(u8 taskId)
{
    ClearStdWindowAndFrameToTransparent(sUngradeShopInfo.windowId, 2); // Incorrect use, making it not copy it to vram.
    RemoveWindow(sUngradeShopInfo.windowId);
    UnlockPlayerFieldControls();
    DestroyTask(taskId);

    if (sUngradeShopInfo.callback)
        sUngradeShopInfo.callback();
}

static void Task_GoToBuyOrSellUpgradeMenu(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        SetMainCallback2((void *)((u16)tCallbackHi << 16 | (u16)tCallbackLo));
    }
}

static void MapPostLoadHook_ReturnToUpgradeShopMenu(void)
{
    FadeInFromBlack();
    CreateTask(Task_ReturnToUpgradeShopMenu, 8);
}

static void Task_ReturnToUpgradeShopMenu(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
        DisplayItemMessageOnField(taskId, gText_AnyOtherUpgrade, ShowUpgradeShopMenuAfterExitingBuyOrSellMenu);
}

static void ShowUpgradeShopMenuAfterExitingBuyOrSellMenu(u8 taskId)
{
    CreateUpgradeShopMenu();
    DestroyTask(taskId);
}

static void CB2_BuyUpgradeMenu(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void VBlankCB_BuyUpgradeMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ChangeBgY(2, 96, BG_COORD_SUB);
}

static void CB2_InitBuyUpgradeMenu(void)
{
    u8 taskId;

    switch (gMain.state)
    {
    case 0:
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
        SetVBlankHBlankCallbacksToNull();
        CpuFastFill(0, (void *)OAM, OAM_SIZE);
        ScanlineEffect_Stop();
        ResetTempTileDataBuffers();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        ClearScheduledBgCopiesToVram();
        sUpgradeShopData = AllocZeroed(sizeof(struct UpgradeShopData));
        sUpgradeShopData->scrollIndicatorsTaskId = TASK_NONE;
        sUpgradeShopData->itemSpriteIds[0] = SPRITE_NONE;
        sUpgradeShopData->itemSpriteIds[1] = SPRITE_NONE;
        BuyUpgradeMenuBuildListMenuTemplate();
        BuyUpgradeMenuInitBgs();
        FillBgTilemapBufferRect_Palette0(0, 0, 0, 0, 0x20, 0x20);
        FillBgTilemapBufferRect_Palette0(1, 0, 0, 0, 0x20, 0x20);
        FillBgTilemapBufferRect_Palette0(2, 0, 0, 0, 0x20, 0x20);
        FillBgTilemapBufferRect_Palette0(3, 0, 0, 0, 0x20, 0x20);
        BuyUpgradeMenuInitWindows();
        BuyUpgradeMenuDecompressBgGraphics();
        gMain.state++;
        break;
    case 1:
        if (!FreeTempTileDataBuffersIfPossible())
            gMain.state++;
        break;
    default:
        BuyUpgradeMenuDrawGraphics();
        BuyUpgradeMenuAddScrollIndicatorArrows();
        taskId = CreateTask(Task_BuyUpgradeMenu, 8);
        gTasks[taskId].tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, 0, 0);
        BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB_BuyUpgradeMenu);
        SetMainCallback2(CB2_BuyUpgradeMenu);
        break;
    }
}

static void BuyUpgradeMenuFreeMemory(void)
{
    Free(sUpgradeShopData);
    Free(sListMenuItems);
    Free(sItemNames);
    FreeAllWindowBuffers();
}

static void BuyUpgradeMenuBuildListMenuTemplate(void)
{
    u16 i;

    sListMenuItems = Alloc((sUngradeShopInfo.itemCount + 1) * sizeof(*sListMenuItems));
    sItemNames = Alloc((sUngradeShopInfo.itemCount + 1) * sizeof(*sItemNames));
    for (i = 0; i < sUngradeShopInfo.itemCount; i++)
        BuyUpgradeMenuSetListEntry(&sListMenuItems[i], sUngradeShopInfo.itemList[i], sItemNames[i]);

    StringCopy(sItemNames[i], gText_Cancel2);
    sListMenuItems[i].name = sItemNames[i];
    sListMenuItems[i].id = LIST_CANCEL;

    gMultiuseListMenuTemplate = sShopBuyMenuListTemplate;
    gMultiuseListMenuTemplate.items = sListMenuItems;
    gMultiuseListMenuTemplate.totalItems = sUngradeShopInfo.itemCount + 1;
    if (gMultiuseListMenuTemplate.totalItems > MAX_ITEMS_SHOWN)
        gMultiuseListMenuTemplate.maxShowed = MAX_ITEMS_SHOWN;
    else
        gMultiuseListMenuTemplate.maxShowed = gMultiuseListMenuTemplate.totalItems;

    sUpgradeShopData->itemsShowed = gMultiuseListMenuTemplate.maxShowed;
}

static void BuyUpgradeMenuSetListEntry(struct ListMenuItem *menuItem, u16 item, u8 *name)
{
    CopyItemName(item, name);
    menuItem->name = name;
    menuItem->id = item;
}

static void BuyUpgradeMenuPrintDescriptionAndShowIcon(s32 item, bool8 onInit, struct ListMenu *list)
{
    const u8 *description;
    if (onInit != TRUE)
        PlaySE(SE_SELECT);

    if (item != LIST_CANCEL)
        BuyUpgradeMenuAddItemIcon(item, sUpgradeShopData->iconSlot);
    else
        BuyUpgradeMenuAddItemIcon(ITEM_LIST_END, sUpgradeShopData->iconSlot);
    BuyUpgradeMenuRemoveItemIcon(item, sUpgradeShopData->iconSlot ^ 1);
    sUpgradeShopData->iconSlot ^= 1;
    if (item != LIST_CANCEL)
        description = GetItemDescription(item);
    else
        description = gText_QuitUpgrading;

    FillWindowPixelBuffer(WIN_ITEM_DESCRIPTION, PIXEL_FILL(0));
    BuyUpgradeMenuPrint(WIN_ITEM_DESCRIPTION, description, 3, 1, 0, COLORID_NORMAL);
}

static void BuyUpgradeMenuPrintPriceInList(u8 windowId, u32 itemId, u8 y)
{
    u8 x;

    if (itemId != LIST_CANCEL)
    {
        ConvertIntToDecimalStringN(
            gStringVar1,
            GetItemPrice(itemId),
            STR_CONV_MODE_LEFT_ALIGN,
            6);

        if (CheckBagHasItem(itemId, 1) || CheckPCHasItem(itemId, 1))
            StringCopy(gStringVar4, gText_UpgradeAcquired);
        else
            StringExpandPlaceholders(gStringVar4, gText_PokedollarVar1);
        x = GetStringRightAlignXOffset(FONT_NARROW, gStringVar4, 120);
        AddTextPrinterParameterized4(windowId, FONT_NARROW, x, y, 0, 0, sUpgradeShopBuyMenuTextColors[COLORID_ITEM_LIST], TEXT_SKIP_DRAW, gStringVar4);
    }
}

static void BuyUpgradeMenuAddScrollIndicatorArrows(void)
{
    if (sUpgradeShopData->scrollIndicatorsTaskId == TASK_NONE && sUngradeShopInfo.itemCount + 1 > MAX_ITEMS_SHOWN)
    {
        sUpgradeShopData->scrollIndicatorsTaskId = AddScrollIndicatorArrowPairParameterized(
            SCROLL_ARROW_UP,
            172,
            12,
            148,
            sUngradeShopInfo.itemCount - (MAX_ITEMS_SHOWN - 1),
            TAG_SCROLL_ARROW,
            TAG_SCROLL_ARROW,
            &sUpgradeShopData->scrollOffset);
    }
}

static void BuyUpgradeMenuRemoveScrollIndicatorArrows(void)
{
    if (sUpgradeShopData->scrollIndicatorsTaskId != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sUpgradeShopData->scrollIndicatorsTaskId);
        sUpgradeShopData->scrollIndicatorsTaskId = TASK_NONE;
    }
}

static void BuyUpgradeMenuPrintCursor(u8 scrollIndicatorsTaskId, u8 colorSet)
{
    u8 y = ListMenuGetYCoordForPrintingArrowCursor(scrollIndicatorsTaskId);
    BuyUpgradeMenuPrint(WIN_ITEM_LIST, gText_SelectorArrow2, 0, y, 0, colorSet);
}

static void BuyUpgradeMenuAddItemIcon(u16 item, u8 iconSlot)
{
    u8 spriteId;
    u8 *spriteIdPtr = &sUpgradeShopData->itemSpriteIds[iconSlot];
    if (*spriteIdPtr != SPRITE_NONE)
        return;
    spriteId = AddItemIconSprite(iconSlot + TAG_ITEM_ICON_BASE, iconSlot + TAG_ITEM_ICON_BASE, item);
    if (spriteId != MAX_SPRITES)
    {
        *spriteIdPtr = spriteId;
        gSprites[spriteId].x2 = 24;
        gSprites[spriteId].y2 = 88;
    }
}

static void BuyUpgradeMenuRemoveItemIcon(u16 item, u8 iconSlot)
{
    u8 *spriteIdPtr = &sUpgradeShopData->itemSpriteIds[iconSlot];
    if (*spriteIdPtr == SPRITE_NONE)
        return;

    FreeSpriteTilesByTag(iconSlot + TAG_ITEM_ICON_BASE);
    FreeSpritePaletteByTag(iconSlot + TAG_ITEM_ICON_BASE);
    DestroySprite(&gSprites[*spriteIdPtr]);
    *spriteIdPtr = SPRITE_NONE;
}

static void BuyUpgradeMenuInitBgs(void)
{
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sUpgradeShopBuyMenuBgTemplates, ARRAY_COUNT(sUpgradeShopBuyMenuBgTemplates));
    SetBgTilemapBuffer(1, sUpgradeShopData->tilemapBuffers[0]);
    SetBgTilemapBuffer(2, sUpgradeShopData->tilemapBuffers[1]);
    SetGpuReg(REG_OFFSET_BG0HOFS, 0);
    SetGpuReg(REG_OFFSET_BG0VOFS, 0);
    SetGpuReg(REG_OFFSET_BG1HOFS, 0);
    SetGpuReg(REG_OFFSET_BG1VOFS, 0);
    SetGpuReg(REG_OFFSET_BG2HOFS, 0);
    SetGpuReg(REG_OFFSET_BG2VOFS, 0);
    SetGpuReg(REG_OFFSET_BG3HOFS, 0);
    SetGpuReg(REG_OFFSET_BG3VOFS, 0);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
    ShowBg(3);
}

static void BuyUpgradeMenuDecompressBgGraphics(void)
{
    DecompressAndCopyTileDataToVram(1, gUpgradeMenu_Gfx, 0, 4, 0);
    DecompressAndCopyTileDataToVram(2, gUpgradeMenu_ScrollGfx, 0, 0, 0);
    DecompressDataWithHeaderWram(gUpgradeMenu_Tilemap, sUpgradeShopData->tilemapBuffers[0]);
    DecompressDataWithHeaderWram(gUpgradeMenu_ScrollTilemap, sUpgradeShopData->tilemapBuffers[1]);
    LoadPalette(gUpgradeMenu_Pal, BG_PLTT_ID(SHOP_MENU_PALETTE_ID), PLTT_SIZE_4BPP);
}

static void BuyUpgradeMenuInitWindows(void)
{
    InitWindows(sUpgradeShopBuyMenuWindowTemplates);
    DeactivateAllTextPrinters();
    LoadUserWindowBorderGfx(WIN_MONEY, 1, BG_PLTT_ID(13));
    LoadMessageBoxGfx(WIN_MONEY, 0xA, BG_PLTT_ID(14));
    PutWindowTilemap(WIN_MONEY);
    PutWindowTilemap(WIN_ITEM_LIST);
    PutWindowTilemap(WIN_ITEM_DESCRIPTION);
}

static void BuyUpgradeMenuPrint(u8 windowId, const u8 *text, u8 x, u8 y, s8 speed, u8 colorSet)
{
    AddTextPrinterParameterized4(windowId, FONT_NORMAL, x, y, 0, 0, sUpgradeShopBuyMenuTextColors[colorSet], speed, text);
}

static void BuyUpgradeMenuDisplayMessage(u8 taskId, const u8 *text, TaskFunc callback)
{
    DisplayMessageAndContinueTask(taskId, WIN_MESSAGE, 10, 14, FONT_NORMAL, GetPlayerTextSpeedDelay(), text, callback);
    ScheduleBgCopyTilemapToVram(0);
}

static void BuyUpgradeMenuDrawGraphics(void)
{
    AddMoneyLabelObject(19, 11);
    PrintMoneyAmountInMoneyBoxWithBorder(WIN_MONEY, 1, 13, GetMoney(&gSaveBlock1Ptr->money));
    ScheduleBgCopyTilemapToVram(0);
    ScheduleBgCopyTilemapToVram(1);
    ScheduleBgCopyTilemapToVram(2);
    ScheduleBgCopyTilemapToVram(3);
}

static void Task_BuyUpgradeMenu(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (!gPaletteFade.active)
    {
        s32 itemId = ListMenu_ProcessInput(tListTaskId);
        ListMenuGetScrollAndRow(tListTaskId, &sUpgradeShopData->scrollOffset, &sUpgradeShopData->selectedRow);

        switch (itemId)
        {
        case LIST_NOTHING_CHOSEN:
            break;
        case LIST_CANCEL:
            PlaySE(SE_SELECT);
            ExitBuyUpgradeMenu(taskId);
            break;
        default:
            PlaySE(SE_SELECT);
            tItemId = itemId;
            ClearWindowTilemap(WIN_ITEM_DESCRIPTION);
            BuyUpgradeMenuRemoveScrollIndicatorArrows();
            BuyUpgradeMenuPrintCursor(tListTaskId, COLORID_GRAY_CURSOR);
            sUpgradeShopData->totalCost = GetItemPrice(itemId);

            if (CheckBagHasItem(itemId, 1) || CheckPCHasItem(itemId, 1))
                BuyUpgradeMenuDisplayMessage(taskId, gText_UpgradeAlreadyAcquired, BuyUpgradeMenuReturnToItemList);
            else if (!IsEnoughMoney(&gSaveBlock1Ptr->money, sUpgradeShopData->totalCost))
                BuyUpgradeMenuDisplayMessage(taskId, gText_UpgradeNotEnough, BuyUpgradeMenuReturnToItemList);
            else
            {
                CopyItemName(itemId, gStringVar1);
                ConvertIntToDecimalStringN(gStringVar2, sUpgradeShopData->totalCost, STR_CONV_MODE_LEFT_ALIGN, 6);
                StringExpandPlaceholders(gStringVar4, gText_YouWantedUpgradeVar1ThatllBeVar2);
                sUpgradeShopData->totalCost = GetItemPrice(tItemId);
                BuyUpgradeMenuDisplayMessage(taskId, gStringVar4, BuyUpgradeMenuConfirmPurchase);
            }
            break;
        }
    }
}

static void BuyUpgradeMenuConfirmPurchase(u8 taskId)
{
    CreateYesNoMenuWithCallbacks(taskId, &sShopBuyUpgradeMenuYesNoWindowTemplates, 1, 0, 0, 1, 13, &sUpgradeShopPurchaseYesNoFuncs);
}

static void BuyUpgradeMenuTryMakePurchase(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    PutWindowTilemap(WIN_ITEM_LIST);

    if (AddBagItem(tItemId, 1) == TRUE)
    {
        GetSetItemObtained(tItemId, FLAG_SET_ITEM_OBTAINED);
        BuyUpgradeMenuDisplayMessage(taskId, gText_UpgradeObtained, BuyUpgradeMenuSubtractMoney);
    }
    else
    {
        BuyUpgradeMenuDisplayMessage(taskId, gText_UpgradeNeedsRoom, BuyUpgradeMenuReturnToItemList);
    }
}

static void BuyUpgradeMenuSubtractMoney(u8 taskId)
{
    IncrementGameStat(GAME_STAT_SHOPPED);
    RemoveMoney(&gSaveBlock1Ptr->money, sUpgradeShopData->totalCost);
    PlaySE(SE_SHOP);
    PrintMoneyAmountInMoneyBox(WIN_MONEY, GetMoney(&gSaveBlock1Ptr->money), 0);

    gTasks[taskId].func = Task_ReturnToUpgradeListAfterItemPurchase;
}

static void Task_ReturnToUpgradeListAfterItemPurchase(u8 taskId)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        PlaySE(SE_SELECT);
        BuyUpgradeMenuReturnToItemList(taskId);
    }
}

static void BuyUpgradeMenuReturnToItemList(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    ClearDialogWindowAndFrameToTransparent(WIN_MESSAGE, FALSE);
    RedrawListMenu(tListTaskId);
    BuyUpgradeMenuPrintCursor(tListTaskId, COLORID_ITEM_LIST);
    PutWindowTilemap(WIN_ITEM_LIST);
    PutWindowTilemap(WIN_ITEM_DESCRIPTION);
    ScheduleBgCopyTilemapToVram(0);
    BuyUpgradeMenuAddScrollIndicatorArrows();
    gTasks[taskId].func = Task_BuyUpgradeMenu;
}

static void ExitBuyUpgradeMenu(u8 taskId)
{
    gFieldCallback = MapPostLoadHook_ReturnToUpgradeShopMenu;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_ExitBuyUpgradeMenu;
}

static void Task_ExitBuyUpgradeMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        RemoveMoneyLabelObject();
        BuyUpgradeMenuFreeMemory();
        SetMainCallback2(CB2_ReturnToField);
        DestroyTask(taskId);
    }
}

#undef tItemId
#undef tListTaskId
#undef tCallbackHi
#undef tCallbackLo

void CreateUpgradeMenu(const u16 *itemsForSale)
{
    CreateUpgradeShopMenu();
    SetUpgradeShopItemsForSale(itemsForSale);
    SetUpgradeShopMenuCallback(ScriptContext_Enable);
}
