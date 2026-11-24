"""Regenerate the PNG app icons in ../icons from the same design as icons/icon.svg.

Usage:  python tools/make-icons.py      (needs Pillow: pip install pillow)
"""
from pathlib import Path
from PIL import Image, ImageDraw

BG = (18, 18, 18, 255)        # #121212
ACCENT = (29, 185, 84, 255)   # #1DB954
OUT = Path(__file__).resolve().parent.parent / "icons"
SS = 4                        # supersampling for smooth edges


def note(d, s, cx, cy, r):
    """Draw the music note (same geometry as icon.svg) centred in a circle of radius r."""
    k = r / 176.0
    X = lambda x: cx + (x - 256) * k
    Y = lambda y: cy + (y - 256) * k
    # stem + flag
    d.polygon([(X(222), Y(150)), (X(342), Y(178)), (X(342), Y(238)), (X(250), Y(214)),
               (X(250), Y(344)), (X(222), Y(344))], fill=BG)
    # note head
    hr = 46 * k
    d.ellipse([X(204) - hr, Y(344) - hr, X(204) + hr, Y(344) + hr], fill=BG)


def render(size, maskable=False, rounded=True):
    s = size * SS
    img = Image.new("RGBA", (s, s), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    if maskable or not rounded:
        d.rectangle([0, 0, s, s], fill=BG)          # full bleed; the OS applies its own mask
    else:
        d.rounded_rectangle([0, 0, s - 1, s - 1], radius=int(s * 112 / 512), fill=BG)
    # maskable: keep everything inside the 80% safe zone
    r = s * (0.30 if maskable else 176 / 512)
    c = s / 2
    d.ellipse([c - r, c - r, c + r, c + r], fill=ACCENT)
    note(d, s, c, c, r)
    return img.resize((size, size), Image.LANCZOS)


OUT.mkdir(exist_ok=True)
render(192).save(OUT / "icon-192.png")
render(512).save(OUT / "icon-512.png")
render(512, maskable=True).save(OUT / "icon-maskable-512.png")
render(180, rounded=False).convert("RGB").save(OUT / "apple-touch-icon.png")   # iOS rounds it itself
print("Icons written to", OUT)
