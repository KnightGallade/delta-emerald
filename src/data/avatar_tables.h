#include "constants/global.h"
#include "constants/event_objects.h"

static const u16 sRegionMapPlayerIcon_BrendanGfx[] = INCBIN_U16("graphics/pokenav/region_map/brendan_icon.4bpp");
static const u16 sRegionMapPlayerIcon_RSBrendanGfx[] = INCBIN_U16("graphics/pokenav/region_map/rs_brendan_icon.4bpp");
static const u16 sRegionMapPlayerIcon_ORASBrendanGfx[] = INCBIN_U16("graphics/pokenav/region_map/oras_brendan_icon.4bpp");
static const u16 sRegionMapPlayerIcon_FRLGRedGfx[] = INCBIN_U16("graphics/pokenav/region_map/frlg_red_icon.4bpp");

static const u16 sRegionMapPlayerIcon_MayGfx[] = INCBIN_U16("graphics/pokenav/region_map/may_icon.4bpp");
static const u16 sRegionMapPlayerIcon_RSMayGfx[] = INCBIN_U16("graphics/pokenav/region_map/rs_may_icon.4bpp");
static const u16 sRegionMapPlayerIcon_ORASMayGfx[] = INCBIN_U16("graphics/pokenav/region_map/oras_may_icon.4bpp");
static const u16 sRegionMapPlayerIcon_FRLGLeafGfx[] = INCBIN_U16("graphics/pokenav/region_map/frlg_leaf_icon.4bpp");

//! TODO: Should the gfx here be seperated?

static const u8 sFrontierPassPlayerIcons_BrendanMay_Gfx[] = INCBIN_U8("graphics/frontier_pass/map_heads.4bpp");
static const u8 sFrontierPassPlayerIcons_RSBrendanMay_Gfx[] = INCBIN_U8("graphics/frontier_pass/rs_map_heads.4bpp");
// TODO - add a new frontier pass icon for oras and frlg
static const u8 sFrontierPassPlayerIcons_ORASBrendanMay_Gfx[] = INCBIN_U8("graphics/frontier_pass/map_heads.4bpp");
static const u8 sFrontierPassPlayerIcons_FRLGRedLeaf_Gfx[] = INCBIN_U8("graphics/frontier_pass/map_heads.4bpp");

#define REGION_MAP_GFX(m, f) { sRegionMapPlayerIcon_ ## m ## Gfx, sRegionMapPlayerIcon_ ## f ## Gfx }

const struct Outfit gOutfits[CHARACTER_COUNT][OUTFIT_COUNT] =
{
    [CHARACTER_PLAYER] = {
        [OUTFIT_DEFAULT] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = NULL,
        },
        [OUTFIT_OLD] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_PLAYER_OLD_UNLOCKED,
        },
        [OUTFIT_NEW] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_PLAYER_NEW_UNLOCKED,
        },
        [OUTFIT_FOURTH] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_PLAYER_FOURTH_UNLOCKED,
        },
    },
    [CHARACTER_RED] = {
        [OUTFIT_DEFAULT] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_RED_DEFAULT_UNLOCKED,
        },
        [OUTFIT_OLD] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_RED_OLD_UNLOCKED,
        },
        [OUTFIT_NEW] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_RED_NEW_UNLOCKED,
        },
        [OUTFIT_FOURTH] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_RED_FOURTH_UNLOCKED,
        },
    },
    [CHARACTER_LEAF] = {
        [OUTFIT_DEFAULT] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_LEAF_DEFAULT_UNLOCKED,
        },
        [OUTFIT_OLD] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_LEAF_OLD_UNLOCKED,
        },
        [OUTFIT_NEW] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_LEAF_NEW_UNLOCKED,
        },
        [OUTFIT_FOURTH] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_LEAF_FOURTH_UNLOCKED,
        },
    },
    [CHARACTER_GOLD] = {
        [OUTFIT_DEFAULT] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_GOLD_DEFAULT_UNLOCKED,
        },
        [OUTFIT_OLD] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_GOLD_OLD_UNLOCKED,
        },
        [OUTFIT_NEW] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_GOLD_NEW_UNLOCKED,
        },
        [OUTFIT_FOURTH] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_GOLD_FOURTH_UNLOCKED,
        },
    },
    [CHARACTER_KRIS] = {
        [OUTFIT_DEFAULT] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_KRIS_DEFAULT_UNLOCKED,
        },
        [OUTFIT_OLD] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_KRIS_OLD_UNLOCKED,
        },
        [OUTFIT_NEW] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_KRIS_NEW_UNLOCKED,
        },
        [OUTFIT_FOURTH] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_KRIS_FOURTH_UNLOCKED,
        },
    },
    [CHARACTER_BRENDAN] = {
        [OUTFIT_DEFAULT] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_BRENDAN_DEFAULT_UNLOCKED,
        },
        [OUTFIT_OLD] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_BRENDAN_OLD_UNLOCKED,
        },
        [OUTFIT_NEW] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_BRENDAN_NEW_UNLOCKED,
        },
        [OUTFIT_FOURTH] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_BRENDAN_NEW_UNLOCKED,
        },
    },
    [CHARACTER_MAY] = {
        [OUTFIT_DEFAULT] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_MAY_DEFAULT_UNLOCKED,
        },
        [OUTFIT_OLD] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_MAY_OLD_UNLOCKED,
        },
        [OUTFIT_NEW] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_MAY_NEW_UNLOCKED,
        },
        [OUTFIT_FOURTH] = {
            .isHidden = FALSE,
            .price = 100,
            .name = COMPOUND_STRING("Name"),
            .desc = COMPOUND_STRING("Placeholder"),
            .trainerPics = { TRAINER_PIC_BRENDAN, TRAINER_BACK_PIC_BRENDAN, },
            .avatarGfxIds = {
                [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_BRENDAN_NORMAL,
                [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,
                [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_BRENDAN_SURFING,
                [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_BRENDAN_UNDERWATER
            },
            .animGfxIds = {
                [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE,
                [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_BRENDAN_FISHING,
                [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_BRENDAN_WATERING,
                [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_BRENDAN_DECORATING,
                [PLAYER_AVATAR_GFX_VSSEEKER] =   OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE
            },
            .iconRM = sRegionMapPlayerIcon_BrendanGfx,
            .iconFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
            .unlockFlag = FLAG_AVATAR_MAY_FOURTH_UNLOCKED,
        },
    },
};
