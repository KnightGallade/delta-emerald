#include "global.h"
#include "overworld.h"
#include "event_data.h"
#include "region_map.h"
#include "string_util.h"
#include "constants/layouts.h"
#include "dynamic_maps.h"

extern const struct MapLayout *const gMapLayouts[];

static const u16 sDynamicMapVars[DYNAMIC_MAP_COUNT] =
{
    [DYNAMIC_MAP_LITTLEROOT] = VAR_LITTLEROOT_DEVELOPMENT,
    [DYNAMIC_MAP_PETALBURG] = VAR_PETALBURG_DEVELOPMENT,
};

static const u16 sDynamicMapLayouts[DYNAMIC_MAP_COUNT][DYNAMIC_MAP_STATE_COUNT] =
{
    [DYNAMIC_MAP_LITTLEROOT] = {
        LAYOUT_LITTLEROOT_TOWN_DEVELOPMENT0,
        LAYOUT_LITTLEROOT_TOWN_DEVELOPMENT1,
        LAYOUT_LITTLEROOT_TOWN_DEVELOPMENT2,
        LAYOUT_LITTLEROOT_TOWN_DEVELOPMENT3,
        LAYOUT_LITTLEROOT_TOWN_DEVELOPMENT4,
        LAYOUT_LITTLEROOT_TOWN_DEVELOPMENT5,
        LAYOUT_LITTLEROOT_TOWN_DEVELOPMENT6,
        LAYOUT_LITTLEROOT_TOWN_DEVELOPMENT7,
        LAYOUT_LITTLEROOT_TOWN_DEVELOPMENT8,
        LAYOUT_LITTLEROOT_TOWN_DEVELOPMENT9,
        LAYOUT_LITTLEROOT_TOWN,
    },
    [DYNAMIC_MAP_PETALBURG] = {
        LAYOUT_PETALBURG_CITY_DEVELOPMENT0,
        LAYOUT_PETALBURG_CITY_DEVELOPMENT1,
        LAYOUT_PETALBURG_CITY_DEVELOPMENT2,
        LAYOUT_PETALBURG_CITY_DEVELOPMENT3,
        LAYOUT_PETALBURG_CITY_DEVELOPMENT4,
        LAYOUT_PETALBURG_CITY_DEVELOPMENT5,
        LAYOUT_PETALBURG_CITY_DEVELOPMENT6,
        LAYOUT_PETALBURG_CITY_DEVELOPMENT7,
        LAYOUT_PETALBURG_CITY_DEVELOPMENT8,
        LAYOUT_PETALBURG_CITY_DEVELOPMENT9,
        LAYOUT_PETALBURG_CITY,
    },
};

u16 GetDynamicLayoutId(u16 layoutId)
{
    u8 dynamicMapId, dynamicMapState;
    switch (layoutId)
    {
        case LAYOUT_LITTLEROOT_TOWN:
            dynamicMapId = DYNAMIC_MAP_LITTLEROOT;
            break;
        case LAYOUT_PETALBURG_CITY:
            dynamicMapId = DYNAMIC_MAP_PETALBURG;
            break;
        default:
            return layoutId;
    }
    dynamicMapState = VarGet(sDynamicMapVars[dynamicMapId]);
    return sDynamicMapLayouts[dynamicMapId][dynamicMapState];
}

const struct MapLayout *GetDynamicMapLayout(u16 mapLayoutId)
{
    u16 layoutId = GetDynamicLayoutId(mapLayoutId);
    return gMapLayouts[layoutId - 1];
}

extern const struct RegionMapLocation gRegionMapEntries[];

static const u8 *sDynamicMapNames[DYNAMIC_MAP_COUNT][DYNAMIC_MAP_STATE_COUNT] =
{
    [DYNAMIC_MAP_LITTLEROOT] = {
        COMPOUND_STRING("LARGE-ROOT GROVE"), // max length
        COMPOUND_STRING("LARGE-ROOT GROVE"),
        COMPOUND_STRING("LARGE-ROOT GROVE"),
        COMPOUND_STRING("ROOTED HUT"),
        COMPOUND_STRING("ROOTED HUT"),
        COMPOUND_STRING("ROOTED HUT"),
        COMPOUND_STRING("ROOTED HAMLET"),
        COMPOUND_STRING("ROOTED HAMLET"),
        COMPOUND_STRING("ROOTED VILLAGE"),
        COMPOUND_STRING("ROOTED VILLAGE"),
        COMPOUND_STRING("LITTLEROOT TOWN"),
    },
    [DYNAMIC_MAP_PETALBURG] = {
        COMPOUND_STRING("PETALED SWAMP"),
        COMPOUND_STRING("PETALED SWAMP"),
        COMPOUND_STRING("PETAL CABIN"),
        COMPOUND_STRING("PETAL CABIN"),
        COMPOUND_STRING("PETAL CABIN"),
        COMPOUND_STRING("PETAL SINK"),
        COMPOUND_STRING("PETALBURG PLAINS"),
        COMPOUND_STRING("PETALBURG HAMLET"),
        COMPOUND_STRING("PETALBURG VILL"),
        COMPOUND_STRING("PETALBURG TOWN"),
        COMPOUND_STRING("PETALBURG CITY"),
    },
};

u8 *CopyDynamicMapName(u8 *dest, mapsec_u16_t regionMapId)
{
    u8 dynamicMapId, dynamicMapState;
    switch (regionMapId)
    {
        case MAPSEC_LITTLEROOT_TOWN:
            dynamicMapId = DYNAMIC_MAP_LITTLEROOT;
            break;
        case MAPSEC_PETALBURG_CITY:
            dynamicMapId = DYNAMIC_MAP_PETALBURG;
            break;
        default:
            return StringCopy(dest, gRegionMapEntries[regionMapId].name);
    }
    dynamicMapState = VarGet(sDynamicMapVars[dynamicMapId]);
    return StringCopy(dest, sDynamicMapNames[dynamicMapId][dynamicMapState]);
}
