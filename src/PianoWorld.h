#include <ronin/framework.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;

struct PianoNote
{
    std::string name;
    bool state;
    KeyboardCode key;
    AudioSource *source;
    SpriteRenderer *render;
    Sprite *normal;
    Sprite *hover;
};

class PianoPlayer : public Behaviour
{
public:
    enum
    {
        NotesNum = 12
    };
    Sprite *spr_black;
    Sprite *spr_lf;
    Sprite *spr_cf;
    Sprite *spr_rf;

    PianoNote notes[NotesNum];

    PianoPlayer() : Behaviour("Piano Player")
    {
    }

    void OnAwake();
    void OnUpdate();
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
