#include "PianoWorld.h"

PianoWorld::PianoWorld() : World("World Piano")
{
}

void inputPos(UI::uid, float newValue)
{
    PianoWorld *pworld = static_cast<PianoWorld *>(World::GetCurrentWorld());
    pworld->piano->seek(Math::Number(newValue * pworld->piano->length()));
}

void PianoWorld::OnStart()
{
    // Create camera2d
    Primitive::CreateCamera2D();

    piano = Primitive::CreateEmptyGameObject()->AddComponent<PianoPlayer>();

    ivstars.set(Vec2::up_right, .2, 100, true);

    /// SET GUI
    sliderPosTrack = GetGUI()->PushSlider(1.0f, 0.0f, 1.0f, {500, 64, 240, 10}, inputPos);
}

void PianoWorld::OnUpdate()
{
    GetGUI()->ElementSetVisible(sliderPosTrack, (piano->length() > 0));
    if(GetGUI()->ElementGetVisible(sliderPosTrack) && piano->playing())
    {
        float dur = piano->duration();
        float curDur = piano->currentDuration();

        dur = curDur / dur;

        GetGUI()->SliderSetValue(sliderPosTrack, dur);
    }

    ivstars.pull();
}

void PianoWorld::OnUnloading()
{
    ivstars.clear();
}
