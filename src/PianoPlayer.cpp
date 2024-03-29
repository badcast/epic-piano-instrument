#include <cstring>

#include "PianoWorld.h"

// Black And White key layout
constexpr KeyboardCode KEY_NONE {static_cast<KeyboardCode>(0)};

constexpr int piano_key_layout[] {0, 1, 1, 0, 1, 1, 1};
// F#
constexpr KeyboardCode piano_key_keyboard_black_keys[] {KB_S, KB_D, KB_G, KB_H, KB_J, KB_2, KB_3, KB_5, KB_6, KB_7, KB_9, KB_0};

// C#
constexpr KeyboardCode piano_key_keyboard_white_keys[][2] {
    {KB_Z, KEY_NONE},
    {KB_X, KEY_NONE},
    {KB_C, KEY_NONE},
    {KB_V, KEY_NONE},
    {KB_B, KEY_NONE},
    {KB_N, KEY_NONE},
    {KB_M, KEY_NONE},
    {KB_Q, KB_COMMA},
    {KB_W, KB_PERIOD},
    {KB_E, KB_SLASH},
    {KB_R, KEY_NONE},
    {KB_T, KEY_NONE},
    {KB_Y, KEY_NONE},
    {KB_U, KEY_NONE},
    {KB_I, KEY_NONE},
    {KB_O, KEY_NONE},
    {KB_P, KEY_NONE}};

constexpr int NotesBlackNum = 12;
constexpr int NotesWhiteNum = 17;

int mousetouched;

ResId notes_res[NotesWhiteNum + NotesBlackNum];
PianoNote notes[NotesWhiteNum + NotesBlackNum];

std::set<int> __stack_records__ {};

std::string GetDataDir()
{
#ifndef DATA_NOTES_DIR
    std::string __dataDirectory {std::move(Path::app_dir())};
    __dataDirectory += "data";
#else
    std::string __dataDirectory {DATA_NOTES_DIR};
#endif
    return __dataDirectory;
}

void OnTransparencyChange(UI::uid, float newValue)
{
    for(int x = 0; x < NotesWhiteNum + NotesBlackNum; ++x)
    {
        notes[x].render->setColor(Color(Color::white, newValue * 255));
    }
}

void OnVolumeChange(UI::uid, float newValue)
{
    for(int x = 0; x < NotesWhiteNum + NotesBlackNum; ++x)
    {
        notes[x].source->setVolume(newValue);
    }
}

PianoPlayer *playerInstance;

void OnRecordState(UI::uid button)
{
    UI::GUI *gui = World::self()->GetGUI();

    if(!playerInstance->recording())
        playerInstance->beginRecord();
    else
        playerInstance->endRecord();

    if(playerInstance->recording())
        gui->ElementSetText(button, "Recording...");
    else
        gui->ElementSetText(button, "Begin Record");
}

void OnRecordPlay(UI::uid but)
{
    if(playerInstance->recording())
        playerInstance->endRecord();

    // Play recorded notes
    playerInstance->playRecord();
}

void OnRecordStopPlay(UI::uid but)
{
    // Stop play
    playerInstance->stopPlay();
}

void PianoPlayer::OnAwake()
{
    int x, y;
    char buffer[64];
    const char ext[] = ".ogg";
    playerInstance = this;
    std::string notesDir {std::move(GetDataDir() += "/sounds/")};

    enum : int
    {
        LAYER_VISUALBACK = 0,
        LAYER_BACKNOTE  = 2,
        LAYER_PARTICLES = 4,
        LAYER_NOTES     = 6,
    };

    // load sprites
    Vec2 vector1 {0.5f, 2.f};
    Vec2 pianoOffset = {0, -2};

    spr_black = Primitive::CreateSpriteRectangle(true, vector1 / 2, Color::black);
    spr_black_hover = Primitive::CreateSpriteRectangle(true, vector1 / 2, Color::gray);
    spr_white = Primitive::CreateSpriteRectangle(true, vector1 - vector1 * 0.05f, Color::white);
    spr_white_hover = Primitive::CreateSpriteRectangle(true, vector1 - vector1 * 0.05f / 7, Color::lightgray);

    if((visual_background = Primitive::CreateSpriteFrom(
            Resources::GetImageSource(Resources::LoadImage(GetDataDir() + "/layer.png", true)), true)) == nullptr)
    {
        RoninSimulator::Log("Visual sprite is no loaded");
    }

    visual_note = Sprite::CreateWhiteSprite();

    __stack_records__.clear();
    clearRecord();

    // Setting White notes
    for(x = 0; x < 7; ++x)
    {
        snprintf(buffer, sizeof(buffer), "C-%d", 6 - x);
        notes[x].name = buffer;
        std::memcpy(notes[x].keys, piano_key_keyboard_white_keys[x], sizeof(notes[x].keys));
    }
    for(; x < 17; ++x)
    {
        snprintf(buffer, sizeof(buffer), "C+%d", x - 7);
        notes[x].name = buffer;
        std::memcpy(notes[x].keys, piano_key_keyboard_white_keys[x], sizeof(notes[x].keys));
    }

    // Setting Black notes
    for(x = 0; x < NotesBlackNum; ++x)
    {
        snprintf(buffer, sizeof(buffer), "F+%d", x);
        notes[x + NotesWhiteNum].name = buffer;
        notes[x + NotesWhiteNum].keys[0] = piano_key_keyboard_black_keys[x];
        // notes[x + NotesWhiteNum].keys[1] = KEY_NONE;
    }

    // init piano sources
    Vec2 offset_left {vector1.x - vector1.x * NotesWhiteNum / 2 - vector1.x * 0.05f * NotesWhiteNum / 2 + pianoOffset.x, pianoOffset.y};
    for(x = 0; x < sizeof(notes) / sizeof(notes[0]); ++x)
    {
        // Add sprite and Transform layer
        notes[x].render = Primitive::CreateEmptyGameObject()->AddComponent<SpriteRenderer>();
        notes[x].render->transform()->localPosition(offset_left);
        notes[x].render->transform()->layer(LAYER_NOTES);

        offset_left += Vec2::right * vector1.x;

        // load clips
        ResId note_wav;
        if(!(note_wav = notes_res[x]))
        {
            note_wav = notes_res[x] = Resources::LoadAudioClip(notesDir + notes[x].name + ext, false);
        }
        notes[x].source = this->AddComponent<AudioSource>();
        notes[x].source->setClip(Resources::GetAudioClipSource(note_wav));
        notes[x].source->setVolume(1.0f);
    }

    // write black tones
    for(x = 0; x < NotesBlackNum; ++x)
    {
        notes[x + NotesWhiteNum].render->setSprite(spr_black);
        notes[x + NotesWhiteNum].render->transform()->layer(LAYER_NOTES + 1);
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
            offset_left.x -= vector1.x / 2;
            offset_left.y += notes[y].render->getSize().y / 2;
            notes[y].render->transform()->position(offset_left);
            ++y;
        }
    }

    // Generate Visual Layer
    SpriteRenderer *visualObject = Primitive::CreateEmptyGameObject()->AddComponent<SpriteRenderer>();
    visualObject->setSprite(visual_background);
    visualObject->setColor(Color(Color::white, 128));
    visualObject->setSize({1, 2});
    vector1 = notes[0].render->transform()->position();
    vector1.x += visual_background->size().x / 2 - notes[0].render->getSize().x / 2 + 0.25f;
    vector1.y += visual_background->size().y + notes[0].render->getSize().y - 0.1f;

    visualObject->transform()->layer(LAYER_VISUALBACK);
    visualObject->transform()->position(vector1);

    // Generate Renderers
    for(x = 0; x < sizeof(notes) / sizeof(notes[0]); ++x)
    {
        SpriteRenderer *backNote;
        ParticleSystem *p1, *p2;

        p1 = Primitive::CreateEmptyGameObject()->AddComponent<ParticleSystem>();

        p1->emit = false;
        p1->loop = true;
        p1->speed = 3;
        p1->rotate = false;
        p1->destroyAfter = false;
        p1->interval = 0.01f;

        p1->setSource(visual_note);
        p1->setInterpolates(1.3, 0.0, 0.1);
        p1->setSize(Vec2::half / 6);
        p1->setColors(Color(255, 100, 124), Color(255, 100, 124), Color(255, 100, 124, 0));

        p2 = p1->gameObject()->AddComponent<ParticleSystem>();

        p2->emit = false; // default set emit is off
        p2->randomDirection = true;
        p2->destroyAfter = false;
        p2->startWith = 6;
        p2->interval = 0.05f;
        p2->speed = 0.3f;

        p2->setInterpolates(1.5f);

        p2->setSource(Sprite::CreateWhiteSprite());

        p2->setSize(Vec2::one * 0.02f);
        p2->setColors(Color::yellow, Color::yellow, {Color::red, 0});
        p2->transform()->layer(LAYER_PARTICLES);

        p2->transform()->position(
            {notes[x].render->transform()->position().x, notes[0].render->transform()->position().y + notes[0].render->getSize().y});

        _particles.emplace_back(p1, p2);

        backNote = Primitive::CreateEmptyGameObject({p1->transform()->position().x, visualObject->transform()->position().y})
                       ->AddComponent<SpriteRenderer>();
        backNote->transform()->layer(LAYER_BACKNOTE);
        backNote->setSize({0.9f, 3.7});
        backNote->setColor({Color::black, 0});

        backNote->setSprite(spr_black);
        _backDrawNotes.emplace_back(backNote);
    }

    // Generate GUI
    World::self()->GetGUI()->PushLabel("Volume:", Vec2Int::right * 25 + Vec2Int::up * 15);
    World::self()->GetGUI()->PushSlider(1, Vec2Int::right * 100, OnVolumeChange);

    World::self()->GetGUI()->PushLabel("Transparency:", Vec2Int::right * 46 + Vec2Int::up * 30);
    World::self()->GetGUI()->PushSlider(1, {100, 20}, OnTransparencyChange);

    World::self()->GetGUI()->PushButton("Begin Record", Vec2Int {500, 24}, OnRecordState);
    World::self()->GetGUI()->PushButton("P", {740, 24, 24, 30}, OnRecordPlay);
    World::self()->GetGUI()->PushButton("S", {764, 24, 24, 30}, OnRecordStopPlay);
}

void PianoPlayer::OnUpdate()
{
    Vec2 ms = Camera::ScreenToWorldPoint(Input::GetMousePointf());

    // flush
    __stack_records__.clear();

    mousetouched = -1;

    // Auto playing
    if(playing() && records.size() > 2)
    {
        float off = TimeEngine::time() - startPlayback;

        if(off >= curPlayback)
        {
            // select the next tracks
            ++track;

            // Are looping
            if(track == records.size() - 1)
            {
                // Replay
                playRecord();
            }
            curPlayback = records.at(track).first;
        }
    }

    for(int note = NotesWhiteNum + NotesBlackNum - 1, playRecNote = -1; note > -1; --note)
    {
        _particles[note].first->emit = false;
        _particles[note].second->emit = false;

        if((Input::GetMouseDown(MouseButton::MouseLeft) && mousetouched == -1))
        {
            SpriteRenderer *noteRenderer = notes[note].render;

            Vec2 notePos = noteRenderer->transform()->position();
            Rectf noteArea = {notePos - noteRenderer->getSprite()->size() / 2, noteRenderer->getSprite()->size()};
            if(Vec2::HasIntersection(ms, noteArea))
            {
                mousetouched = note;
            }
        }

        if(track != -1)
        {
            std::set<int> *_selNotes = &(records.at(track).second);
            if(_selNotes->find(note) != std::end(*_selNotes))
                playRecNote = note;
            else
                playRecNote = -1;
        }

        if((Input::GetKeyDown(notes[note].keys[0]) || Input::GetKeyDown(notes[note].keys[1])) || mousetouched == note ||
           playRecNote == note)
        {
            if(notes[note].close == false)
            {
                notes[note].close = true;
                notes[note].render->setSprite(notes[note].hover);
                notes[note].source->Play();
            }
            __stack_records__.insert(note);

            // Draw particle
            _particles[note].first->emit = true;
            _particles[note].second->emit = true;
            _backDrawNotes[note]->setColor({Color::black, 127});
        }
        else if(Input::GetKeyUp(notes[note].keys[0]) || Input::GetKeyUp(notes[note].keys[1]))
        {
            notes[note].close = false;
            notes[note].render->setSprite(notes[note].normal);
            _backDrawNotes[note]->setColor(Color::Lerp(_backDrawNotes[note]->getColor(), Color::transparent, TimeEngine::deltaTime() * 2));
        }

    }

    // Can record
    if(recording())
    {
        int cmpt = 1;
        // float lastTime = records.at(records.size() - 1).first;
        curPlayback = TimeEngine::time() - startPlayback;

        if(__stack_records__.size() == records.at(records.size() - 1).second.size())
        {

            /*std::vector*/
            // cmpt = memcmp(records.at(records.size() - 1).second.data(), __stack_records__.data(),__stack_records__.size());

            /*std::set*/
            cmpt = 0; // set of begin compare
            for(int v : __stack_records__)
            {
                for(std::pair<float, std::set<int>> &k : records)
                {
                    if(k.second.find(v) == std::end(k.second))
                    {
                        cmpt = 1; // not have compared notes
                        break;
                    }
                }
                if(cmpt == 1)
                    break;
            }
        }

        // No write last screen if then compare as 0
        if(cmpt != 0)
            // Record
            records.emplace_back(std::move(std::make_pair<float, std::set<int>>(float(curPlayback), std::move(__stack_records__))));
    }
}

void PianoPlayer::OnGizmos()
{
    Gizmos::DrawTextLegacy((Vec2::left + Vec2::down * 3.4f), "Epic Piano Instrument v1.1 | Running on Ronin Engine (badcast)");

    if(Input::GetMouseDown(MouseButton::MouseRight))
    {
        Gizmos::SetColor(Color {Color::black, 160});
        Gizmos::DrawFillRect(Vec2::zero, 16, 10);
        Gizmos::SetColor(Color::white);
        Gizmos::DrawTextLegacy(Vec2::zero, "Reloading...");
        RoninSimulator::ReloadWorld();
    }
    if(mousetouched != -1)
    {
        Gizmos::SetColor(Color::white);
        Rect area;
        Rectf noteArea = {notes[mousetouched].render->transform()->position(), notes[mousetouched].render->getSprite()->size()};
        Gizmos::DrawRectangle(noteArea.GetXY(), noteArea.w, noteArea.h);
    }
}

bool PianoPlayer::recording() const
{
    return records.size() == 1 || records.size() > 1 && records[records.size() - 1].first != -1.0f;
}

bool PianoPlayer::playing() const
{
    return track != -1;
}

void PianoPlayer::beginRecord()
{
    clearRecord();
    startPlayback = TimeEngine::time();
    records.emplace_back(std::move(std::make_pair(0.f, std::move(std::set<int> {}))));
}

void PianoPlayer::endRecord()
{
    if(recording())
    {
        records.emplace_back(std::move(std::make_pair(-1.f, std::move(std::set<int> {}))));
        stopPlay();
    }
}

void PianoPlayer::clearRecord()
{
    stopPlay();
    records.clear();
}

bool PianoPlayer::playRecord()
{
    if(!records.empty() && !recording())
    {
        startPlayback = TimeEngine::time();
        track = 0;         // start play
        curPlayback = 0.0; // reset time
        return true;
    }

    return false;
}

void PianoPlayer::stopPlay()
{
    track = -1;
    startPlayback = 0;
    curPlayback = 0.0; // reset time
}
