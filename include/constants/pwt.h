#ifndef GUARD_CONSTANTS_PWT_H
#define GUARD_CONSTANTS_PWT_H

// Each round in the tournament
#define PWT_ROUND1        0
#define PWT_ROUND2        1
#define PWT_SEMIFINAL     2
#define PWT_FINAL         3
#define PWT_ROUNDS_COUNT  4

//#define PWT_TOURNAMENT_SIZE 16 -- defined in global
#define PWT_MATCHES_COUNT  PWT_TOURNAMENT_SIZE - 1

// Results
// #define PWT_PLAYER_WON_MATCH  1
// #define PWT_PLAYER_LOST_MATCH 2
// #define PWT_PLAYER_RETIRED    9

// Tournament Types
#define PWT_TOURNAMENT_TYPE_INDIGO_LEAGUE 0
#define PWT_TOURNAMENT_TYPE_HOENN         1
#define PWT_TOURNAMENT_TYPE_ROYALE        2

// Battle Styles
#define PWT_BATTLE_STYLE_SINGLES 0
#define PWT_BATTLE_STYLE_DOUBLES 1

// Function calls
#define PWT_FUNC_CHECK_INELIGIBLE      0
#define PWT_FUNC_SET_DATA              1
#define PWT_FUNC_GET_DATA              2
#define PWT_FUNC_INIT                  3
#define PWT_FUNC_INIT_TRAINERS         4
#define PWT_FUNC_SET_TRAINERS          5
#define PWT_FUNC_SHOW_OPPONENT_CARD    6
#define PWT_FUNC_SHOW_TOURNEY_TREE     7
#define PWT_FUNC_SET_PLAYER_PARTY      8
#define PWT_FUNC_SET_OPPONENT_ID       9
#define PWT_FUNC_SET_OPPONENT_GFX      10
#define PWT_FUNC_ANNOUNCE_OPPONENT     11
#define PWT_FUNC_BUFFER_BATTLE_STRINGS 12
#define PWT_FUNC_SHOW_OPPONENT_INTRO   13
#define PWT_FUNC_SET_OPPONENT_PARTY    14

// Data to set/get
#define PWT_DATA_TOURNAMENT_TYPE 0
#define PWT_DATA_BATTLE_STYLE    1
#define PWT_DATA_ROUND_NUM       2

// Input IDs on the tourney tree
#define PWT_TOURNEY_TREE_SELECTED_CLOSE   0
#define PWT_TOURNEY_TREE_NO_SELECTION     1
#define PWT_TOURNEY_TREE_SELECTED_TRAINER 2
#define PWT_TOURNEY_TREE_SELECTED_MATCH   3

// Modes for showing the tourney tree info card
#define PWT_INFOCARD_NEXT_OPPONENT  0
#define PWT_INFOCARD_TRAINER        1
#define PWT_INFOCARD_MATCH          2

// Input IDs for the info cards
#define PWT_INFOCARD_INPUT_NONE      0
#define PWT_TRAINERCARD_INPUT_UP     1
#define PWT_TRAINERCARD_INPUT_DOWN   2
#define PWT_TRAINERCARD_INPUT_LEFT   3
#define PWT_TRAINERCARD_INPUT_RIGHT  4
#define PWT_MATCHCARD_INPUT_UP       5
#define PWT_MATCHCARD_INPUT_DOWN     6
#define PWT_MATCHCARD_INPUT_LEFT     7
#define PWT_MATCHCARD_INPUT_RIGHT    8
#define PWT_INFOCARD_INPUT_AB        9

// ID for Exit/Cancel on the tourney tree
#define PWT_TOURNEY_TREE_CLOSE_BUTTON  31

#define PWT_CARD_ALTERNATE_SLOT (1 << 0) // When set, uses an alternate slot to store the incoming card sprites
#define PWT_MOVE_CARD_RIGHT     (1 << 1)
#define PWT_MOVE_CARD_DOWN      (1 << 2)
#define PWT_MOVE_CARD_LEFT      (1 << 3)
#define PWT_MOVE_CARD_UP        (1 << 4)
#define PWT_MOVE_CARD           (PWT_MOVE_CARD_RIGHT | PWT_MOVE_CARD_DOWN | PWT_MOVE_CARD_LEFT | PWT_MOVE_CARD_UP)

// Trainer ranks for NPC v NPC battle weights
#define PWT_RANK_TRAINER          0
#define PWT_RANK_LEADER           1
#define PWT_RANK_IMPORTANT_NPC    2
#define PWT_RANK_ELITE_FOUR       3
#define PWT_RANK_RIVAL            4
#define PWT_RANK_CHAMPION         5
#define PWT_RANK_COUNT            6

// Victory id's, haven't decided if I want to keep this
// TODO - decide
#define PWT_TEXT_NO_WINNER_YET     0
#define PWT_TEXT_WON_USING_MOVE    1
#define PWT_TEXT_CHAMP_USING_MOVE  2
#define PWT_TEXT_WON_ON_FORFEIT    3
#define PWT_TEXT_CHAMP_ON_FORFEIT  4
#define PWT_TEXT_WON_NO_MOVES      5
#define PWT_TEXT_CHAMP_NO_MOVES    6

// WIN/LOSE states
#define PWT_BEFORE_TEXT            0
#define PWT_PLAYER_LOST_TEXT       1
#define PWT_PLAYER_WON_TEXT        2

#endif //GUARD_CONSTANTS_PWT_H