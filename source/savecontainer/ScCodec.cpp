#include "savecontainer/ScCodec.hpp"

#include <zlib.h>
#include <zstd.h>

#if defined(_MSC_VER)
#define SC_THREAD_LOCAL __declspec(thread)
#else
#define SC_THREAD_LOCAL __thread
#endif

namespace savecontainer
{
namespace
{
SC_THREAD_LOCAL ZSTD_CCtx* sCompress = NULL;
SC_THREAD_LOCAL ZSTD_DCtx* sDecompress = NULL;
}

bool Codec::ZstdCompress(const uint8_t* source, size_t size, uint8_t* destination, size_t capacity, size_t* written)
{
    if (sCompress == NULL)
        sCompress = ZSTD_createCCtx();
    if (sCompress == NULL)
        return false;
    const size_t result = ZSTD_compressCCtx(sCompress, destination, capacity, source, size, kZstdLevel);
    if (ZSTD_isError(result))
        return false;
    *written = result;
    return true;
}

bool Codec::ZstdDecompress(const uint8_t* source, size_t size, uint8_t* destination, size_t expected)
{
    if (sDecompress == NULL)
        sDecompress = ZSTD_createDCtx();
    if (sDecompress == NULL)
        return false;
    const size_t result = ZSTD_decompressDCtx(sDecompress, destination, expected, source, size);
    return !ZSTD_isError(result) && result == expected;
}

bool Codec::RleCompress(const uint8_t* source, size_t size, std::vector<uint8_t>& out)
{
    out.clear();
    out.reserve(size / 2 + 16);
    size_t at = 0;
    while (at < size)
    {
        const uint8_t value = source[at++];
        unsigned int run = 1;
        while (at < size && source[at] == value && run < 256)
        {
            ++at;
            ++run;
        }
        if (run <= 3)
        {
            if (value == 0xFF)
            {
                out.push_back(0xFF);
                out.push_back(static_cast<uint8_t>(run - 1));
            }
            else
                for (unsigned int index = 0; index < run; ++index)
                    out.push_back(value);
        }
        else
        {
            out.push_back(0xFF);
            out.push_back(static_cast<uint8_t>(run - 1));
            out.push_back(value);
        }
    }
    return true;
}

bool Codec::RleDecompress(const uint8_t* source, size_t size, std::vector<uint8_t>& out)
{
    out.clear();
    size_t at = 0;
    while (at < size)
    {
        const uint8_t value = source[at++];
        if (value != 0xFF)
        {
            out.push_back(value);
            continue;
        }
        if (at >= size)
            return false;
        unsigned int run = source[at++];
        uint8_t runValue = 0xFF;
        if (run >= 3)
        {
            if (at >= size)
                return false;
            runValue = source[at++];
        }
        ++run;
        out.insert(out.end(), run, runValue);
    }
    return true;
}

bool Codec::LegacyCompress(const uint8_t* source, size_t size, std::vector<uint8_t>& out, bool* rle)
{
    std::vector<uint8_t> flat;
    RleCompress(source, size, flat);
    *rle = true;
    const uint8_t* input = flat.empty() ? NULL : &flat[0];
    size_t inputSize = flat.size();
    if (flat.size() >= size)
    {
        *rle = false;
        input = source;
        inputSize = size;
    }
    uLongf bound = compressBound(static_cast<uLong>(inputSize));
    out.assign(bound, 0);
    if (compress(&out[0], &bound, input, static_cast<uLong>(inputSize)) != Z_OK)
        return false;
    out.resize(bound);
    return true;
}

bool Codec::LegacyDecompress(const uint8_t* source, size_t size, bool rle, size_t expected, std::vector<uint8_t>& out)
{
    std::vector<uint8_t> flat(expected + 16);
    uLongf produced = static_cast<uLongf>(flat.size());
    if (uncompress(&flat[0], &produced, source, static_cast<uLong>(size)) != Z_OK)
        return false;
    flat.resize(produced);
    if (!rle)
    {
        out = flat;
        return out.size() == expected;
    }
    return RleDecompress(flat.empty() ? NULL : &flat[0], flat.size(), out) && out.size() == expected;
}
}
