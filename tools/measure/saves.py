#!/usr/bin/env python3
"""Reports what a save holds: the files in the blob, and for each region file
how many chunks it stores, their compressed and decompressed sizes and the
sector slack. It reads the sizes the container records; it does not decompress.

    saves.py <world.bin> [--chunks]
"""
import argparse
import struct
import sys
import zlib

SECTOR = 4096
RLE_FLAG = 0x80000000
COMPRESSED_FLAG = 0x40000000
LENGTH_MASK = 0x3FFFFFFF
ENTRY = 144


def read_blob(path):
    data = open(path, "rb").read()
    wrapped = len(data)
    if data[:4] == b"\0\0\0\0" and data[8:9] == b"\x78":
        data = zlib.decompress(data[8:])
    print("file %d bytes on disk, container %d bytes" % (wrapped, len(data)))
    big = False
    index_offset, count = struct.unpack_from("<II", data, 0)
    if index_offset + count * ENTRY > len(data):
        big = True
        index_offset, count = struct.unpack_from(">II", data, 0)
    end = ">" if big else "<"
    versions = struct.unpack_from(end + "hh", data, 8)
    files = []
    for index in range(count):
        base = index_offset + index * ENTRY
        name = data[base:base + 128].decode("utf-16-be" if big else "utf-16-le").split("\0")[0]
        size, start = struct.unpack_from(end + "II", data, base + 128)
        files.append((name, size, start))
    return data, files, end, versions, big


def region(data, start, size, end):
    chunks = []
    table = struct.unpack_from(end + "1024I", data, start)
    for slot, entry in enumerate(table):
        if entry == 0:
            continue
        sector, count = entry >> 8, entry & 0xFF
        where = start + sector * SECTOR
        length, raw = struct.unpack_from(end + "II", data, where)
        chunks.append({"slot": slot, "sectors": count, "stored": length & LENGTH_MASK, "raw": raw,
                       "rle": bool(length & RLE_FLAG), "compressed": bool(length & COMPRESSED_FLAG)})
    return chunks


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("blob")
    parser.add_argument("--chunks", action="store_true")
    args = parser.parse_args()
    data, files, end, versions, big = read_blob(args.blob)
    print("blob %d bytes, %d files, versions %s, %s endian" % (len(data), len(files), versions, "big" if big else "little"))
    total = {"chunks": 0, "stored": 0, "raw": 0, "sectors": 0}
    other = 0
    for name, size, start in files:
        if not name.endswith(".mcr") or size == 0:
            other += size
            continue
        chunks = region(data, start, size, end)
        stored = sum(c["stored"] for c in chunks)
        raw = sum(c["raw"] for c in chunks)
        sectors = sum(c["sectors"] for c in chunks)
        print("%-28s file %9d  chunks %4d  stored %9d  raw %9d  sectors %9d  slack %8d" % (
            name, size, len(chunks), stored, raw, sectors * SECTOR, sectors * SECTOR - stored))
        if args.chunks:
            for c in chunks:
                print("    slot %4d  stored %7d  raw %7d  sectors %2d  rle %d" % (c["slot"], c["stored"], c["raw"], c["sectors"], c["rle"]))
        total["chunks"] += len(chunks)
        total["stored"] += stored
        total["raw"] += raw
        total["sectors"] += sectors * SECTOR
    if total["chunks"]:
        print("regions: %d chunks, stored %d (%.0f per chunk), raw %d (%.0f per chunk), ratio %.2f, sector slack %.1f%%; "
              "other files %d bytes" % (total["chunks"], total["stored"], total["stored"] / total["chunks"], total["raw"],
                                         total["raw"] / total["chunks"], total["raw"] / max(1, total["stored"]),
                                         100.0 * (total["sectors"] - total["stored"]) / max(1, total["sectors"]), other))


main()
