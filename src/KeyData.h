// Black And White key layout
const int piano_key_layout[] = {0, 1, 1, 0, 1, 1, 1};

// F#
const KeyboardCode piano_key_keyboard_black_keys[] = {KB_S, KB_D, KB_G, KB_H, KB_J, KB_2, KB_3, KB_5, KB_6, KB_7, KB_9, KB_0};

// C#
const KeyboardCode piano_key_keyboard_white_keys[][2] = {{KB_Z, KB_UNKNOWN}, {KB_X, KB_UNKNOWN}, {KB_C, KB_UNKNOWN}, {KB_V, KB_UNKNOWN}, {KB_B, KB_UNKNOWN}, {KB_N, KB_UNKNOWN}, {KB_M, KB_UNKNOWN}, {KB_Q, KB_COMMA}, {KB_W, KB_PERIOD}, {KB_E, KB_SLASH}, {KB_R, KB_UNKNOWN}, {KB_T, KB_UNKNOWN}, {KB_Y, KB_UNKNOWN}, {KB_U, KB_UNKNOWN}, {KB_I, KB_UNKNOWN}, {KB_O, KB_UNKNOWN}, {KB_P, KB_UNKNOWN}};

constexpr int NotesBlackNum = sizeof(piano_key_keyboard_black_keys) / sizeof(piano_key_keyboard_black_keys[0]);
constexpr int NotesWhiteNum = sizeof(piano_key_keyboard_white_keys) / sizeof(piano_key_keyboard_white_keys[0]);
constexpr int AllNotes = NotesBlackNum + NotesWhiteNum;
