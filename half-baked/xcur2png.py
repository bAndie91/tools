#!/usr/bin/env python3
"""Dump every image of every Xcursor file to PNG: OUTDIR/<name>-<size>[.<frame>].png
usage: xcur2png.py CURSORS_DIR OUTDIR     (symlinked aliases are skipped)"""
import os, struct, sys
from PIL import Image

src, out = sys.argv[1], sys.argv[2]
os.makedirs(out, exist_ok=True)
for name in sorted(os.listdir(src)):
    p = os.path.join(src, name)
    if os.path.islink(p) or not os.path.isfile(p):
        continue
    print(p)
    d = open(p, "rb").read()
    if d[:4] != b"Xcur":
        print('not Xcur:', p)
        continue
    seen = {}
    for i in range(struct.unpack_from("<I", d, 12)[0]):
        typ, size, pos = struct.unpack_from("<3I", d, 16 + 12 * i)
        if typ != 0xFFFD0002:  # images only (not comments)
            continue
        w, h = struct.unpack_from("<2I", d, pos + 16)
        # ARGB32 little-endian, premultiplied -> bytes B,G,R,A
        im = Image.frombytes("RGBA", (w, h), d[pos + 36:pos + 36 + 4 * w * h], "raw", "BGRa")
        n = seen[size] = seen.get(size, -1) + 1  # animated cursors: several frames per size
        im.save(os.path.join(out, f"{name}-{size:03d}" + (f".{n}" if n else "") + ".png"))
