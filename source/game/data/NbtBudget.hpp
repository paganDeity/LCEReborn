#ifndef GAME_DATA_NBTBUDGET_HPP
#define GAME_DATA_NBTBUDGET_HPP

// A limit on what reading one NBT value may allocate, for a value that comes from another machine. A list's count and an array's length are
// numbers in the data, read before the elements, so a few bytes can ask for gigabytes. While a budget is open on the reading thread every
// loader charges what it is about to allocate, and stops when the budget is spent. With no budget open (a save, which is trusted) nothing is
// charged. PC only: the Wii U readers are as the original has them.
namespace NbtBudget
{
// what a packet's NBT may allocate in all, the quota Java Edition gives a packet
const long long kPacketBytes = 2097152;

void Open(long long bytes);
// false, and the budget is marked spent, when it cannot cover this; always true with no budget open. A negative count is refused.
bool Charge(long long bytes);
// closes the budget; true when it was spent
bool Close();
}

#endif
