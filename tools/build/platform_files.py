"""Which source files a platform compiles, by file name.

A file named name_os_arch.cpp is compiled only for that operating system and
architecture, name_os.cpp only for that operating system, name_arch.cpp only for
that architecture, and any other file everywhere. OPERATING_SYSTEMS and
ARCHITECTURES below are the names, including wiiu and ppc for the Wii U.
android also selects _linux files and ios also selects _darwin files.

A folder may not hold both double-suffixed files and single-suffixed ones: which
of them applies would be unclear. check() reports such folders.

    platform_files.py select --os linux --arch amd64 [--from LIST] [FILE...]
    platform_files.py check [--from LIST] [FILE...]

--from reads more file names from LIST, one per line, for lists too long for a
command line.
"""
import argparse
import sys
from pathlib import PurePosixPath

OPERATING_SYSTEMS = frozenset(
    (
        "aix", "android", "darwin", "dragonfly", "freebsd", "hurd", "illumos", "ios", "js", "linux", "nacl",
        "netbsd", "openbsd", "plan9", "solaris", "wasip1", "windows", "zos", "wiiu",
    )
)
ARCHITECTURES = frozenset(
    (
        "386", "amd64", "amd64p32", "arm", "armbe", "arm64", "arm64be", "loong64", "mips", "mipsle", "mips64",
        "mips64le", "mips64p32", "mips64p32le", "ppc", "ppc64", "ppc64le", "riscv", "riscv64", "s390", "s390x",
        "sparc", "sparc64", "wasm",
    )
)
IMPLIED_OS = {"android": "linux", "ios": "darwin"}


class PlatformFileError(Exception):
    """A file name or a folder breaks the platform suffix rules."""


def constraint(path):
    """The (os, arch) a file is limited to, None where it is not limited, and how many suffixes it uses."""
    tokens = PurePosixPath(path).stem.split("_")
    if len(tokens) >= 3 and tokens[-2] in OPERATING_SYSTEMS and tokens[-1] in ARCHITECTURES:
        return tokens[-2], tokens[-1], 2
    if len(tokens) >= 2 and tokens[-1] in OPERATING_SYSTEMS:
        return tokens[-1], None, 1
    if len(tokens) >= 2 and tokens[-1] in ARCHITECTURES:
        return None, tokens[-1], 1
    return None, None, 0


def matches_os(wanted, os_name):
    return wanted == os_name or IMPLIED_OS.get(os_name) == wanted


def selected(path, os_name, arch):
    if os_name not in OPERATING_SYSTEMS:
        raise PlatformFileError("unknown operating system %r" % os_name)
    if arch not in ARCHITECTURES:
        raise PlatformFileError("unknown architecture %r" % arch)
    file_os, file_arch, _ = constraint(path)
    return (file_os is None or matches_os(file_os, os_name)) and (file_arch is None or file_arch == arch)


def check(paths):
    """Every folder that mixes double-suffixed files with single-suffixed ones."""
    kinds = {}
    for path in paths:
        suffixes = constraint(path)[2]
        if suffixes:
            kinds.setdefault(str(PurePosixPath(path).parent), set()).add(suffixes)
    return sorted(folder for folder, found in kinds.items() if len(found) > 1)


def main(argv):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    commands = parser.add_subparsers(dest="command", required=True)
    select = commands.add_parser("select", help="print the files this platform compiles, one per line")
    select.add_argument("--os", dest="os_name", required=True)
    select.add_argument("--arch", required=True)
    select.add_argument("--from", dest="listing", help="a file listing more files, one per line")
    select.add_argument("files", nargs="*")
    verify = commands.add_parser("check", help="fail on folders that mix double and single suffixes")
    verify.add_argument("--from", dest="listing", help="a file listing more files, one per line")
    verify.add_argument("files", nargs="*")
    args = parser.parse_args(argv)
    if args.listing:
        with open(args.listing, encoding="utf-8") as listing:
            args.files += [line.strip() for line in listing if line.strip()]
    try:
        mixed = check(args.files)
        if mixed:
            raise PlatformFileError(
                "these folders mix _os_arch files with _os or _arch files: %s" % ", ".join(mixed)
            )
        if args.command == "select":
            for path in args.files:
                if selected(path, args.os_name, args.arch):
                    print(path)
    except PlatformFileError as error:
        print("error: %s" % error, file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
