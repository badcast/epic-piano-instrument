#include <algorithm>
#include <ronin/Debug.h>

#include "PianoWorld.h"

constexpr auto Header = "RECORD";
constexpr std::uint32_t RecVersion = 1;

void PianoPlayer::exportToStream(std::ostream &out)
{
    if(length() < 1 || !out)
        return;
    PianoRecord rec;
    rec.version = RecVersion;
    std::copy(Header, sizeof(Header) + Header + 1, rec.header);
    rec.lens = static_cast<std::uint32_t>(length());
    out.write(reinterpret_cast<char*>(&rec), sizeof(rec));
    stop();
    for(std::uint32_t x = 0; x < rec.lens; ++x)
    {
        // write checksums
        std::uint32_t value = -1;
        out.write(reinterpret_cast<char*>(&value), sizeof(value));
        // Write delay
        out.write(reinterpret_cast<char*>(&records[x].first), sizeof(float));
        value = records[x].second.size();
        out.write(reinterpret_cast<char*>(&value), sizeof(value));
        for(int point : records[x].second)
        {
            out.write(reinterpret_cast<char*>(&point), sizeof(point));
        }
    }
    Debug::Log("Export complete");
}
