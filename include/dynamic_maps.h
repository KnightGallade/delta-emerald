#ifndef GUARD_DYNAMIC_MAPS_H
#define GUARD_DYNAMIC_MAPS_H

enum DynamicMaps {
    DYNAMIC_MAP_LITTLEROOT = 0,
    DYNAMIC_MAP_PETALBURG,
    DYNAMIC_MAP_COUNT,
};

#define DYNAMIC_MAP_STATE_COUNT 11 // For now max limit, will see in the future

u16 GetDynamicLayoutId(u16 layoutId);
const struct MapLayout *GetDynamicMapLayout(u16 mapLayoutId);
u8 *CopyDynamicMapName(u8 *dest, mapsec_u16_t regionMapId);

#endif // GUARD_DYNAMIC_MAPS_H
