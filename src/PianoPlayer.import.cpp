#include <cstring>

#include "PianoWorld.h"

void PianoPlayer::importFromStream(std::istream &in)
{
    if(playing() || recording() || !in)
        return;
    PianoRecord rec;
    in.read(reinterpret_cast<char*>(&rec), sizeof(rec));
    if(rec.version != 1)
        return;
    if(strncmp(rec.header, "RECORD", sizeof(rec.header)))
        return;
    if(rec.lens == 0)
        return;
    decltype(records) newRecords;
    int val,i;
    float fv;
    for(i = 0; i < rec.lens; ++i)
    {

        in.read(reinterpret_cast<char*>(&fv), sizeof(fv));

    }

    records = newRecords;
}
