Zstandard 1.5.7, https://facebook.github.io/zstd/ (BSD-3-Clause or GPL-2.0; used under BSD-3-Clause, see LICENSE).
Unmodified lib/zstd.h, lib/zstd_errors.h and the .c and .h files of lib/common, lib/compress and lib/decompress from
https://github.com/facebook/zstd/releases/download/v1.5.7/zstd-1.5.7.tar.gz
(SHA-256 eb33e51f49a15e023950cd7825ca74a4a2b43db8354825ac24fc1b7ee09e6fa3). The x86-64 assembly Huffman decoder
(huf_decompress_amd64.S) is left out and the build defines ZSTD_DISABLE_ASM, so every platform compiles the same C.
Used by source/savecontainer to compress the chunk records of a save.
