#include "PianoWorld.h"
#include <cstring>

void PianoWorld::OnStart()
{
    // Create camera2d
    Primitive::create_camera2D()->visibleObjects = true;

    piano = Primitive::create_empty_game_object()->AddComponent<PianoPlayer>();
}
