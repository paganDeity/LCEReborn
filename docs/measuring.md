# Measuring

How close is the rebuilt game to the original? `tools/measure/parity.py`
answers that function by function, from two files you have: the original
`Minecraft.Client.rpx` in your update 688 dump, and the one you built.

```
python tools/build.py
python tools/measure/parity.py --original <update v688 dump>/code/Minecraft.Client.rpx
```

It reads `build/Minecraft.Client.rpx`, its link map
`build/Minecraft.Client.map` and the `source/` tree by default (`--rebuilt`,
`--map` and `--source` change them), runs in about a minute, and
`--report FILE` writes one row per function with the reasons for each
verdict. It needs Python 3.8 or newer and a C++ demangler: `llvm-cxxfilt`
or `c++filt` if your machine has one, otherwise the `c++filt` inside the
Docker image `tools/build.py` builds.

## What it compares

Both executables keep their function names, relocations and machine code.
The tool pairs every function of the original with the function of the same
name in the rebuilt one and compares the facts a different compiler leaves
alone:

- the functions it calls;
- how many calls it makes through a pointer (virtual calls, callbacks);
- the floating-point constants it loads, and what each one is used for (an
  operand of a multiplication, an addition or subtraction, a division, a
  comparison, a store, a call argument); a constant next to the one the
  original loads counts as misread;
- the 32-bit constants it builds;
- the string literals it uses;
- the constant integer arguments of the calls both functions make;
- whether its body is far thinner than the original's.

A function is **clean** when all of these agree, and **differs** otherwise;
the report says why. Constructors, destructors and thunks are counted
separately, since the C++ ABI shapes them as much as the source does, and
the virtual function tables the constructors install are compared slot by
slot.

The rebuilt RPX is compiled by RedHills, the original by the compiler the
game shipped with, and the two inline different functions. The tool allows
for that: a call the original makes that the rebuilt function lacks still
counts when the callee could have been inlined there and the rebuilt body
holds what the callee does, and the other way round. A function of the
original with no copy in the rebuilt executable is looked up in `source/`:
when the source defines it, RedHills inlined it wherever it is used, and it
is reported as **inlined**; otherwise it is **missing**. Functions with
nothing to compare (a small accessor, for one) are reported as
**trivial**. Neither inlined nor trivial functions count in the
percentages. Code each compiler brings itself, the C++ standard library,
boost, rapidxml and the C runtime, is not compared.

## What the number means

- **It is a check of calls, constants and strings, not of behaviour.**
  Field offsets, the order of operations and branch conditions are not
  compared, so a clean function can still behave differently from the
  original.
- **It understates.** The two compilers sometimes produce different facts
  from the same source (one folds a constant the other loads, one keeps a
  call the other inlines in a way the tool cannot follow), so some correct
  functions are reported as differing.
- **It measures the build you run.** The numbers below come from the
  public build of the commit named with them; your own build of that commit
  gives the same result.

## World generation

Function parity does not check the terrain a seed produces. The generator is
written from the original's compiled code like the rest of `source/`, and
what has been measured here is that it is reproducible: two worlds made from
seed 4242, world size classic, in separate dedicated server runs, show the
same terrain from the same viewpoint. At 1280x720, 877 of 921,600 pixels
differed and every one of them was in the sky.

The two runs picked spawn points 3 blocks apart (206.5, 247.5 and 209.5,
249.5). Whether the original derives the spawn offset from the seed has not
been checked.

A comparison with the retail game's own output for the same seed has not
been made here. It needs the retail game running with a way to read its
world save. Until that exists, the claim about generation is the method's:
the code was read from the original, it is not a measured match.

## Current result

Measured on 18 October 2026 with the public build of the commit that last
updated these numbers, against update 688's `Minecraft.Client.rpx`.

Of the original's 40,032 functions outside the standard library and the
runtimes, 23,851 were compared; 6,378 were inlined by RedHills and 9,803
had nothing to compare.

| Result | Functions | Share of compared |
| --- | ---: | ---: |
| Clean | 18,463 | 77.4% |
| Differs | 4,832 | 20.3% |
| Missing | 556 | 2.3% |

| Area | Clean | Differs | Clean share |
| --- | ---: | ---: | ---: |
| Game | 14,713 | 3,470 | 80.9% |
| Wii U platform layer | 551 | 287 | 65.8% |
| Mii faces (FFL) | 228 | 81 | 73.8% |
| NEX online library | 970 | 407 | 70.4% |
| Pia online library | 1,598 | 454 | 77.9% |
| Other Nintendo SDK | 43 | 5 | 89.6% |
| Sound and middleware | 340 | 115 | 74.7% |
| Cafe SDK runtime | 19 | 10 | 65.5% |
| Constructors, destructors and thunks | 4,138 | 709 | 84.6% |

The 556 missing functions are ones the tool could not find a definition of
in `source/`; its search reads source text, so some of them are defined in a
form it does not recognise.

Of the 2,464 virtual function tables the original's constructors install,
2,441 are identical in the rebuilt executable, and 102,993 of their 103,024
slots match. Of the 23 that differ, 15 differ in one way: the original's
compiler gives an override with a covariant return type
(`CompoundTag::copy`, for one) a second slot of its own, and RedHills does
not yet. 7 are tables the tool pairs with the wrong rebuilt table (the
`IdMapper` base table of a `MappedRegistry` against the derived
`SimpleRegistry` one), and one has trailing slots the rebuilt table lacks.
29 tables of abstract base classes are never installed by the rebuilt
executable, whose compiler drops a store the derived constructor
overwrites, and are not compared.

## Saves

`tools/measure/saves.py <world.bin>` unwraps the save (a zero word, the
container size, then a zlib stream), lists the container's files, and for each
region file reports the chunk count, the stored (compressed) and raw sizes the
container records for each chunk, and the sector slack. It does not decompress
the chunks, so it says nothing about the split between blocks, light, biomes and
NBT inside a chunk.

Two worlds made by this build (about 1,400 chunks each in the overworld):

| | world A | world B |
| --- | --- | --- |
| file on disk | 4.92 MB | 4.97 MB |
| container after unwrapping | 7.07 MB | 7.87 MB |
| chunks stored (nether, end counted) | 1388 | 1524 |
| stored per chunk (LZX + RLE) | 3.5 KB | 3.4 KB |
| raw per chunk before compression | 13.3 KB | 12.5 KB |
| compression ratio | 3.8 | 3.7 |
| 4 KB sector slack | 31% | 41% |

- A dense chunk would be tens of kilobytes (16x16x128 blocks, data, two light
  nibble planes); the format already stores sections sparsely (`ElementStorage`,
  `SparseLightStorage`) and compresses each chunk, so the first gain is not there.
- The outer zlib pass over the whole container saves about nothing: the chunks
  are already compressed, and zlib takes 4.92 MB to 4.92 MB on 7.07 MB. Compressing
  it costs about 110 to 125 ms (Python's zlib, level 1 and 6) on every save.
- The 4 KB sector slack is real in memory and in the container (31 to 41%), but
  it compresses away in the outer pass, so it costs time and memory, not disk.
### In the game

`harness.py save` saves the hosted world and `harness.py chunkstats` reads the
chunk path's timers (debug builds, `ChunkStats`). World A (the save above)
loaded headless, then saved once, on one machine, llvmpipe, three save threads:

| Stage | Chunks | Total | Per chunk | Bytes |
| --- | --- | --- | --- | --- |
| load: read from the container | 606 | 2.8 ms | 4.5 us | 3.5 KB stored |
| load: LZX + RLE decompress | 606 | 29 ms | 48 us | 3.5 KB to 10.9 KB |
| load: deserialise into a chunk | 606 | 44 ms | 72 us | |
| save: serialise to a buffer | 509 | 9 ms | 18 us | |
| save: LZX + RLE compress (save threads) | 509 | 192 ms | 377 us | 10.7 KB to 3.8 KB |
| save: container zlib | 1 | 143 ms | | 7.19 MB to 4.94 MB |
| save: container flush in all | 1 | 144 ms | | |

- A load costs about 125 us a chunk, split between decompressing and building
  the chunk, so the read itself is not the problem.
- A save of 509 chunks compresses every one of them, although the world was only
  loaded and nothing was changed. Compression is 20 times the cost of
  serialising, and it runs off the main thread. The container zlib pass is
  almost all of the flush (143 of 144 ms) and runs once per save on the whole
  world, so it grows with the world and not with what changed.
- The zlib pass shrinks 7.19 MB to 4.94 MB, but the chunks it holds are already
  4.8 MB: it removes the sector slack, and nothing else.

A second save a few seconds later, with nothing done in the world, wrote 202 of
the 509 chunks again (the original rule rewrites a chunk that holds entities when
the game time moved on) and spent the same 143 ms in the container zlib: the
chunk work follows what changed, the container pass does not.

Not measured: what a save copies on its way to disk beyond these stages,
resident bytes per loaded chunk, and the split of a chunk's bytes between
blocks, light, biomes and NBT.

### After the native container

The same world, loaded from the old container and saved once: the container pass
(`containerEncode`) takes 38 ms for 7.19 MB in and 4.89 MB out, against 143 ms of
zlib for 4.94 MB; the written file is 4.89 MB. The same 606 chunks load again from
it. The chunk compression (171 ms for 508 chunks) is now the largest part of a
save.

### With zstd chunks

Same world, loaded from the old container and saved once: the 515 chunks compressed
in 32 ms in the save threads (171 ms before), at 3.84 KB a chunk; loading 606 chunks
took 9 ms to decompress (26 ms before). Save container work after nothing changed:
about 5 ms.
