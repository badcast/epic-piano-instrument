#include "PianoWorld.h"

void PianoWorld::OnStart()
{
    // Create camera2d
    Primitive::CreateCamera2D();
    piano = Primitive::CreateEmptyGameObject()->AddComponent<PianoPlayer>();
}
