#!/usr/bin/env python3
"""Convert the X11 core cursor font (cursor.bdf) to an Xcursor theme.
usage: bdf2xcursor.py cursor.bdf OUTDIR [nominal_size=32]
Writes OUTDIR/index.theme and OUTDIR/cursors/<name>. Stdlib only."""
import os, re, struct, sys

bdf, out = sys.argv[1], sys.argv[2]
size = int(sys.argv[3]) if len(sys.argv) > 3 else 32

# glyph encoding -> (w, h, xoff, yoff, bits[row][col])
glyphs = {}
for blk in open(bdf).read().split("STARTCHAR")[1:]:
    enc = int(re.search(r"ENCODING (\d+)", blk)[1])
    w, h, xo, yo = map(int, re.search(r"BBX (-?\d+) (-?\d+) (-?\d+) (-?\d+)", blk).groups())
    rows = re.search(r"BITMAP\n(.*?)ENDCHAR", blk, re.S)[1].split()
    glyphs[enc] = (w, h, xo, yo,
                   [[(int(r, 16) >> (len(r) * 4 - 1 - x)) & 1 for x in range(w)] for r in rows])

def bit(g, x, y):  # glyph coords (y up, origin = hotspot)
    w, h, xo, yo, bits = g
    c, r = x - xo, yo + h - 1 - y
    return 0 <= c < w and 0 <= r < h and bits[r][c]

def xcursor(n):  # glyph n = shape, n+1 = mask
    src, msk = glyphs[n], glyphs[n + 1]
    x0 = min(src[2], msk[2]); x1 = max(src[2] + src[0], msk[2] + msk[0])
    y0 = min(src[3], msk[3]); y1 = max(src[3] + src[1], msk[3] + msk[1])
    w, h = x1 - x0, y1 - y0
    pix = [0xFF000000 if bit(src, x, y) else 0xFFFFFFFF if bit(msk, x, y) else 0
           for y in range(y1 - 1, y0 - 1, -1) for x in range(x0, x1)]
    img = struct.pack("<9I", 36, 0xFFFD0002, size, 1, w, h, -x0, y1, 0) \
        + struct.pack(f"<{w * h}I", *pix)
    return struct.pack("<4s3I", b"Xcur", 16, 0x10000, 1) \
        + struct.pack("<3I", 0xFFFD0002, size, 28) + img

d = os.path.join(out, "cursors")
os.makedirs(d, exist_ok=True)
names = re.findall(r"#define XC_(\w+)\s+(\d+)", open("/usr/include/X11/cursorfont.h").read())
for name, n in names:
    if name != "num_glyphs":
        open(os.path.join(d, name), "wb").write(xcursor(int(n)))

# CSS/freedesktop names used by GTK/Qt -> core names (not exhaustive)
for a, t in dict(default="left_ptr", text="xterm", pointer="hand2", wait="watch",
                 progress="watch", help="question_arrow", move="fleur", grabbing="fleur",
                 **{"all-scroll": "fleur", "ew-resize": "sb_h_double_arrow",
                    "col-resize": "sb_h_double_arrow", "ns-resize": "sb_v_double_arrow",
                    "row-resize": "sb_v_double_arrow", "not-allowed": "circle",
                    "cell": "plus"}).items():
    if not os.path.lexists(os.path.join(d, a)):
        os.symlink(t, os.path.join(d, a))

open(os.path.join(out, "index.theme"), "w").write("[Icon Theme]\nName=" + os.path.splitext(os.path.basename(bdf))[0] + "\n")
