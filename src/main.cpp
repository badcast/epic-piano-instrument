#include <exception>
#include <iostream>

#include "PianoWorld.h"

using namespace std;
#if WIN32
int WinMain(void *, void *, void *, int)
#else
int main()
#endif
{
    RoninSimulator::Init();

    PianoWorld world;

    Resolution resolution {1024, 768};
    RoninSimulator::Show(resolution, false);

    if(RoninSimulator::LoadWorld(&world) == true)
    {
        RoninSimulator::Simulate();
    }

     RoninSimulator::Finalize();

    return 0;
}
