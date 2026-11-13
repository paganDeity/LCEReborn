#ifndef SAVECONTAINER_SCWRITER_HPP
#define SAVECONTAINER_SCWRITER_HPP

#include <stddef.h>
#include <stdint.h>

#include <map>
#include <vector>

namespace savecontainer
{
class Writer
{
public:
    Writer();

    void Reset();
    bool Prepare(const uint8_t* blob, size_t size, std::vector<uint8_t>& out, bool* replace, uint64_t onDiskSize);
    void Adopt(const uint8_t* blob, size_t size, uint64_t fileLength);
    uint64_t ExpectedFileSize() const { return m_fileSize; }
    bool Valid() const { return m_valid; }

private:
    typedef std::map<uint16_t, uint64_t> ChunkHashes;

    bool m_valid;
    uint64_t m_fileSize;
    uint64_t m_live;
    std::vector<uint8_t> m_table;
    std::map<uint64_t, uint64_t> m_raw;
    std::map<uint64_t, ChunkHashes> m_chunks;

    bool Build(const uint8_t* blob, size_t size, std::vector<uint8_t>* out, bool full, bool* changed);
};
}

#endif
