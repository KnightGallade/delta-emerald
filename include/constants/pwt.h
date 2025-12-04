#ifndef GUARD_CONSTANTS_PWT_H
#define GUARD_CONSTANTS_PWT_H

// Battle modes
#define PWT_MODE_SINGLES      0
#define PWT_MODE_DOUBLES      1
#define PWT_MODE_MULTIS       2
#define PWT_MODE_LINK_MULTIS  3
#define PWT_MODE_COUNT        4

// Tournament Ids
#define PWT_TOURNAMENT_NONE         0
#define PWT_TOURNAMENT_DRIFTVEIL    1
#define PWT_TOURNAMENT_ROYALE       2
#define PWT_TOURNAMENT_COUNT        3 // Maybe will use?

#define PWT_ROUND1        0
#define PWT_ROUND2        1
#define PWT_SEMIFINAL     2
#define PWT_FINAL         3
#define PWT_ROUNDS_COUNT  4

//#define PWT_TOURNAMENT_SIZE 16 -- defined in global
#define PWT_MATCHES_COUNT  PWT_TOURNAMENT_SIZE - 1

#define PWT_BATTLE_PARTY_SIZE  3

#define PWT_PLAYER_WON_MATCH  1
#define PWT_PLAYER_LOST_MATCH 2
#define PWT_PLAYER_RETIRED    9

#define PWT_FUNC_INIT                       0
#define PWT_FUNC_GET_DATA                   1
#define PWT_FUNC_SET_DATA                   2
// #define PWT_FUNC_GET_ROUND_TEXT             3
#define PWT_FUNC_GET_OPPONENT_NAME          4
#define PWT_FUNC_INIT_OPPONENT_PARTY        5
#define PWT_FUNC_SHOW_OPPONENT_INFO         6
#define PWT_FUNC_SHOW_TOURNEY_TREE          7
// #define PWT_FUNC_SHOW_PREV_TOURNEY_TREE     8
#define PWT_FUNC_SET_OPPONENT_ID            9
#define PWT_FUNC_SET_OPPONENT_GFX           10
#define PWT_FUNC_SHOW_STATIC_TOURNEY_TREE   11
// #define PWT_FUNC_RESOLVE_WINNERS            12
// #define PWT_FUNC_SAVE                       13
// #define PWT_FUNC_INCREMENT_STREAK           14
#define PWT_FUNC_SET_TRAINERS               15
#define PWT_FUNC_RESET_SKETCH_AFTER_BATTLE  16
#define PWT_FUNC_RESTORE_HELD_ITEMS         17
#define PWT_FUNC_REDUCE_PARTY               18
// #define PWT_FUNC_COMPARE_SEEDS              19
// #define PWT_FUNC_GET_WINNER_NAME            20
// #define PWT_FUNC_INIT_RESULTS_TREE          21
#define PWT_FUNC_INIT_TRAINERS              22
#define PWT_FUNC_CHECK_INELIGIBLE           23
#define PWT_FUNC_SET_PARTY_ORDER            24
#define PWT_FUNC_RESET_SKETCH_BEFORE_BATTLE 25
#define PWT_FUNC_GET_OPPONENT_INTRO         26

#define PWT_DATA_SELECTED_MON_ORDER         0
#define PWT_DATA_BATTLE_NUM                 1
#define PWT_DATA_SELECTED_MONS              2
// #define DOME_DATA_WIN_STREAK              0
// #define DOME_DATA_WIN_STREAK_ACTIVE       1
// #define DOME_DATA_ATTEMPTED_SINGLES_50    2
// #define DOME_DATA_ATTEMPTED_SINGLES_OPEN  3
// #define DOME_DATA_HAS_WON_SINGLES_50      4
// #define DOME_DATA_HAS_WON_SINGLES_OPEN    5
// #define DOME_DATA_ATTEMPTED_CHALLENGE     6
// #define DOME_DATA_HAS_WON_CHALLENGE       7
// #define DOME_DATA_PREV_TOURNEY_TYPE       9

// ID for Exit/Cancel on the tourney tree
#define PWT_TOURNEY_TREE_CLOSE_BUTTON  31

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

#define PWT_CARD_ALTERNATE_SLOT (1 << 0) // When set, uses an alternate slot to store the incoming card sprites
#define PWT_MOVE_CARD_RIGHT     (1 << 1)
#define PWT_MOVE_CARD_DOWN      (1 << 2)
#define PWT_MOVE_CARD_LEFT      (1 << 3)
#define PWT_MOVE_CARD_UP        (1 << 4)
#define PWT_MOVE_CARD           (PWT_MOVE_CARD_RIGHT | PWT_MOVE_CARD_DOWN | PWT_MOVE_CARD_LEFT | PWT_MOVE_CARD_UP)

// Text IDs for sPWTWinTexts
#define PWT_TEXT_NO_WINNER_YET     0
#define PWT_TEXT_WON_USING_MOVE    1
#define PWT_TEXT_CHAMP_USING_MOVE  2
#define PWT_TEXT_WON_ON_FORFEIT    3
#define PWT_TEXT_CHAMP_ON_FORFEIT  4
#define PWT_TEXT_WON_NO_MOVES      5
#define PWT_TEXT_CHAMP_NO_MOVES    6

// Offsets/start positions within sPWTOpponentStatsTexts
#define PWT_TEXT_TWO_GOOD_STATS   0
#define PWT_TEXT_ONE_GOOD_STAT    15
#define PWT_TEXT_TWO_BAD_STATS    21
#define PWT_TEXT_ONE_BAD_STAT     36
#define PWT_TEXT_WELL_BALANCED    42
#define PWT_TEXT_HP    0
#define PWT_TEXT_ATK   5
#define PWT_TEXT_DEF   9
#define PWT_TEXT_SPEED 12
#define PWT_TEXT_SPATK 14

// Determine Battle Dome trainers battle styles
#define PWT_MOVE_POINTS_COMBO      0   // Moves that work well in combination: moves that cause or are affected by weather/terrain, Stockpile+, entry hazards, sleep inflictions & effects benefiting from it, offensive boosts of a single stat with at least two stages or at least two stats, and several other effects
#define PWT_MOVE_POINTS_STAT_RAISE 1
#define PWT_MOVE_POINTS_STAT_LOWER 2
#define PWT_MOVE_POINTS_RARE       3   // Moves that appear in less than 5% of levelup learnsets
#define PWT_MOVE_POINTS_HEAL       4   // Moves that heal
#define PWT_MOVE_POINTS_RISKY      5   // Move effects deemed risky by the Emerald developers (excluding High Jump Kick and others for some reason)
#define PWT_MOVE_POINTS_STATUS     6   // Moves that cause status effects without dealing damage
#define PWT_MOVE_POINTS_DMG        7   // Moves that deal damage (BP > 0)
#define PWT_MOVE_POINTS_DEF        8   // Moves like screens, accuracy-lowers or evasiveness-raisers, (special) defense raisers, protect etc.
#define PWT_MOVE_POINTS_ACCURATE   9   // Moves with 100% accuracy (or that are guaranteed hits)
#define PWT_MOVE_POINTS_POWERFUL   10  // Moves with 100 BP or more
#define PWT_MOVE_POINTS_POPULAR    11  // TM/HM moves with 90 BP or more or those that raise a single offensive stat by at least 2 stages
#define PWT_MOVE_POINTS_LUCK       12  // Move effects that depend on luck and moves with Accuracy of <= 50%
#define PWT_MOVE_POINTS_STRONG     13  // Moves with 90 BP or more
#define PWT_MOVE_POINTS_LOW_PP     14  // Moves with 5 PP or less
#define PWT_MOVE_POINTS_EFFECT     15  // Moves with additional effects
#define NUM_PWT_MOVE_POINT_TYPES   16

// Different trainer titles, gives more weight towards them winning battles NPC vs NPC, and sets Pokéballs
#define PWT_TITLE_TRAINER          0
#define PWT_TITLE_LEADER           1
#define PWT_TITLE_IMPORTANT_NPC    2
#define PWT_TITLE_ELITE            3
#define PWT_TITLE_RIVAL            4
#define PWT_TITLE_CHAMPION         5
#define PWT_TITLE_COUNT            6

// WIN/LOSE states
#define PWT_BEFORE_TEXT            0
#define PWT_PLAYER_LOST_TEXT       1
#define PWT_PLAYER_WON_TEXT        2

#endif //GUARD_CONSTANTS_PWT_H
