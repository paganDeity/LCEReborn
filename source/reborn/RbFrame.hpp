#ifndef REBORN_RBFRAME_HPP
#define REBORN_RBFRAME_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>

namespace reborn
{
class Frame
{
public:
    static void Write(std::vector<uint8_t>& out, uint64_t id, const uint8_t* payload, size_t size);
};

// a blocking stream of bytes, such as a connection's socket
class ByteSource
{
public:
    virtual ~ByteSource() {}
    // false when the stream ends before size bytes
    virtual bool Read(uint8_t* data, size_t size) = 0;
};

// the next frame whose number accept takes; a frame it does not take is skipped by its length and counted in skipped.
// False at the end of the stream, or on a length outside 1 to maxFrame
bool ReadFrame(ByteSource& source, size_t maxFrame, bool (*accept)(uint64_t id, void* context), void* context, uint64_t* id,
               std::vector<uint8_t>* payload, size_t* skipped);
}

#endif
