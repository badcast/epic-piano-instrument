#ifndef _OTHER_OBJECTS_HPP_
#define _OTHER_OBJECTS_HPP_

#include <ronin/framework.h>

using namespace RoninEngine;
using namespace RoninEngine::Runtime;

class IVStars
{
private:
    float _speed;
    Vec2 _dir;
    std::list<SpriteRenderer *> _stars;

public:
    IVStars();

    void set(Vec2 direction, float speed, int count, bool startOfScreen);
    void clear();
    void play();
};

#endif
