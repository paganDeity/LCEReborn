#!/usr/bin/env python3
"""Print the WebAssembly module that tests/mod_test.cpp embeds as kWasm.

It imports lce.log, lce.reply and lce.command, registers the command "ping" in
lce_init and answers it with "pong" in lce_command. Paste the output over the
kWasm array when the module changes.
"""


def section(number, body):
    return bytes([number, len(body)]) + body


def name(text):
    return bytes([len(text)]) + text.encode()


types = bytes([4, 0x60, 2, 0x7F, 0x7F, 0, 0x60, 1, 0x7F, 1, 0x7F, 0x60, 0, 0, 0x60, 2, 0x7F, 0x7F, 1, 0x7F])
imports = bytes([3]) + name("lce") + name("log") + b"\0\0" + name("lce") + name("reply") + b"\0\0" + name("lce") + name("command") + b"\0\0"
functions = bytes([3, 1, 2, 3])
memory = bytes([1, 0, 1])
exports = bytes([4]) + name("memory") + b"\x02\0" + name("lce_alloc") + b"\0\x03" + name("lce_init") + b"\0\x04" + name("lce_command") + b"\0\x05"
alloc = bytes([0, 0x41, 0x80, 0x10, 0x0B])
init = bytes([0, 0x41, 0x10, 0x41, 0x0A, 0x10, 0, 0x41, 0x20, 0x41, 0x04, 0x10, 2, 0x0B])
command = bytes([0, 0x41, 0x30, 0x41, 0x04, 0x10, 1, 0x41, 0, 0x0B])
code = bytes([3]) + b"".join(bytes([len(f)]) + f for f in (alloc, init, command))
data = bytes([3, 0, 0x41, 0x10, 0x0B, 0x0A]) + b"hello wasm" + bytes([0, 0x41, 0x20, 0x0B, 4]) + b"ping" + bytes([0, 0x41, 0x30, 0x0B, 4]) + b"pong"
module = b"\0asm\1\0\0\0" + section(1, types) + section(2, imports) + section(3, functions) + section(5, memory) + section(7, exports) + section(10, code) + section(11, data)

print("const uint8_t kWasm[] = {")
print(",\n".join("    " + ", ".join("0x%02x" % b for b in module[i:i + 12]) for i in range(0, len(module), 12)))
print("};")
