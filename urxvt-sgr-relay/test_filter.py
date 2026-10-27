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
    out = bytearray()
    pos = 0
    for n in chunks:
        out.extend(f.feed(data[pos:pos+n]))
        pos += n
    out.extend(f.feed(data[pos:], final=True))
    return bytes(out)


# Default-background conceal must not use SGR 39 (default foreground, often white).
filter_ = mod.SGRFilter(mod.Palette())
out = filter_.feed(b"\x1b[8mhidden", final=True)
assert b"\x1b[38;5;0m" in out, repr(out)
assert b"\x1b[39m" not in out, repr(out)

# If configured defaults differ, true-color output must resolve the *background*
# default, not accidentally use the default foreground when concealing.
palette = mod.Palette()
palette.default_fg = (255, 255, 255)
palette.default_bg = (12, 34, 56)
filter_ = mod.SGRFilter(palette)
import os
old_truecolor = os.environ.get("URXVT_SGR_TRUECOLOR")
os.environ["URXVT_SGR_TRUECOLOR"] = "1"
try:
    out = filter_.feed(b"\x1b[8mhidden\x1b[28mvisible", final=True)
finally:
    if old_truecolor is None:
        os.environ.pop("URXVT_SGR_TRUECOLOR", None)
    else:
        os.environ["URXVT_SGR_TRUECOLOR"] = old_truecolor
assert b"\x1b[38;2;12;34;56m" in out and b"\x1b[39mvisible" in out, repr(out)

# Style starts, reset/closing codes, and color updates while styled.
assert b"faint" in run(b"\x1b[2mfaint\x1b[22mnormal")
out = run(b"\x1b[8mconcealed\x1b[28mvisible")
assert b"concealed" in out and b"\x1b[39mvisible" in out
out = run(b"\x1b[9mstruck\x1b[29mnormal")
assert "s\u0336t\u0336r\u0336u\u0336c\u0336k".encode() in out, repr(out)
assert b"normal" in out and b"\x1b[4m" not in out, repr(out)
out = run(b"\x1b[9mA\x1b[0mB")
assert "A\u0336".encode() in out and out.endswith(b"B"), repr(out)
out = run(b"\x1b[2;31mred faint\x1b[39mstill faint\x1b[22mplain")
assert b"38;5;" in out and b"plain" in out
out = run(b"\x1b[8;7mconcealed reverse\x1b[27;28mvisible")
assert b"concealed reverse" in out and b"visible" in out

# Strike through is UTF-8 safe across read boundaries, and controls stay intact.
utf8 = "\x1b[9mAé界e\u0301\nB\x1b[29mC".encode()
whole = run(utf8)
assert "A\u0336é\u0336界\u0336e\u0336\u0301\nB\u0336C".encode() == whole, repr(whole)
assert run(utf8, [3, 1, 2, 1, 4, 2, 1]) == whole
# Invalid UTF-8 is preserved byte-for-byte instead of crashing or disappearing.
invalid = b"\x1b[9m\xffx\x1b[29m"
assert b"\xffx" + "\u0336".encode() in run(invalid)

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
