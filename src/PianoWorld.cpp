#include "PianoWorld.h"

PianoWorld::PianoWorld() : World("World Piano")
{
}

Ref<PianoPlayer> playerInstance;

////////////////////////
//////// EVENTS ////////
////////////////////////
/*
void OnVolumeChange(UI::uid, float newValue)
{
    for(int x = 0; x < AllNotes; ++x)
    {
        notes[x].source->setVolume(newValue);
    }
}
*/

void OnPositionChange(UI::uid, float newValue)
{
    PianoWorld *pworld = static_cast<PianoWorld *>(World::GetCurrentWorld());
    pworld->piano->seek(Math::Number(newValue * pworld->piano->length()));
}

void OnButtonPlay(UI::uid id)
{
    PianoWorld * world = static_cast<PianoWorld *>(World::GetCurrentWorld());

    // Update icon states (Button icon change)

    /*
        player_stop
        player_record
        player_play
    */

    const char * newState;

    if(id == world->buttonPlay && world->piano->length() > 0 && !world->piano->recording())
    {
        if(world->piano->playing())
        {
            world->piano->stop();
            newState = "player_play";
        }
        else
        {
            world->piano->play();
            newState = "player_stop";
        }

        world->GetGUI()->SpriteButtonSetIcon(id, globalAsset->GetAtlasObject()->GetSpriteFromName(newState));
    }

    if(id == world->buttonRec)
    {
        if(!world->piano->recording())
        {
            world->piano->beginRecord();
            world->GetGUI()->SpriteButtonSetIcon(world->buttonPlay, globalAsset->GetAtlasObject()->GetSpriteFromName("player_play"));
            newState = "player_stop";
        }
        else
        {
            world->piano->endRecord();
            newState = "player_record";
        }

        world->GetGUI()->SpriteButtonSetIcon(id, globalAsset->GetAtlasObject()->GetSpriteFromName(newState));
    }

    if(id == world->buttonMute)
    {
        world->piano->setMute(!world->piano->paramMute);

        if(world->piano->paramMute)
            newState = "player_mute";
        else
            newState = "player_unmute";

        world->GetGUI()->SpriteButtonSetIcon(id, globalAsset->GetAtlasObject()->GetSpriteFromName(newState));
    }

    if(id == world->buttonEfx)
    {
        world->piano->paramEFXRemainder = !world->piano->paramEFXRemainder;

        if(world->piano->paramEFXRemainder)
            newState = "player_efx";
        else
            newState = "player_unmute";

        world->GetGUI()->SpriteButtonSetIcon(id, globalAsset->GetAtlasObject()->GetSpriteFromName(newState));
    }
}

void PianoWorld::OnStart()
{
    // Create camera2d
    Primitive::CreateCamera2D();

    piano = Primitive::CreateEmptyGameObject()->AddComponent<PianoPlayer>();

    ivstars.set(Vec2::up_right, .2, 100, true);

    /// SET GUI
    playerInstance = piano;

    std::vector<SpriteRef> _buttonStates
    {
        globalAsset->GetAtlasObject()->GetSpriteFromName("button.layer.normal"),
        globalAsset->GetAtlasObject()->GetSpriteFromName("button.layer.hover"),
        globalAsset->GetAtlasObject()->GetSpriteFromName("button.layer.disable")
    };

    buttonRec = GetGUI()->PushSpriteButton(_buttonStates, {10, 10, 50, 50}, OnButtonPlay);
    buttonPlay = GetGUI()->PushSpriteButton(_buttonStates, {70, 10, 50, 50}, OnButtonPlay);
    buttonMute = GetGUI()->PushSpriteButton(_buttonStates, {10, 65, 50, 50}, OnButtonPlay);
    buttonEfx = GetGUI()->PushSpriteButton(_buttonStates, {10, 120, 50, 50}, OnButtonPlay);
//    sliderPosTrack = GetGUI()->PushSlider(1.0f, 0.0f, 1.0f, {500, 64, 240, 10}, OnPositionChange);

    GetGUI()->SpriteButtonSetIcon(buttonRec, globalAsset->GetAtlasObject()->GetSpriteFromName("player_record"));
    GetGUI()->SpriteButtonSetIcon(buttonPlay, globalAsset->GetAtlasObject()->GetSpriteFromName("player_play"));
    GetGUI()->SpriteButtonSetIcon(buttonMute, globalAsset->GetAtlasObject()->GetSpriteFromName("player_unmute"));
    GetGUI()->SpriteButtonSetIcon(buttonEfx, globalAsset->GetAtlasObject()->GetSpriteFromName("player_efx"));
}

void PianoWorld::OnUpdate()
{
    // GetGUI()->ElementSetVisible(sliderPosTrack, (piano->length() > 0));
    // if(GetGUI()->ElementGetVisible(sliderPosTrack) && piano->playing())
    // {
    //     float dur = piano->duration();
    //     float curDur = piano->currentDuration();

    //     dur = curDur / dur;

    //     GetGUI()->SliderSetValue(sliderPosTrack, dur);
    // }

    ivstars.pull();
}

void PianoWorld::OnUnloading()
{
    ivstars.clear();
}
