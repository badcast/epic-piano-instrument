#include <set>
#include <ronin/framework.h>

// #include <SoundTouch.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;

struct PianoNote
{
    bool close;

    std::string name;
    KeyboardCode keys[2];
    AudioSource *source;
    SpriteRenderer *render;

    Sprite *normal;
    Sprite *hover;
};

class PianoPlayer : public Behaviour
{
protected:
    std::vector<std::pair<float, std::set<int>>> records {};
    int track;
    float startPlayback;
    float curPlayback;
    std::vector<std::pair<ParticleSystem *, ParticleSystem *>> _particles;
    std::vector<SpriteRenderer*> _backDrawNotes;

public:
    Sprite *spr_black;
    Sprite *spr_black_hover;
    Sprite *spr_white;
    Sprite *spr_white_hover;

    Sprite *visual_note;

    Sprite *visual_background;

    PianoPlayer() : Behaviour("Piano Player")
    {
    }

    void OnAwake();
    void OnUpdate();
    void OnGizmos();

    bool recording() const;
    bool playing() const;
    void beginRecord();
    void endRecord();
    bool playRecord();
    void stopPlay();
    void clearRecord();
};

class PianoWorld : public World
{

public:
    PianoPlayer *piano;
    PianoWorld() : World("World Piano")
    {
    }

    void OnStart();
};
