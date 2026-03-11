#ifndef GUARD_CONSTANTS_OUTFITS_H
#define GUARD_CONSTANTS_OUTFITS_H

//! macro modes

// ScrCmd_getoutfitstatus
#define OUTFIT_CHECK_FLAG 0
#define OUTFIT_CHECK_USED 1
// ScrCmd_toggleoutfit
#define OUTFIT_TOGGLE_UNLOCK 0
#define OUTFIT_TOGGLE_LOCK 1
// BufferOutfitStrings
#define OUTFIT_BUFFER_NAME  0
#define OUTFIT_BUFFER_DESC  1

// Outfits
enum {
    OUTFIT_DEFAULT = 0,
    OUTFIT_OLD,
    OUTFIT_NEW,
    OUTFIT_FOURTH,
    OUTFIT_COUNT
};

#endif //! GUARD_CONSTANTS_OUTFITS_H