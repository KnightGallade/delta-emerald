#ifndef GUARD_CHARACTER_MENU_H
#define GUARD_CHARACTER_MENU_H

void OpenOutfitMenu(MainCallback retCB);
void Task_OpenOutfitMenu(u8 taskId);

//! misc funcs
void BufferOutfitStrings(u8 *dest, u8 outfitId, u8 dataType);
u32 GetPlayerTrainerPicIdByCharacterOutfitType(u32 characterId, u32 outfitId, bool32 type);
const void *GetPlayerHeadGfxOrPal(u8 which, bool32 isFP);
u8 GetOutfitFlag(u8 character, u8 outfit);
void UnlockOutfit(u8 character, u8 outfit);
void LockOutfit(u8 character, u8 outfit);
void ToggleOutfit(u8 character, u8 outfit);
bool8 GetOutfitStatus(u8 character, u8 outfit);
bool8 IsPlayerWearingOutfit(u16 id);
u32 GetOutfitPrice(u16 id);

#endif //! GUARD_CHARACTER_MENU_H