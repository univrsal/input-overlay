# Sprite creator (CCT)

The config creation tool (`docs/cct/`) now includes a **sprite creator** for building texture atlases without an external image editor.

## Opening it

1. Open [io-cct](https://univrsal.github.io/input-overlay/cct/) (or `docs/cct/index.html` locally).
2. On the **Setup** dialog, click **Or create sprites first…**, **or**
3. After starting the editor, click **Create sprites…** in the element panel.

## Workflow

1. **Add sprites** — set ID, optional label, width/height, then **Add sprite**.
2. **Paint** — use Pencil, Eraser, Fill, Rect, Circle, or Pick (eyedropper) on the center canvas. Adjust brush size and zoom as needed.
3. **Pack** — sprites are automatically laid out into an atlas with the **3 px spacing** required by input-overlay (`CFG_INNER_BORDER` / `constants.texture_space`).
4. **Templates** (optional) — replace the atlas with a starter set:
   - **WASD keyboard set** — Q/W/E/A/S/D and common keys, each with idle + pressed variants
   - **Gamepad face buttons** — A/B/X/Y, shoulders, sticks, d-pad, start/back
   - **Mouse buttons** — body, L/R/M click states, scroll wheel, movement dot
5. **Export**
   - **Download PNG texture** — the atlas image for OBS / the CCT texture drop zone
   - **Download mapping JSON** — starter config with `mapping: [u, v, w, h]` per sprite
   - **Use in config editor** — loads the PNG as the active texture, seeds default element size, and updates UV on any existing elements that share a sprite ID

## Tips

- Keep **transparent atlas background** checked unless you need a solid backdrop.
- Element **pressed** states in input-overlay usually come from a second texture region to the right of (or below) the idle region; templates emit `*_pressed` ids for this pattern.
- Refine template pixels after generating — they are simple geometric starters, not final art.
- IDs in the sprite list should match element `id` values in your JSON if you want **Use in config editor** to patch UVs automatically.

## Files

| File | Role |
|------|------|
| `js/sprite.js` | Core: cells, atlas packer, pixel painter, dialog controller |
| `js/sprite_templates.js` | Keyboard / gamepad / mouse quick templates |
| `css/sprite.css` | Dialog layout and controls |
| `index.html` | Dialog markup + script/style includes |

## Keyboard shortcuts / tools

| Tool | Behavior |
|------|----------|
| Pencil | Draw with current color and brush size |
| Eraser | Clear pixels (transparent) |
| Fill | Flood-fill from clicked pixel |
| Rect / Circle | Drag to place a filled shape |
| Pick | Sample color from the sprite into the color picker |
