#ifndef SAVECONTAINER_SCBLOB_HPP
#define SAVECONTAINER_SCBLOB_HPP

#include <stddef.h>
#include <stdint.h>

#include <vector>

namespace savecontainer
{
class BlobFile
{
public:
    BlobFile() : timestamp(0) {}

    std::vector<uint16_t> name;
    int64_t timestamp;
    std::vector<uint8_t> data;
};

class BlobViewFile
{
public:
    BlobViewFile() : timestamp(0), data(NULL), size(0) {}

    std::vector<uint16_t> name;
    int64_t timestamp;
    const uint8_t* data;
    size_t size;
};

class BlobView
{
public:
    BlobView() : originalVersion(0), saveVersion(0) {}

    static bool Parse(const uint8_t* bytes, size_t size, BlobView& view);

    int16_t originalVersion;
    int16_t saveVersion;
    std::vector<BlobViewFile> files;
};

class Blob
{
public:
    static const size_t kHeaderBytes = 12;
    static const size_t kNameUnits = 64;
    static const size_t kEntryBytes = 144;

    Blob() : originalVersion(0), saveVersion(0) {}

    static bool Parse(const uint8_t* bytes, size_t size, Blob& blob);
    void Build(std::vector<uint8_t>& bytes) const;

    int16_t originalVersion;
    int16_t saveVersion;
    std::vector<BlobFile> files;
};
}

#endif
