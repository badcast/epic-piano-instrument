#include <cstring>

#include <ronin/Debug.h>

#include "PianoWorld.h"

/////////////////////////
//////// MEMBERS ////////
/////////////////////////

#include "KeyData.h"

constexpr auto TitleApplication = "Epic Piano Instrument v1.1 | Running on Ronin Engine (badcast)";

int mousetouched;
ResId notes_res[AllNotes];
PianoNote notes[AllNotes];
std::set<int> __stack_records__ {};

///////////////////////////
//////// FUNCTIONS ////////
///////////////////////////

std::string GetDataDir()
{
#ifndef EPICPIANO_DATA_DIRECTORY
    std::string __dataDirectory {std::move(Paths::GetRuntimeDir())};
    __dataDirectory += "data";
#else
    std::string __dataDirectory {EPICPIANO_DATA_DIRECTORY};
#endif
    return __dataDirectory;
}

const float playTimer()
{
    return Time::startUpTime();
}

PianoPlayer::PianoPlayer() : Behaviour("Piano Player")
{
}

///////////////////////
//////// INITS ////////
///////////////////////

void PianoPlayer::OnAwake()
{
    enum : int
    {
        LAYER_VISUALBACK = 0,
        LAYER_BACKNOTE = 2,
        LAYER_PARTICLES = 4,
        LAYER_NOTES = 6,
    };

    int x, y;
    char buffer[64];
    const char ext[] = ".ogg";

    standbyAfter = 5.0f;
    standbyTickOut = 0.0f;
    paramEFXRemainder = true;

    std::string notesDir {std::move(GetDataDir() += "/sounds/")};

    Vec2 vector1 {0.5f, 2.f};
    Vec2 pianoOffset = {0, -2};

    spriteBlack = Primitive::CreateSpriteRectangle(vector1 / 2, Color::black);
    spriteBlackHover = Primitive::CreateSpriteRectangle(vector1 / 2, Color::gray);
    spriteWhite = Primitive::CreateSpriteRectangle(vector1 - vector1 * 0.05f, Color::white);
    spriteWhiteHover = Primitive::CreateSpriteRectangle(vector1 - vector1 * 0.05f / 7, Color::lightgray);

    if((spriteVisualBackground = Primitive::CreateSpriteFrom(Resources::GetImageSource(Resources::LoadImage(GetDataDir() + "/layer.png", true)))) == nullptr)
    {
        Debug::Log("Visual sprite is no loaded");
    }

    spriteVisualNote = Sprite::CreateWhiteSprite();
    spriteCorner =  Primitive::CreateSpriteFrom(Resources::LoadImageRight(GetDataDir() + "/corner.png", true));

    __stack_records__.clear();
    clear();

    // Setting White notes
    for(x = 0; x < 7; ++x)
    {
        snprintf(buffer, sizeof(buffer), "C-%d", 6 - x);
        notes[x].name = buffer;
        std::memcpy(notes[x].keys, shortcutWhiteKeys[x], sizeof(notes[x].keys));
    }
    for(; x < 17; ++x)
    {
        snprintf(buffer, sizeof(buffer), "C+%d", x - 7);
        notes[x].name = buffer;
        std::memcpy(notes[x].keys, shortcutWhiteKeys[x], sizeof(notes[x].keys));
    }

    // Setting Black notes
    for(x = 0; x < NotesBlackNum; ++x)
    {
        snprintf(buffer, sizeof(buffer), "F+%d", x);
        notes[x + NotesWhiteNum].name = buffer;
        notes[x + NotesWhiteNum].keys[0] = shortcutBlackKeys[x];
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
        ResId waveNote;
        if(!(waveNote = notes_res[x]))
        {
            waveNote = notes_res[x] = Resources::LoadAudioClip(notesDir + notes[x].name + ext, false);
        }
        notes[x].source = this->AddComponent<AudioSource>();
        notes[x].source->setClip(Resources::GetAudioClipSource(waveNote));
        notes[x].source->setVolume(1.0f);
        notes[x].noteName = Input::GetKeyName(notes[x].keys[0]);
    }

    // write black tones
    for(x = 0; x < NotesBlackNum; ++x)
    {
        notes[x + NotesWhiteNum].render->setSprite(spriteBlack);
        notes[x + NotesWhiteNum].render->transform()->layer(LAYER_NOTES + 1);
        notes[x + NotesWhiteNum].normal = spriteBlack;
        notes[x + NotesWhiteNum].hover = spriteBlackHover;
    }

    // write white tones
    for(x = 0, y = NotesWhiteNum; x < NotesWhiteNum; ++x)
    {
        notes[x].render->setSprite(spriteWhite);
        notes[x].normal = spriteWhite;
        notes[x].hover = spriteWhiteHover;

        // set black notes position
        if(keyLayouts[x % (sizeof(keyLayouts) / sizeof(keyLayouts[0]))])
        {
            offset_left = notes[x].render->transform()->position();
            offset_left.x -= vector1.x / 2;
            offset_left.y += notes[y].render->getSize().y / 2;
            notes[y].render->transform()->position(offset_left);
            ++y;
        }
    }

    // Generate Visual Layer
    SpriteRendererRef visualObject = Primitive::CreateEmptyGameObject()->AddComponent<SpriteRenderer>();
    visualObject->setSprite(spriteVisualBackground);
    visualObject->setColor(Color::MakeTransparency(Color::white, .6f));
    visualObject->setSize({1, 2});
    vector1 = notes[0].render->transform()->position();
    vector1.x += spriteVisualBackground->size().x / 2 - notes[0].render->getSize().x / 2 + 0.25f;
    vector1.y += spriteVisualBackground->size().y + notes[0].render->getSize().y - 0.1f;

    visualObject->transform()->layer(LAYER_VISUALBACK);
    visualObject->transform()->position(vector1);

    hint = TitleApplication;

    // Generate Renderers
    for(x = 0; x < sizeof(notes) / sizeof(notes[0]); ++x)
    {
        SpriteRendererRef backNote;
        ParticleSystemRef p1, p2;

        p1 = Primitive::CreateEmptyGameObject()->AddComponent<ParticleSystem>();

        p1->emit = false;
        p1->loop = true;
        p1->speed = 3;
        p1->rotate = false;
        p1->destroyAfter = false;
        p1->interval = 0.01f;

        p1->setSource(spriteVisualNote);
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
        p2->setColors(Color::yellow, Color::yellow, {Color::red, 0u});
        p2->transform()->layer(LAYER_PARTICLES);

        p2->transform()->position({notes[x].render->transform()->position().x, notes[0].render->transform()->position().y + notes[0].render->getSize().y});

        _particles.emplace_back(p1, p2);

        backNote = Primitive::CreateEmptyGameObject(Vec2 {p1->transform()->position().x, visualObject->transform()->position().y})->AddComponent<SpriteRenderer>();
        backNote->transform()->layer(LAYER_BACKNOTE);
        backNote->setSize({0.9f, 3.7});
        backNote->setColor({Color::black, 0});

        backNote->setSprite(spriteBlack);
        _backDrawNotes.emplace_back(backNote);
    }
}

void PianoPlayer::OnUpdate()
{
    int standing = 0;

    Vec2 ms = Camera::ScreenToWorldPoint(Input::GetMousePointf());

    // flush
    __stack_records__.clear();

    mousetouched = -1;

    if(Input::GetKeyUp(KeyboardCode::KB_ESCAPE))
    {
        RoninSimulator::RequestQuit();
    }

    // Auto playing
    if(playing() && records.size() > 2)
    {
        if(currentDuration() > curPlayback)
        {
            // Are looping
            // TODO: Set custom select state are loop after play-end.
            if(track == records.size() - 1)
            {
                // Replay
                play();
            }
            curPlayback = records.at(track).first;

            // select the next tracks
            if(currentDuration() >= records[track+1].first)
                ++track;
        }
    }

    for(int note = AllNotes - 1, playRecNote = -1; note > -1; --note)
    {
        _particles[note].first->emit = false;
        _particles[note].second->emit = false;

        if((Input::GetMouseDown(MouseButton::MouseLeft) && mousetouched == -1))
        {
            SpriteRendererRef noteRenderer = notes[note].render;

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

        if((Input::GetKeyDown(notes[note].keys[0]) || Input::GetKeyDown(notes[note].keys[1])) || mousetouched == note || playRecNote == note)
        {
            if(notes[note].close == false)
            {
                notes[note].render->setSprite(notes[note].hover);
                notes[note].startPlayTime = playTimer();
                if(!paramMute)
                    notes[note].source->setVolume(1);
                notes[note].source->Play();
                notes[note].close = true;
            }
            __stack_records__.insert(note);

            // Draw particle
            _particles[note].first->emit = true;
            _particles[note].second->emit = true;
            _backDrawNotes[note]->setColor({Color::black, 127});

            standing|=1;
        }
        else if(Input::GetKeyUp(notes[note].keys[0]) || Input::GetKeyUp(notes[note].keys[1]) || mousetouched != note)
        {
            notes[note].close = false;
            notes[note].render->setSprite(notes[note].normal);
            _backDrawNotes[note]->setColor(Color::Lerp(_backDrawNotes[note]->getColor(), Color::transparent, Time::deltaTime() * 2));
        }
    }

    // Effect is tile remove
    if(paramEFXRemainder && !paramMute)
    {
        float diff;
        for(int note = 0; note < AllNotes; ++note)
        {
            diff = playTimer() - notes[note].startPlayTime;
            if(notes[note].startPlayTime > 0 && diff > 0.1f)
            {
                if(notes[note].source->getVolume() == 0)
                {
                    notes[note].source->Stop();
                    notes[note].startPlayTime = 0;
                }
                else
                {
                    diff = Math::Clamp01(1.0 - diff);
                    notes[note].source->setVolume(diff);
                }
            }
        }
    }

    // Can record
    if(recording())
    {
        int cmpt = 1;

        if(__stack_records__.size() == records.at(records.size() - 1).second.size())
        {
            /*std::vector*/
            if constexpr(std::is_same_v<decltype(records), std::vector<int>>)
            {
                //cmpt = memcmp(records.at(records.size() - 1).second.data(), __stack_records__.data(), __stack_records__.size());
            }
            else
            {
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
        }

        // No write last screen if then compare as 0
        if(cmpt != 0)
        {
            // Now Record!
            curPlayback = currentDuration();
            records.emplace_back(std::move(std::make_pair<float, std::set<int>>(std::move(curPlayback), std::move(__stack_records__))));
        }
    }

    if(standing == 0)
    {
        if(standbyTickOut == 0.0f)
        {
            standbyTickOut = Time::time();
        }
        else if(standbyTickOut != -1.0f)
        {
            if((Time::time() - standbyTickOut) >= standbyAfter)
            {
                // StandBy now
                onStandBy();
                standbyTickOut=-1.0f;
            }
        }
    }
    else
    {
        standbyTickOut = 0.0f;
    }
}

void PianoPlayer::OnGizmos()
{
    Vec2 vec1;

    vec1 = Camera::ScreenToWorldPoint(Vec2{150,30});

    if(playing())
    {
        hint = "Play";
    }
    else if(recording())
    {
        hint = "Record...";
    }

    if(paramLegacyFonts)
        RenderUtility::DrawTextLegacy(vec1, hint);
    else
        RenderUtility::DrawTextClassic(vec1, hint);

    if(Input::GetMouseDown(MouseButton::MouseRight))
    {
        RenderUtility::SetColor(Color {Color::black, 160});
        RenderUtility::DrawFillRect(Vec2::zero, 16, 10);
        RenderUtility::SetColor(Color::white);
        RenderUtility::DrawTextLegacy(Vec2::zero, "Reloading...");
        RoninSimulator::ReloadWorld();
        return;
    }

    if(mousetouched != -1)
    {
        RenderUtility::SetColor(Color::white);
        Rectf noteArea = {notes[mousetouched].render->transform()->position(), notes[mousetouched].render->getSprite()->size()};
        RenderUtility::DrawRectangle(noteArea.getXY(), noteArea.w, noteArea.h);
    }

    Vec2 p;
    for(int n = 0; n < AllNotes; ++n)
    {
        p = notes[n].render->transform()->position();
        p.x -= 0.05f;
        if(notes[n].close)
            p.y -= 0.25f;
        else
            p.y -= 0.2f;

        if(paramLegacyFonts)
            RenderUtility::DrawTextLegacy(p, notes[n].noteName, n < NotesWhiteNum ? true : notes[n].close);
        else
        {
            if(n < NotesWhiteNum || notes[n].close)
                RenderUtility::SetColor(Color::red);
            else
                RenderUtility::SetColor(Color::gray);

            RenderUtility::DrawTextClassic(p, notes[n].noteName);
        }
    }
}

void PianoPlayer::onStandBy()
{
    for( int x = 0; x < _particles.size(); ++x)
    {
        _particles[x].first->Reset();
        _particles[x].second->Reset();
    }

    hint = TitleApplication;
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
    clear();
    startPlayback = playTimer();
    records.emplace_back(std::move(std::make_pair(0.f, std::move(std::set<int> {}))));
    Debug::Log("Begin Record");
}

void PianoPlayer::endRecord()
{
    if(recording())
    {
        records.emplace_back(currentDuration(), std::move(std::set<int> {}));
        records.emplace_back(std::move(std::make_pair(-1.f, std::move(std::set<int> {}))));
        stop();
    }
    Debug::Log("End Record");
}

float PianoPlayer::duration()
{
    float t = static_cast<float>(length());

    if(t != -1.0f && t > 0)
    {
        t = records[records.size() - 2].first;
    }

    return t;
}

float PianoPlayer::currentDuration()
{
    return playTimer() - startPlayback;
}

void PianoPlayer::setPosition(float value)
{
    if(value < 0 || length() != -1)
        return;
    int i;
    for(i = 0; i < records.size(); ++i)
    {
        if(records[i].first >= value)
        {
            track = i;
            break;
        }
    }
}

void PianoPlayer::setMute(bool value)
{
    paramMute = value;

    for(int x = 0; x < AllNotes; ++x)
    {
        notes[x].source->setVolume(value ? 0.0F : 1.0F);
    }
}

int PianoPlayer::length()
{
    if(recording())
        return -1;

    return records.size()-1;
}

int PianoPlayer::peek()
{
    return track;
}

bool PianoPlayer::seek(int pos)
{
    if(recording() || pos < 0 || pos > records.size() - 1)
        return false;

    track = pos;

    return true;
}

void PianoPlayer::clear()
{
    stop();
    records.clear();
    Debug::Log("Clear");
}

bool PianoPlayer::play()
{
    if(!records.empty() && !recording())
    {
        startPlayback = playTimer();
        track = 0;         // start play
        curPlayback = 0.0f; // reset time
        return true;
    }

    return false;
}

void PianoPlayer::stop()
{
    track = -1;
    startPlayback = 0;
    curPlayback = 0.0f; // reset time
}
