"""Makes RawMeshes/Kenney/colormap_dark.png from Kenney's colormap.png: the bright yellow-orange cells that the carved pumpkin uses for the
insides of its cuts (red 255, green 166-200, blue 62-90) become near black, so the carved face stands out against the orange skin.
The body (a softer orange) and the stem (green) are left alone. Run with the Pillow package: python Tools/darken_pumpkin_colormap.py
Tools/import_kenney_pumpkin.py then imports the dark version."""
import os
from PIL import Image

folder = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "RawMeshes", "Kenney")
image = Image.open(os.path.join(folder, "colormap.png")).convert("RGBA")
pixels = image.load()
changed = 0
for y in range(image.height):
    for x in range(image.width):
        r, g, b, a = pixels[x, y]
        if r == 255 and 160 <= g <= 205 and 55 <= b <= 95:
            pixels[x, y] = (16, 7, 2, a)
            changed += 1
image.save(os.path.join(folder, "colormap_dark.png"))
print("darkened %d texels of the colour map" % changed)
