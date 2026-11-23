#!/usr/bin/env python3
"""Convert a BDF cursor font to an Xcursor theme (stdlib only).

usage: bdf2xcursor.py [options] FONT.bdf OUTDIR [nominal_size=32]

Writes OUTDIR/index.theme and OUTDIR/cursors/<name>.

A cursor font stores every cursor as two glyphs: a *shape* (which pixels get
the foreground colour) and a *mask* (which pixels are visible at all).  The
origin of the glyph is the hotspot.  This is exactly what XCreateGlyphCursor
consumes, and it is what the X core font ("cursor.bdf") and others, such as
DEC's "DECW$CURSOR", use.

Shape/mask pairing (--pairing):
  name      "foo" + "foo_mask" ("-mask", ".mask", "mask" suffixes also work)
  encoding  glyph N (even) + glyph N+1, as in the X core cursor font
  auto      name if any pair is found that way, otherwise encoding

Cursor names (--names):
  glyph       the shape glyph's STARTCHAR name (sanitised for use as filename)
  cursorfont  names from X11/cursorfont.h, looked up by the shape's encoding
              (right for the X core cursor font only)
  auto        cursorfont if the font is called "cursor", otherwise glyph

--map FILE renames cursors.  Each line is
    <glyph name or encoding>  <cursor name> [<more names, made symlinks>]
with '#' comments.  Cursors not in the map keep their automatic name.
"""
import argparse, os, re, struct, sys

XCURSOR_IMAGE = 0xFFFD0002

# CSS/freedesktop names used by GTK/Qt -> core names; only created when the
# target exists in the generated theme (not exhaustive)
ALIASES = {"default": "left_ptr", "text": "xterm", "pointer": "hand2",
           "wait": "watch", "progress": "watch", "help": "question_arrow",
           "move": "fleur", "grabbing": "fleur", "all-scroll": "fleur",
           "ew-resize": "sb_h_double_arrow", "col-resize": "sb_h_double_arrow",
           "ns-resize": "sb_v_double_arrow", "row-resize": "sb_v_double_arrow",
           "not-allowed": "circle", "cell": "plus"}

MASK_RE = re.compile(r"^(.+?)[_\-. ]?mask$", re.I)


class Glyph:
    def __init__(self, name, enc, bbx, rows):
        self.name, self.enc = name, enc
        self.w, self.h, self.xo, self.yo = bbx
        self.bits = []  # bits[row][col], row 0 = top
        for r in range(max(self.h, 0)):
            row = rows[r] if r < len(rows) else ""
            nb, val = len(row) * 4, int(row, 16) if row else 0
            self.bits.append([(val >> (nb - 1 - c)) & 1 if c < nb else 0
                              for c in range(max(self.w, 0))])

    @property
    def empty(self):
        return self.w <= 0 or self.h <= 0

    def box(self):  # x0, y0, x1, y1 in glyph coords (y up, origin = hotspot)
        return self.xo, self.yo, self.xo + self.w, self.yo + self.h

    def get(self, x, y):
        c, r = x - self.xo, self.yo + self.h - 1 - y
        return 0 <= c < self.w and 0 <= r < self.h and bool(self.bits[r][c])


def parse_bdf(path):
    font, glyphs, cur, in_bitmap = "", [], None, False
    with open(path, encoding="latin-1") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            if in_bitmap:
                if line == "ENDCHAR":
                    glyphs.append(Glyph(cur["name"], cur["enc"], cur["bbx"], cur["rows"]))
                    cur, in_bitmap = None, False
                else:
                    cur["rows"].append(line)
                continue
            key, _, rest = line.partition(" ")
            rest = rest.strip()
            if key == "FONT" and not font:
                font = rest
            elif key == "STARTCHAR":
                cur = {"name": rest, "enc": -1, "bbx": (0, 0, 0, 0), "rows": []}
            elif cur is not None:
                if key == "ENCODING":
                    cur["enc"] = int(rest.split()[0])
                elif key == "BBX":
                    cur["bbx"] = tuple(map(int, rest.split()[:4]))
                elif key == "BITMAP":
                    in_bitmap = True
    return font, glyphs


def pairs_by_name(glyphs):
    by = {g.name: g for g in glyphs}
    out = []
    for g in glyphs:
        m = MASK_RE.match(g.name)
        if m and m.group(1) in by and by[m.group(1)] is not g:
            out.append((by[m.group(1)], g))
    return out


def pairs_by_encoding(glyphs):
    by = {g.enc: g for g in glyphs if g.enc >= 0}
    return [(by[e], by[e + 1]) for e in sorted(by) if e % 2 == 0 and e + 1 in by]


def load_cursorfont(path):
    try:
        txt = open(path).read()
    except OSError:
        return {}
    return {int(n): k for k, n in re.findall(r"#define\s+XC_(\w+)\s+(\d+)", txt)
            if k != "num_glyphs"}


def load_map(path):
    m = {}
    for line in open(path):
        parts = line.split("#", 1)[0].split()
        if len(parts) >= 2:
            m[parts[0]] = parts[1:]
    return m


def sanitize(name):
    return re.sub(r"[^\w.+-]", "_", name, flags=re.A).lstrip(".") or "cursor"


def render(src, msk, fg, bg):
    """Return (w, h, xhot, yhot, pixels) in Xcursor ARGB, rows top to bottom.
    X semantics: a pixel is visible only where the mask is set; it gets the
    foreground colour where the shape is set, background colour elsewhere."""
    boxes = [g.box() for g in (src, msk) if not g.empty]
    if boxes:
        x0 = min(b[0] for b in boxes); y0 = min(b[1] for b in boxes)
        x1 = max(b[2] for b in boxes); y1 = max(b[3] for b in boxes)
    else:
        x0, y0, x1, y1 = 0, -1, 1, 0  # empty cursor -> 1x1 transparent
    w, h = x1 - x0, y1 - y0
    pix = [(fg if src.get(x, y) else bg) if msk.get(x, y) else 0
           for y in range(y1 - 1, y0 - 1, -1) for x in range(x0, x1)]
    xhot = min(max(-x0, 0), w - 1)  # hotspot = glyph origin
    yhot = min(max(y1, 0), h - 1)
    return w, h, xhot, yhot, pix


def xcursor(size, w, h, xhot, yhot, pix):
    img = struct.pack("<9I", 36, XCURSOR_IMAGE, size, 1, w, h, xhot, yhot, 0) \
        + struct.pack(f"<{w * h}I", *pix)
    return struct.pack("<4s3I", b"Xcur", 16, 0x10000, 1) \
        + struct.pack("<3I", XCURSOR_IMAGE, size, 28) + img


def put(path, data=None, link=None):
    if os.path.lexists(path):
        os.remove(path)
    if link is not None:
        os.symlink(link, path)
    else:
        with open(path, "wb") as f:
            f.write(data)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("bdf")
    ap.add_argument("outdir")
    ap.add_argument("size", nargs="?", type=int, default=32,
                    help="nominal size recorded in the files (default 32)")
    ap.add_argument("--pairing", choices=("auto", "name", "encoding"), default="auto")
    ap.add_argument("--names", choices=("auto", "glyph", "cursorfont"), default="auto")
    ap.add_argument("--cursorfont", default="/usr/include/X11/cursorfont.h")
    ap.add_argument("--map", help="file renaming cursors, see --help text in the source")
    ap.add_argument("--fg", default="000000", help="foreground RRGGBB (default 000000)")
    ap.add_argument("--bg", default="ffffff", help="background RRGGBB (default ffffff)")
    ap.add_argument("--theme-name", help="Name= in index.theme (default: BDF file name)")
    ap.add_argument("--no-aliases", action="store_true",
                    help="do not add CSS/freedesktop alias symlinks")
    a = ap.parse_args()

    fg = 0xFF000000 | int(a.fg, 16)
    bg = 0xFF000000 | int(a.bg, 16)
    font, glyphs = parse_bdf(a.bdf)

    pairs = []
    if a.pairing in ("auto", "name"):
        pairs = pairs_by_name(glyphs)
    if not pairs and a.pairing in ("auto", "encoding"):
        pairs = pairs_by_encoding(glyphs)
    if not pairs:
        sys.exit("no shape/mask glyph pairs found in " + a.bdf)
    paired = {id(g) for p in pairs for g in p}
    for g in glyphs:
        if id(g) not in paired:
            print(f"warning: glyph {g.name!r} (encoding {g.enc}) has no shape/mask partner, skipped",
                  file=sys.stderr)

    use_cf = a.names == "cursorfont" or (a.names == "auto" and font.lower() == "cursor")
    cf = load_cursorfont(a.cursorfont) if use_cf else {}
    cmap = load_map(a.map) if a.map else {}

    d = os.path.join(a.outdir, "cursors")
    os.makedirs(d, exist_ok=True)
    used = set()
    for src, msk in pairs:
        names = cmap.get(src.name) or cmap.get(str(src.enc))
        if not names:
            names = [cf.get(src.enc) or src.name or f"glyph_{src.enc}"]
        names = [sanitize(n) for n in names]
        if names[0] in used:
            names[0] = f"{names[0]}_{src.enc}"
        data = xcursor(a.size, *render(src, msk, fg, bg))
        put(os.path.join(d, names[0]), data)
        used.add(names[0])
        for extra in names[1:]:
            if extra in used:
                print(f"warning: name {extra!r} already used, alias skipped", file=sys.stderr)
                continue
            put(os.path.join(d, extra), link=names[0])
            used.add(extra)

    if not a.no_aliases:
        for alias, target in ALIASES.items():
            if alias not in used and target in used:
                put(os.path.join(d, alias), link=target)

    theme = a.theme_name or os.path.splitext(os.path.basename(a.bdf))[0]
    with open(os.path.join(a.outdir, "index.theme"), "w") as f:
        f.write("[Icon Theme]\nName=" + theme + "\n")
    print(f"{len(pairs)} cursors written to {d}", file=sys.stderr)


if __name__ == "__main__":
    main()