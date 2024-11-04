#include <iostream>
#include <exception>

#include "PianoWorld.h"

using namespace std;

Asset * globalAsset;

#if WIN32
int WinMain(void *, void *, void *, int)
#else
int main()
#endif
{
    RoninSimulator::Init();

    Resolution resolution {1024, 600};
    RoninSimulator::Show(resolution, false);

    RoninSimulator::SetDebugMode(false);

    RoninSettings settings = RoninSettings::GetCurrent();
    settings.verticalSync = true;
    settings.Apply();

    if(AssetManager::LoadAsset(GetDataDir() + "/controls.asset", &globalAsset) == false)
    {
        std::cerr << "Fail load asset file" << std::endl;
    }
    else
    {
        if(RoninSimulator::LoadWorld<PianoWorld>() != nullptr)
        {
            RoninSimulator::Simulate();
        }

        AssetManager::DeleteAsset(globalAsset);
    }

    RoninSimulator::Finalize();

    return 0;
}
