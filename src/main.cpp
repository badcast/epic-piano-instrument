#include <iostream>
#include <exception>

#include "PianoWorld.h"

using namespace std;

AssetRef globalAsset = nullptr;

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
    settings.textureQuality = RoninSettings::RenderTextureScaleQuality::Nearest;
    settings.Apply();

    globalAsset =  AssetManager::LoadAsset(GetDataDir() + "/controls.asset");
    if(!globalAsset)
    {
        std::cerr << "Fail load asset file" << std::endl;
    }
    else
    {
        if(RoninSimulator::LoadWorldAfterSplash<PianoWorld>() != nullptr)
        {
            RoninSimulator::Simulate();
        }
        AssetManager::UnloadAsset(globalAsset);
    }
    RoninSimulator::Finalize();
    return 0;
}
