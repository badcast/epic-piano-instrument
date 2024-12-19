#pragma once

#include <set>
#include <iostream>
#include <ronin/framework.h>
#include "IVStars.hpp"
#include "KeyData.h"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;

extern AssetRef globalAsset;

struct PianoNote
{
    bool close;

    std::string name;
    KeyboardCode keys[2];
    AudioSourceRef source;
    SpriteRendererRef render;
    SpriteRendererRef renderCorner;
    std::string noteName;
    float startPlayTime;

    SpriteRef normal;
    SpriteRef hover;
};

struct PianoRecord
{
    int version;
    char header[7];
    std::uint32_t lens;
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
    std::vector<std::pair<ParticleSystemRef, ParticleSystemRef>> _particles;
    std::vector<SpriteRendererRef> _backDrawNotes;

    ResId notes_res[AllNotes];
    PianoNote notes[AllNotes];

    void onStandBy();

public:
    bool paramEFXRemainder;
    bool paramLegacyFonts;
    bool paramMute;

    SpriteRef spriteBlack;
    SpriteRef spriteBlackHover;
    SpriteRef spriteWhite;
    SpriteRef spriteWhiteHover;
    SpriteRef spriteVisualNote;
    SpriteRef spriteVisualBackground;
    SpriteRef spriteCorner;

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

    void importFromStream(std::istream &in);
    void exportToStream(std::ostream &out);
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

    Ref<PianoPlayer> piano;
    PianoWorld();

    void OnStart();

    void OnUpdate();

    void OnUnloading();
};

// Global Function
std::string GetDataDir();
std::uint32_t ComputeCheksums(float time, int sums);
