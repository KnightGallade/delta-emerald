#ifndef GUARD_PWT_H
#define GUARD_PWT_H

void CopyPWTTrainerName(u8 *, u16);
u8 GetPWTTrainerFrontSpriteId(u16);
enum TrainerClassID GetPWTOpponentClass(u16);
void CopyPWTTrainerText(u8, u16);

struct PWTTrainerData
{
    u8 facilityClass;
    u16 trainerObjectGfxId;
    u8 trainerName[TRAINER_NAME_LENGTH + 1];
    const u8 *speechBefore;
    const u8 *speechWin;
    const u8 *speechLose;
    u16 mons[PARTY_SIZE];
    u16 monPool[PWT_REGULAR_POOL_SIZE];
    u16 megaPool[PWT_MEGA_POOL_SIZE];
    u16 legendPool[PWT_LEGEND_POOL_SIZE];
    u8 rank;
    const u8 *battleData1; // Originally tournament ranking
    const u8 *battleData2; // Originally battle style
    const u8 *battleData3; // Originally stat emphasis
};

#endif // GUARD_PWT_H