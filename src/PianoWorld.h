#pragma once

#include <set>
#include <ronin/framework.h>
#include "IVStars.hpp"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;

extern Asset* globalAsset;

struct PianoNote
{
    bool close;

    std::string name;
    KeyboardCode keys[2];
    AudioSource *source;
    SpriteRenderer *render;
    SpriteRenderer *renderCorner;
    const char *noteName;
    float startPlayTime;

    Sprite *normal;
    Sprite *hover;
};

class PianoPlayer : public Behaviour
{
protected:
    int track;

    float startPlayback;
    float curPlayback;
    float standbyAfter;
    float standbyTickOut;
    const char * hint;
    std::vector<std::pair<float, std::set<int>>> records {};
    std::vector<std::pair<ParticleSystem *, ParticleSystem *>> _particles;
    std::vector<SpriteRenderer *> _backDrawNotes;

    void onStandBy();

public:
    bool paramEFXRemainder;
    bool paramLegacyFonts;
    bool paramMute;

    Sprite *spriteBlack;
    Sprite *spriteBlackHover;
    Sprite *spriteWhite;
    Sprite *spriteWhiteHover;
    Sprite *spriteVisualNote;
    Sprite *spriteVisualBackground;
    Sprite *spriteCorner;

    PianoPlayer();

    void OnAwake();
    void OnUpdate();
    void OnGizmos();

    bool recording() const;
    bool playing() const;
    void beginRecord();
    void endRecord();

    float duration();
    float currentDuration();
    void setPosition(float value);
    void setMute(bool value);

    int length();
    int peek();
    bool seek(int pos);
    bool play();
    void stop();
    void clear();

    // TODO: Make save/load records from/to filename
    void loadFromStream(const std::istream &in);
    void saveToStream(std::ostream &out);
};

class PianoWorld : public World
{
public:
    IVStars ivstars;

    UI::uid sliderPosTrack;
    UI::uid buttonPlay;
    UI::uid buttonRec;
    UI::uid buttonMute;
    UI::uid buttonEfx;

    Asset* resource;

    PianoPlayer *piano;
    PianoWorld();

    void OnStart();

    void OnUpdate();

    void OnUnloading();
};

// Global Function
std::string GetDataDir();
