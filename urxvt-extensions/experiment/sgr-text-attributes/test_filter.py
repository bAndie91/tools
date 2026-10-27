#!/usr/bin/env python3
import importlib.util
import importlib.machinery
import sys
from pathlib import Path

path = Path(__file__).with_name("urxvt-sgr-proxy")
loader = importlib.machinery.SourceFileLoader("urxvt_sgr_proxy", str(path))
spec = importlib.util.spec_from_loader(loader.name, loader)
mod = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = mod
loader.exec_module(mod)


def run(data: bytes, chunks=None) -> bytes:
    f = mod.SGRFilter()
    if chunks is None:
        return f.feed(data, final=True)
    out = b""
    pos = 0
    for n in chunks:
        out += f.feed(data[pos:pos+n])
        pos += n
    out += f.feed(data[pos:], final=True)
    return out

# Style starts, reset/closing codes, and color updates while styled.
assert b"faint" in run(b"\x1b[2mfaint\x1b[22mnormal")
out = run(b"\x1b[8mconcealed\x1b[28mvisible")
assert b"concealed" in out and b"\x1b[39mvisible" in out
out = run(b"\x1b[9mstruck\x1b[29mnormal")
assert b"\x1b[4mstruck" in out and b"\x1b[24mnormal" in out
out = run(b"\x1b[2;31mred faint\x1b[39mstill faint\x1b[22mplain")
assert b"38;5;" in out and b"plain" in out
out = run(b"\x1b[8;7mconcealed reverse\x1b[27;28mvisible")
assert b"concealed reverse" in out and b"visible" in out
# Sequences that merely look like SGR inside OSC/DCS control strings are opaque.
opaque = b"\x1b]0;title contains \x1b[2m text\x07X"
assert run(opaque) == opaque
dcs = b"\x1bP1;2|opaque \x1b[9m data\x1b\\X"
assert run(dcs) == dcs
# CSI escape sequences split over arbitrary read chunks must still be handled.
data = b"a\x1b[2mb\x1b[22mc"
whole = run(data)
assert run(data, [2, 2, 1, 1, 3]) == whole
print("SGR filter tests passed")
