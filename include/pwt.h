#ifndef GUARD_PWT_H
#define GUARD_PWT_H

int GetPWTTrainerSelectedMons(u16 tournamentTrainerId);
int TrainerIdToPWTTournamentId(u16 trainerId);
void CopyPWTTrainerText(u8 whichText, u16 trainerId);
u8 GetPWTTrainerFrontSpriteId(u16 trainerId);
u8 SetPWTPtrsGetLevel(void);
void GetPWTTrainerName(u8 *dst, u16 trainerId);
enum TrainerClassID GetPWTOpponentClass(u16 trainerId);

struct PWTTrainerData
{
    u8 facilityClass;
    u16 trainerObjectGfxId;
    u8 trainerName[PLAYER_NAME_LENGTH + 1];
    const u8 *speechBefore;
    const u8 *speechWin;
    const u8 *speechLose;
    u16 mons[PARTY_SIZE];
    u8 title;
    const u8 *battleData1; // Originally tournament ranking
    const u8 *battleData2; // Originally battle style
    const u8 *battleData3; // Originally stat emphasis
};

#endif // GUARD_BATTLE_DOME_H
