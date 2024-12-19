#include <iostream>
#include <exception>

#include "PianoWorld.h"

AssetRef globalAsset = nullptr;

#if WIN32
int WinMain(void *, void *, void *, int)
#else
int main(int argc, char **argv)
#endif
{
    std::string pathname = *argv;
    std::string::size_type _lastpos = pathname.find_last_of(Paths::GetPathSeperatorOS());
    if(std::string::npos != _lastpos)
        pathname = pathname.substr(_lastpos+1);
    std::cout << pathname << " version " << PIANO_BUILD_VERSION << std::endl;

    RoninSimulator::Init();
    Resolution resolution {1024, 600};
    RoninSimulator::Show(resolution, false);

    RoninSimulator::SetDebugMode(false);

    RoninSettings settings = RoninSettings::GetCurrent();
    settings.verticalSync = true;
    settings.textureQuality = RoninSettings::RenderTextureScaleQuality::Nearest;
    settings.Apply();


    globalAsset = AssetManager::LoadAsset(GetDataDir() + Paths::GetPathSeperatorOS() + "controls.asset");
    globalAsset = AssetManager::LoadAsset(GetDataDir() + Paths::GetPathSeperatorOS() + "controls.asset");
    if(!globalAsset)
    {
        std::cerr << "Fail load asset file" << std::endl;
    }
    else
    {
        if(RoninSimulator::LoadWorld<PianoWorld>() != nullptr)
        {
            RoninSimulator::Simulate();
        }
        AssetManager::UnloadAsset(globalAsset);
        globalAsset = nullptr;
    }
    RoninSimulator::Finalize();
    return 0;
}
