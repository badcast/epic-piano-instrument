#include <ronin/framework.h>

// #include <SoundTouch.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;

struct PianoNote
{
    bool state;

    std::string name;
    KeyboardCode key;
    AudioSource *source;
    SpriteRenderer *render;

    Sprite *normal;
    Sprite *hover;
};

class PianoPlayer : public Behaviour
{
public:
    Sprite *spr_black;
    Sprite *spr_black_hover;
    Sprite *spr_white;
    Sprite *spr_white_hover;

    PianoPlayer() : Behaviour("Piano Player")
    {
    }

    void OnAwake();
    void OnUpdate();
    void OnGizmos();
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
