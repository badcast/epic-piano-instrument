#pragma once

#include "PianoWorld.h"
/*
 ____  _
|  _ \(_) __ _ _ __   ___
| |_) | |/ _` | '_ \ / _ \
|  __/| | (_| | | | | (_) |
|_|   |_|\__,_|_| |_|\___/

 ____             _         _____             _
|  _ \ ___  _ __ (_)_ __   | ____|_ __   __ _(_)_ __   ___
| |_) / _ \| '_ \| | '_ \  |  _| | '_ \ / _` | | '_ \ / _ \
|  _ < (_) | | | | | | | | | |___| | | | (_| | | | | |  __/
|_| \_\___/|_| |_|_|_| |_| |_____|_| |_|\__, |_|_| |_|\___|
                                        |___/
*/

// Black And White key layout
constexpr int keyLayouts[] = {0, 1, 1, 0, 1, 1, 1};
// F#
constexpr KeyboardCode shortcutBlackKeys[] = {KeyboardCode::KB_S, KeyboardCode::KB_D, KeyboardCode::KB_G, KeyboardCode::KB_H, KeyboardCode::KB_J, KeyboardCode::KB_2, KeyboardCode::KB_3, KeyboardCode::KB_5, KeyboardCode::KB_6, KeyboardCode::KB_7, KeyboardCode::KB_9, KeyboardCode::KB_0};
// C#
constexpr KeyboardCode shortcutWhiteKeys[][2] = {{KB_Z, KB_UNKNOWN}, {KB_X, KB_UNKNOWN}, {KB_C, KB_UNKNOWN}, {KB_V, KB_UNKNOWN}, {KB_B, KB_UNKNOWN}, {KB_N, KB_UNKNOWN}, {KB_M, KB_UNKNOWN}, {KB_Q, KB_COMMA}, {KB_W, KB_PERIOD}, {KB_E, KB_SLASH}, {KB_R, KB_UNKNOWN}, {KB_T, KB_UNKNOWN}, {KB_Y, KB_UNKNOWN}, {KB_U, KB_UNKNOWN}, {KB_I, KB_UNKNOWN}, {KB_O, KB_UNKNOWN}, {KB_P, KB_UNKNOWN}};
constexpr int NotesBlackNum = sizeof(shortcutBlackKeys) / sizeof(shortcutBlackKeys[0]);
constexpr int NotesWhiteNum = sizeof(shortcutWhiteKeys) / sizeof(shortcutWhiteKeys[0]);
constexpr int AllNotes = NotesBlackNum + NotesWhiteNum;
