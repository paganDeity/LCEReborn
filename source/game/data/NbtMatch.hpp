#ifndef GAME_DATA_NBTMATCH_HPP
#define GAME_DATA_NBTMATCH_HPP

class Tag;

namespace NbtMatch
{
bool matches(Tag* pattern, Tag* actual);
}

#endif
