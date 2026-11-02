#!/usr/bin/env python3
"""
Generic (non-airline) 32x32 logo tiles, compiled into the firmware.

Draws these tiles and writes them to firmware/data/logos/:
  _CESSNA.rgb565  - Cessna side view with a red stripe (known Cessnas)
  _LIGHT.rgb565   - the same plane without the stripe (Piper, Cirrus, ...)
  _BIZJET.rgb565  - business jet side view (T-tail, rear engine)
  _PRIVATE.rgb565 - airliner side view (tail-number flight, type unknown)

Then embeds EVERY firmware/data/logos/_*.rgb565 (so also _HELI and _CARGO from
gen_special_logos.py) into firmware/core/GenericTiles.cpp. The device prefers
those built-in copies over LittleFS, so a normal firmware/OTA upload carries
them and no uploadfs is needed.

No dependencies:  python tools/gen_generic_tiles.py
Then rebuild the firmware.
"""
import os
import struct

S = 32
ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
LOGOS = os.path.join(ROOT, "firmware", "data", "logos")
OUT_CPP = os.path.join(ROOT, "firmware", "core", "GenericTiles.cpp")

BLACK = (0, 0, 0)
W = (232, 236, 245)   # body
G = (150, 156, 168)   # detail grey
DK = (95, 100, 112)   # dark grey (prop, gear)
BL = (80, 150, 220)   # windows
RD = (210, 45, 45)    # Cessna stripe


# ---- tiny rasterizer (pixel-centre sampling) ----
def blank():
    return [[BLACK] * S for _ in range(S)]


def poly(img, pts, c):
    n = len(pts)
    for y in range(S):
        for x in range(S):
            px, py = x + 0.5, y + 0.5
            inside = False
            j = n - 1
            for i in range(n):
                xi, yi = pts[i]
                xj, yj = pts[j]
                if (yi > py) != (yj > py) and px < (xj - xi) * (py - yi) / (yj - yi) + xi:
                    inside = not inside
                j = i
            if inside:
                img[y][x] = c


def rect(img, x0, y0, x1, y1, c):  # inclusive
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            if 0 <= x < S and 0 <= y < S:
                img[y][x] = c


def line(img, x0, y0, x1, y1, c):  # Bresenham
    dx, dy = abs(x1 - x0), -abs(y1 - y0)
    sx, sy = (1 if x0 < x1 else -1), (1 if y0 < y1 else -1)
    err = dx + dy
    while True:
        if 0 <= x0 < S and 0 <= y0 < S:
            img[y0][x0] = c
        if x0 == x1 and y0 == y1:
            break
        e2 = 2 * err
        if e2 >= dy:
            err += dy
            x0 += sx
        if e2 <= dx:
            err += dx
            y0 += sy


def disc(img, cx, cy, r, c):
    for y in range(S):
        for x in range(S):
            if (x + 0.5 - cx) ** 2 + (y + 0.5 - cy) ** 2 <= r * r:
                img[y][x] = c


# ---- the tiles (all nose right) ----
def cessna_side(stripe):
    img = blank()
    poly(img, [(2.5, 6), (5.5, 6), (10, 13), (2.5, 14)], W)                 # tail fin
    # The cabin rises to meet the wing; the rear fuselage and nose sit lower,
    # so the wing stands clear of the body in front of and behind the cabin.
    poly(img, [(2, 13.5), (13, 12), (13, 10), (22, 10), (23, 12.5), (28, 13.2),
               (29.5, 15.5), (28, 18), (22, 19), (12, 18.5), (2, 15.5)], W)
    rect(img, 0, 14, 7, 14, G)                                              # stabilizer edge
    rect(img, 10, 8, 25, 9, W)                                              # high wing, edge-on
    poly(img, [(22.2, 10.5), (23.5, 10.5), (26.5, 13.5), (22.2, 13.5)], BL) # windshield
    rect(img, 14, 11, 16, 13, BL)                                           # side windows
    rect(img, 18, 11, 20, 13, BL)
    line(img, 20, 10, 17, 18, G)                                            # wing strut
    if stripe:
        rect(img, 3, 15, 28, 15, RD)
    rect(img, 30, 9, 30, 21, DK)                                            # propeller
    rect(img, 29, 14, 30, 16, G)                                            # spinner
    line(img, 17, 19, 15, 22, DK)                                           # main gear
    disc(img, 15, 23.5, 1.6, DK)
    line(img, 26, 18, 26, 22, DK)                                           # nose gear
    disc(img, 26.5, 23.5, 1.4, DK)
    return img


def bizjet_side():
    img = blank()
    poly(img, [(3, 14.5), (24, 13), (28.5, 14), (31.5, 16.2), (28.5, 18),
               (24, 18.6), (6, 18), (3, 16.5)], W)                         # slim fuselage
    poly(img, [(3, 15), (4, 6), (7, 6), (11, 14.5)], W)                     # swept fin
    rect(img, 1, 5, 9, 5, W)                                                # T-tail
    rect(img, 2, 6, 8, 6, G)
    poly(img, [(8, 11.5), (15, 11.5), (16, 13), (15, 14.5), (8, 14.5)], G)  # rear engine
    rect(img, 15, 12, 16, 14, DK)
    poly(img, [(12, 17.5), (21, 17.5), (15, 21), (10, 21)], G)              # low swept wing
    for x in (17, 19, 21, 23):
        img[15][x] = BL                                                     # cabin windows
    poly(img, [(25, 14), (28, 14.4), (29.5, 15.8), (25, 15.8)], BL)         # cockpit
    return img


def airliner_side():
    img = blank()
    poly(img, [(2, 13), (8, 12), (25, 12), (29, 13.5), (31.5, 16), (29, 18.5),
               (25, 19), (8, 19), (2, 16)], W)                              # fuselage
    poly(img, [(2, 13.5), (3, 4), (6.5, 4), (11, 13)], W)                   # fin
    rect(img, 0, 15, 6, 15, G)                                              # stabilizer
    poly(img, [(12, 18), (20, 18), (14, 23), (10, 23)], G)                  # wing
    poly(img, [(13, 20), (18, 20), (18.5, 21.5), (18, 23), (13, 23)], DK)   # engine
    for x in range(10, 25, 2):
        img[14][x] = BL                                                     # windows
    poly(img, [(26, 13), (28.5, 13.6), (30, 15), (26, 15)], BL)             # cockpit
    return img


# ---- output ----
def rgb565(r, g, b):
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def tile_bytes(img):
    out = bytearray(struct.pack("<HH", S, S))
    for row in img:
        for r, g, b in row:
            out += struct.pack("<H", rgb565(r, g, b))
    return bytes(out)


def write_tile(name, img):
    with open(os.path.join(LOGOS, name + ".rgb565"), "wb") as f:
        f.write(tile_bytes(img))
    print("wrote", name + ".rgb565")


def write_cpp():
    names = sorted(f[:-7] for f in os.listdir(LOGOS) if f.startswith("_") and f.endswith(".rgb565"))
    parts = [
        "// Generated by tools/gen_generic_tiles.py -- do not edit; rerun the script.\n"
        "#include \"core/GenericTiles.h\"\n"
        "#include <string.h>\n"
        "#include <pgmspace.h>\n"
    ]
    for name in names:
        data = open(os.path.join(LOGOS, name + ".rgb565"), "rb").read()
        rows = ["    " + ", ".join("0x%02x" % b for b in data[i:i + 16]) + ","
                for i in range(0, len(data), 16)]
        parts.append("\nstatic const uint8_t kTile%s[] PROGMEM = {\n%s\n};\n" % (name, "\n".join(rows)))
    parts.append("\nstruct BuiltinTileEntry\n{\n    const char *key;\n    const uint8_t *data;\n    size_t len;\n};\n")
    parts.append("\nstatic const BuiltinTileEntry kBuiltinTiles[] = {\n")
    for name in names:
        parts.append("    {\"%s\", kTile%s, sizeof(kTile%s)},\n" % (name, name, name))
    parts.append("};\n")
    parts.append(
        "\nconst uint8_t *builtinTile(const char *key, size_t &len)\n{\n"
        "    for (const BuiltinTileEntry &t : kBuiltinTiles)\n"
        "        if (key && strcmp(key, t.key) == 0)\n"
        "        {\n            len = t.len;\n            return t.data;\n        }\n"
        "    len = 0;\n    return nullptr;\n}\n"
        "\nsize_t builtinTileCount() { return sizeof(kBuiltinTiles) / sizeof(kBuiltinTiles[0]); }\n"
        "\nconst char *builtinTileKey(size_t i)\n{\n"
        "    return i < builtinTileCount() ? kBuiltinTiles[i].key : nullptr;\n}\n"
    )
    with open(OUT_CPP, "w", newline="\n") as f:
        f.write("".join(parts))
    print("embedded", ", ".join(names), "->", os.path.relpath(OUT_CPP, ROOT))


def main():
    os.makedirs(LOGOS, exist_ok=True)
    write_tile("_CESSNA", cessna_side(stripe=True))
    write_tile("_LIGHT", cessna_side(stripe=False))
    write_tile("_BIZJET", bizjet_side())
    write_tile("_PRIVATE", airliner_side())
    write_cpp()


if __name__ == "__main__":
    main()
