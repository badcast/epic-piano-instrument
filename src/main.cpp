#include <exception>
#include <iostream>

#include "PianoWorld.h"

using namespace std;
#if WIN32
int WinMain(void*, void*, void*, int)
#else
int main()
#endif
{
    RoninSimulator::Init();

    Resolution resolution{1024, 512};
    RoninSimulator::Show(resolution, false);

    PianoWorld world;
    RoninSimulator::SetDebugMode(true);
    RoninSimulator::LoadWorld(&world);
    RoninSimulator::Simulate();

    RoninSimulator::Finalize();

    return 0;
}
