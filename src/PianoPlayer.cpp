#include "PianoWorld.h"

void PianoPlayer::OnAwake()
{
    // init piano sources
    int x, y;
    char buffer[64];
    const char ext[] = ".wav";

    for(x = 0, y = 5; x < NotesNum; ++x, ++y)
    {
        snprintf(buffer, sizeof(buffer), "piano_C%d", y);
        notes[x].name = buffer;
        snprintf(buffer, sizeof(buffer), "piano_C%d-1", y);
        notes[++x].name = buffer;
    }

    // load clips
    for(x = 0; x < NotesNum; ++x)
    {
        // Add sprite
        notes[x].render = Primitive::create_empty_game_object()->AddComponent<SpriteRenderer>();
        //   notes[x].render->transform()->set_parent(this->transform());

        // Add AudioSource and load
        notes[x].source = this->AddComponent<AudioSource>();
        resource_id note_wav = Resources::LoadAudioClip(std::string("./data/sounds/") + notes[x].name + ext, true);
        notes[x].source->clip(Resources::GetAudioClipSource(note_wav));
        notes[x].source->volume(0.3f);
    }

    // load sprites
    std::string sprite_directory {"./data/sprites/"};
    spr_black = Primitive::create_empty_sprite2D();
    spr_lf = Primitive::create_empty_sprite2D();
    spr_cf = Primitive::create_empty_sprite2D();
    spr_rf = Primitive::create_empty_sprite2D();

    spr_black->set_surface(Resources::GetImageSource(Resources::LoadImage(sprite_directory + "black.png", true)));
    spr_lf->set_surface(Resources::GetImageSource(Resources::LoadImage(sprite_directory + "lf.png", true)));
    spr_cf->set_surface(Resources::GetImageSource(Resources::LoadImage(sprite_directory + "cf.png", true)));
    spr_rf->set_surface(Resources::GetImageSource(Resources::LoadImage(sprite_directory + "rf.png", true)));

    // init key tone and layout
#define set_layout(note) note.render->transform()->position(pos);
    notes[x = 0].key = KB_Q;
    notes[x].render->set_sprite(spr_lf);
    Vec2 scale = spr_cf->size();
    Vec2 pos = transform()->position();

    pos.x = -scale.x * 3;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_W;
    notes[x].render->set_sprite(spr_cf);
    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_E;
    notes[x].render->set_sprite(spr_rf);
    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_R;
    notes[x].render->set_sprite(spr_lf);
    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_T;
    notes[x].render->set_sprite(spr_cf);
    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_Y;
    notes[x].render->set_sprite(spr_rf);
    pos.x += scale.x;
    set_layout(notes[x]);

    // init key half-tone
    notes[x = 1].key = KB_2;
    notes[x].render->set_sprite(spr_black);
    scale = spr_black->size();
    pos.y = spr_cf->size().y / 2 - scale.y / 2;
    pos.x = notes[0].render->transform()->position().x + 0.015;

    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_3;
    notes[x].render->set_sprite(spr_black);
    pos.x += scale.x * 2;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_5;
    notes[x].render->set_sprite(spr_black);
    pos.x += scale.x * 4;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_6;
    notes[x].render->set_sprite(spr_black);
    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_7;
    notes[x].render->set_sprite(spr_black);
    pos.x += scale.x;
    set_layout(notes[x]);

    x += 2;
    notes[x].key = KB_9;
    notes[x].render->set_sprite(spr_black);
    pos.x += scale.x;
    set_layout(notes[x]);
#undef set_layout
}

void PianoPlayer::OnUpdate()
{
    int mouseNoteSelect = 0;
    for(int note = 0; note < NotesNum; ++note)
    {
        if((Input::GetMouseDown(MouseState::MouseLeft) && mouseNoteSelect == 0) && notes[note].render->get_sprite())
        {
            Vec2 ms = Camera::ScreenToWorldPoint(Input::GetMousePointf());
            Vec2 notePos = Vec2::Abs(notes[note].render->transform()->position() - ms);
            Rectf noteCoord = {Vec2::zero, Vec2(notes[note].render->get_sprite()->rect().getWH()) / 100};

            if(Vec2::InArea(notePos, noteCoord))
            {
                GameObject *minors = Primitive::create_box2d(notes[note].render->transform()->position(), 45, Color::red);
                minors->transform()->layer = 10;
                destroy(minors, 1);
                mouseNoteSelect = 1;
            }
        }

        if(Input::GetKeyDown(notes[note].key) || mouseNoteSelect == 1)
        {
            if(notes[note].state == false)
            {
                notes[note].state = true;
                notes[note].source->play();
                mouseNoteSelect = 2;
            }
        }
        else
        {
            notes[note].state = false;
        }
    }
}
