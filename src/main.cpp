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
    RoninSimulator::init();

    Resolution resolution{1024, 512};
    RoninSimulator::show(resolution, false);

    PianoWorld world;
    RoninSimulator::set_debug_mode(true);
    RoninSimulator::load_world(&world);
    RoninSimulator::simulate();

    RoninSimulator::utilize();

    return 0;
}
