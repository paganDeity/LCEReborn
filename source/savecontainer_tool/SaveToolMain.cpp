#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>

#include <zlib.h>

#include "savecontainer/ScContainer.hpp"
#include "savecontainer/ScExport.hpp"

namespace
{
bool ReadFile(const char* path, std::vector<uint8_t>& bytes)
{
    FILE* file = fopen(path, "rb");
    if (file == NULL)
        return false;
    uint8_t buffer[65536];
    size_t got;
    bytes.clear();
    while ((got = fread(buffer, 1, sizeof(buffer), file)) > 0)
        bytes.insert(bytes.end(), buffer, buffer + got);
    fclose(file);
    return true;
}

bool WriteFile(const char* path, const std::vector<uint8_t>& bytes)
{
    const std::string temporary = std::string(path) + ".part";
    FILE* file = fopen(temporary.c_str(), "wb");
    if (file == NULL)
        return false;
    const bool wrote = bytes.empty() || fwrite(&bytes[0], 1, bytes.size(), file) == bytes.size();
    if (fclose(file) != 0 || !wrote)
        return false;
    return rename(temporary.c_str(), path) == 0;
}

uint32_t Read32(const uint8_t* at)
{
    return static_cast<uint32_t>(at[0]) | (static_cast<uint32_t>(at[1]) << 8) | (static_cast<uint32_t>(at[2]) << 16)
           | (static_cast<uint32_t>(at[3]) << 24);
}

bool LegacyToBlob(const std::vector<uint8_t>& legacy, std::vector<uint8_t>& blob)
{
    if (legacy.size() < 8 || Read32(&legacy[0]) != 0)
        return false;
    uLongf size = Read32(&legacy[4]);
    if (size == 0 || size > (1u << 30))
        return false;
    blob.assign(size, 0);
    return uncompress(&blob[0], &size, &legacy[8], static_cast<uLong>(legacy.size() - 8)) == Z_OK && size == blob.size();
}

void BlobToLegacy(const std::vector<uint8_t>& blob, std::vector<uint8_t>& legacy)
{
    uLongf size = compressBound(static_cast<uLong>(blob.size()));
    legacy.assign(size + 8, 0);
    compress2(&legacy[8], &size, &blob[0], static_cast<uLong>(blob.size()), 6);
    const uint32_t original = static_cast<uint32_t>(blob.size());
    memcpy(&legacy[4], &original, 4);
    legacy.resize(size + 8);
}
}

int main(int argc, char** argv)
{
    if (argc != 4 || (strcmp(argv[1], "to-native") != 0 && strcmp(argv[1], "to-legacy") != 0))
    {
        fputs("Usage: LCERebornSaveTool to-native <legacy world.bin> <native world.bin>\n"
              "       LCERebornSaveTool to-legacy <native world.bin> <legacy world.bin>\n"
              "The legacy container is the zlib-wrapped one earlier builds write; it is little endian.\n",
              stderr);
        return 2;
    }
    std::vector<uint8_t> input, blob, output;
    if (!ReadFile(argv[2], input))
    {
        fprintf(stderr, "Can't read %s\n", argv[2]);
        return 1;
    }
    if (strcmp(argv[1], "to-native") == 0)
    {
        if (!LegacyToBlob(input, blob))
        {
            fputs("The input is not a legacy container\n", stderr);
            return 1;
        }
        savecontainer::Container::Encode(&blob[0], blob.size(), output);
        if (output.empty())
        {
            fputs("The input holds no readable container\n", stderr);
            return 1;
        }
    }
    else
    {
        if (!savecontainer::Container::Decode(&input[0], input.size(), blob, NULL))
        {
            fputs("The input is not a native container, or it is damaged\n", stderr);
            return 1;
        }
        std::vector<uint8_t> converted;
        if (!savecontainer::Export::ToLegacyChunks(&blob[0], blob.size(), converted))
        {
            fputs("A chunk could not be converted to the old compression\n", stderr);
            return 1;
        }
        BlobToLegacy(converted, output);
    }
    if (!WriteFile(argv[3], output))
    {
        fprintf(stderr, "Can't write %s\n", argv[3]);
        return 1;
    }
    printf("%s: %lu bytes in, %lu bytes out\n", argv[1], static_cast<unsigned long>(input.size()),
           static_cast<unsigned long>(output.size()));
    return 0;
}
