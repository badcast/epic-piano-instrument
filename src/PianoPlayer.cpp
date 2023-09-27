#include <cstring>

#include "PianoWorld.h"

constexpr int piano_key_layout[] {0, 1, 1, 0, 1, 1, 1};

// F#
constexpr KeyboardCode piano_key_keyboard_black_keys[] {KB_S, KB_D, KB_G, KB_H, KB_J, KB_2, KB_3, KB_5, KB_6, KB_7, KB_9, KB_0};

// C#
constexpr KeyboardCode piano_key_keyboard_white_keys[] {
    KB_Z, KB_X, KB_C, KB_V, KB_B, KB_N, KB_M, KB_Q, KB_W, KB_E, KB_R, KB_T, KB_Y, KB_U, KB_I, KB_O, KB_P};


constexpr int NotesBlackNum = 12;
constexpr int NotesWhiteNum = 17;

resource_id notes_res[NotesWhiteNum + NotesBlackNum];
PianoNote notes[NotesWhiteNum + NotesBlackNum];

void OnVolumeChange(UI::uid, float newValue)
{
    for(int x = 0; x < NotesWhiteNum + NotesBlackNum; ++x)
    {
        notes[x].source->setVolume(newValue);
    }
}

void PianoPlayer::OnAwake()
{
    int x, y;
    char buffer[64];
    const char ext[] = ".ogg";

    // load sprites
    Vec2 size {0.5f, 2.f};
    spr_black = Primitive::create_sprite2D_box(size / 2, Color::black);
    spr_black_hover = Primitive::create_sprite2D_box(size / 2, Color::gray);
    spr_white = Primitive::create_sprite2D_box(size - size * 0.05f, Color::white);
    spr_white_hover = Primitive::create_sprite2D_box(size - size * 0.05f / 7, Color::lightgray);
    std::string notesDir {"./data/sounds/"};

    // Setting White notes
    for(x = 0; x < 7; ++x)
    {
        snprintf(buffer, sizeof(buffer), "C-%d", 6 - x);
        notes[x].name = buffer;
        notes[x].key = piano_key_keyboard_white_keys[x];
    }
    for(; x < 17; ++x)
    {
        snprintf(buffer, sizeof(buffer), "C+%d", x - 7);
        notes[x].name = buffer;
        notes[x].key = piano_key_keyboard_white_keys[x];
    }

    // Setting Black notes
    for(x = 0; x < NotesBlackNum; ++x)
    {
        snprintf(buffer, sizeof(buffer), "F+%d", x);
        notes[x + NotesWhiteNum].name = buffer;
        notes[x + NotesWhiteNum].key = piano_key_keyboard_black_keys[x];
    }

    // init piano sources
    Vec2 offset_left {size.x - size.x * NotesWhiteNum / 2 - size.x * 0.05f * NotesWhiteNum / 2, 0};
    for(x = 0; x < sizeof(notes) / sizeof(notes[0]); ++x)
    {
        // Add sprite and Transform layer
        notes[x].render = Primitive::create_empty_game_object()->AddComponent<SpriteRenderer>();
        notes[x].render->transform()->localPosition(offset_left);
        offset_left += Vec2::right * size.x;

        // load clips
        resource_id note_wav;
        if(!(note_wav = notes_res[x]))
        {
            note_wav = notes_res[x] = Resources::LoadAudioClip(notesDir + notes[x].name + ext, false);
        }
        notes[x].source = this->AddComponent<AudioSource>();
        notes[x].source->setClip(Resources::GetAudioClipSource(note_wav));
        notes[x].source->setVolume(1);
    }

    // write black tones
    for(x = 0; x < NotesBlackNum; ++x)
    {
        notes[x + NotesWhiteNum].render->set_sprite(spr_black);
        notes[x + NotesWhiteNum].normal = spr_black;
        notes[x + NotesWhiteNum].hover = spr_black_hover;
    }

    // write white tones
    for(x = 0, y = NotesWhiteNum; x < NotesWhiteNum; ++x)
    {
        notes[x].render->set_sprite(spr_white);
        notes[x].normal = spr_white;
        notes[x].hover = spr_white_hover;

        // set black notes position
        if(piano_key_layout[x % (sizeof(piano_key_layout) / sizeof(piano_key_layout[0]))])
        {
            offset_left = notes[x].render->transform()->position();
            offset_left.x -= size.x / 2;
            offset_left.y += notes[y].render->size.y / 2;
            notes[y].render->transform()->position(offset_left);
            notes[y].render->transform()->layer = 2;
            ++y;
        }
    }

    World::self()->getGUI()->PushLabel("Volume: ", Vec2Int::right * 5 + Vec2Int::up * 5);
    World::self()->getGUI()->PushSlider(1, Vec2Int::right * 100, OnVolumeChange);
}

void PianoPlayer::OnUpdate()
{
    int mouseNoteSelect = 0;
    // Priority Black Notes
    for(int note = NotesWhiteNum + NotesBlackNum - 1; note > -1; --note)
    {
        if((Input::GetMouseDown(MouseState::MouseLeft) && mouseNoteSelect == 0) && notes[note].render->get_sprite())
        {
            Vec2 ms = Camera::ScreenToWorldPoint(Input::GetMousePointf());
            Vec2 sz {notes[note].render->get_sprite()->rect().getWH() / 100.f};
            Vec2 notePos = Vec2::Abs(notes[note].render->transform()->position() - ms + sz / 2);
            Rectf noteCoord = {Vec2::zero, sz};
            mouseNoteSelect = static_cast<int>(Vec2::HasIntersection(notePos, noteCoord));
        }

        if(Input::GetKeyDown(notes[note].key) || mouseNoteSelect == 1)
        {
            if(notes[note].state == false)
            {
                notes[note].state = true;
                notes[note].render->set_sprite(notes[note].hover);
                notes[note].source->Play();
            }
            mouseNoteSelect = 2;
        }
        else if(Input::GetKeyUp(notes[note].key))
        {
            notes[note].state = false;
            notes[note].render->set_sprite(notes[note].normal);
        }
    }
}

void PianoPlayer::OnGizmos()
{
    Gizmos::DrawTextLegacy(Vec2::up * 2, "Epic Piano Instrument v1.0");
    Gizmos::DrawTextLegacy((Vec2::right + Vec2::down) * 2, "Ronin Engine");

    if(Input::GetMouseDown(MouseState::MouseRight))
    {
        Gizmos::SetColor(Color {0, 0, 0, 100});
        Gizmos::DrawFillRect(Vec2::zero, 10, 5);
        Gizmos::SetColor(Color::white);
        Gizmos::DrawTextLegacy(Vec2::zero, "Reloading...");
        RoninSimulator::ReloadWorld();
    }
}
