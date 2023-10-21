#include <cstring>

#include "PianoWorld.h"

// Black And White key layout
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

std::string GetDataDir()
{
#ifndef DATA_NOTES_DIR
    std::string __dataDirectory = Path::app_dir();
    __dataDirectory += "data";
#else
    std::string __dataDirectory {DATA_NOTES_DIR};
#endif
    return __dataDirectory;
}

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
    spr_black = Primitive::CreateSpriteRectangle(true, size / 2, Color::black);
    spr_black_hover = Primitive::CreateSpriteRectangle(true, size / 2, Color::gray);
    spr_white = Primitive::CreateSpriteRectangle(true, size - size * 0.05f, Color::white);
    spr_white_hover = Primitive::CreateSpriteRectangle(true, size - size * 0.05f / 7, Color::lightgray);
    std::string notesDir {GetDataDir() += "/sounds/"};

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
        notes[x].render = Primitive::CreateEmptyGameObject()->AddComponent<SpriteRenderer>();
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
        notes[x + NotesWhiteNum].render->setSprite(spr_black);
        notes[x + NotesWhiteNum].normal = spr_black;
        notes[x + NotesWhiteNum].hover = spr_black_hover;
    }

    // write white tones
    for(x = 0, y = NotesWhiteNum; x < NotesWhiteNum; ++x)
    {
        notes[x].render->setSprite(spr_white);
        notes[x].normal = spr_white;
        notes[x].hover = spr_white_hover;

        // set black notes position
        if(piano_key_layout[x % (sizeof(piano_key_layout) / sizeof(piano_key_layout[0]))])
        {
            offset_left = notes[x].render->transform()->position();
            offset_left.x -= size.x / 2;
            offset_left.y += notes[y].render->getSize().y / 2;
            notes[y].render->transform()->position(offset_left);
            notes[y].render->transform()->layer(2);
            ++y;
        }
    }

    World::self()->GetGUI()->PushLabel("Volume:", Vec2Int::right * 5 + Vec2Int::up * 5);
    World::self()->GetGUI()->PushSlider(1, Vec2Int::right * 100, OnVolumeChange);
}

int mousetouched;

void PianoPlayer::OnUpdate()
{
    Vec2 ms = Camera::ScreenToWorldPoint(Input::GetMousePointf());

    mousetouched = -1;
    for(int note = NotesWhiteNum + NotesBlackNum - 1; note > -1; --note)
    {
        if((Input::GetMouseDown(MouseState::MouseLeft) && mousetouched == -1))
        {
            SpriteRenderer *noteRenderer = notes[note].render;

            Vec2 notePos = noteRenderer->transform()->position();
            Rectf noteArea = {notePos - noteRenderer->getSprite()->size() / 2, noteRenderer->getSprite()->size()};
            if(Vec2::HasIntersection(ms, noteArea))
            {
                mousetouched = note;
            }
        }

        if(Input::GetKeyDown(notes[note].key) || mousetouched == note)
        {
            if(notes[note].state == false)
            {
                notes[note].state = true;
                notes[note].render->setSprite(notes[note].hover);
                notes[note].source->Play();
            }
        }
        else if(Input::GetKeyUp(notes[note].key))
        {
            notes[note].state = false;
            notes[note].render->setSprite(notes[note].normal);
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
    if(mousetouched != -1)
    {
        Gizmos::SetColor(Color::white);
        Rect area;
        Rectf noteArea = {
            notes[mousetouched].render->transform()->position() ,
            notes[mousetouched].render->getSprite()->size()};
        Gizmos::DrawRectangle(noteArea.getXY(), noteArea.w, noteArea.h);
    }
}
