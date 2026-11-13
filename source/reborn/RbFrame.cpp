#include "reborn/RbFrame.hpp"

#include "reborn/RbVarint.hpp"

namespace reborn
{
void Frame::Write(std::vector<uint8_t>& out, uint64_t id, const uint8_t* payload, size_t size)
{
    Varint::Write(out, Varint::Size(id) + size);
    Varint::Write(out, id);
    if (size != 0)
        out.insert(out.end(), payload, payload + size);
}

bool ReadFrame(ByteSource& source, size_t maxFrame, bool (*accept)(uint64_t id, void* context), void* context, uint64_t* id,
               std::vector<uint8_t>* payload, size_t* skipped)
{
    for (;;)
    {
        uint8_t header[Varint::kMaxBytes];
        size_t used = 0;
        do
        {
            if (used == Varint::kMaxBytes || !source.Read(&header[used], 1))
                return false;
        } while ((header[used++] & 0x80) != 0);
        size_t at = 0;
        uint64_t length = 0;
        if (!Varint::Read(header, used, &at, &length) || length == 0 || length > maxFrame)
            return false;
        payload->resize(static_cast<size_t>(length));
        if (!source.Read(&(*payload)[0], payload->size()))
            return false;
        size_t start = 0;
        if (!Varint::Read(&(*payload)[0], payload->size(), &start, id))
            return false;
        if (accept == NULL || accept(*id, context))
        {
            payload->erase(payload->begin(), payload->begin() + start);
            return true;
        }
        if (skipped != NULL)
            ++*skipped;
    }
}
}
