#include "PianoWorld.h"

void PianoWorld::OnStart()
{
    // Create camera2d
    Primitive::create_camera2D();
    piano = Primitive::create_empty_game_object()->AddComponent<PianoPlayer>();
}
