#include <cstring>

#include "PianoWorld.h"

void PianoPlayer::importFromStream(std::istream &in)
{
    PianoRecord rec;
    std::uint32_t num1,num2;
    int i,num3;
    float fv;
    std::set<int> _sets;
    decltype(records) newRecords;

    if(playing() || recording() || !in)
        return;
    in.read(reinterpret_cast<char*>(&rec), sizeof(rec));
    if(rec.version != 1)
        return;
    if(strncmp(rec.header, "RECORD", sizeof(rec.header)))
        return;
    if(rec.lens == 0)
        return;
    for(i = 0; i < rec.lens; ++i)
    {
        // read checksum
        in.read(reinterpret_cast<char*>(&num1), sizeof(num1));
        in.read(reinterpret_cast<char*>(&fv), sizeof(float));
        in.read(reinterpret_cast<char*>(&num2), sizeof(num2));
        for(;num2-->0;)
        {
            in.read(reinterpret_cast<char*>(&num3), sizeof(num3));
            _sets.insert(num3);
        }
        newRecords.emplace_back(fv,std::move(_sets));
    }
    if(newRecords.back().first != -1.0F)
        newRecords.emplace_back(-1.0F, std::set<int>{});
    records = std::move(newRecords);
}
