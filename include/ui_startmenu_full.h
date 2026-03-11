#ifndef GUARD_UI_START_MENU_H
#define GUARD_UI_START_MENU_H

#include "main.h"

void Task_OpenStartMenuFullScreen(u8 taskId);
void StartMenuFull_Init(MainCallback callback);

// Define the different menus
enum MenuTypes {
    MENU_DEFAULT_MALE = 0,
    MENU_DEFAULT_FEMALE,
    MENU_FRLG_RED,
};


#endif // GUARD_UI_MENU_H
