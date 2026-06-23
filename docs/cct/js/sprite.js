/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.cc/alex/input-overlay
 * Copyright 2025 univrsal <uni@vrsal.xyz>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

/**
 * Sprite / texture-atlas builder for input-overlay CCT.
 *
 * Users paint simple key/button sprites on a canvas, arrange them into an
 * atlas with the required 3px inner border (CFG_INNER_BORDER), then export
 * a PNG that can be dropped straight into the config editor.
 */

const SPRITE_BORDER = 3; /* must match CFG_INNER_BORDER / constants.texture_space */
const SPRITE_OUTER_BORDER = 1;

let sprite_creator = null;

class sprite_cell {
  constructor(id, label, w, h, x, y) {
    this.id = id || ('cell_' + Date.now());
    this.label = label || 'Sprite';
    this.w = w || 64;
    this.h = h || 64;
    this.x = x || 0;
    this.y = y || 0;
    /* offscreen canvas holding the pixel data for this cell */
    this.canvas = document.createElement('canvas');
    this.canvas.width = this.w;
    this.canvas.height = this.h;
    this.ctx = this.canvas.getContext('2d');
    this.ctx.imageSmoothingEnabled = false;
    this.fill_default();
  }

  fill_default() {
    const c = this.ctx;
    c.fillStyle = '#2a2a2e';
    c.fillRect(0, 0, this.w, this.h);
    c.strokeStyle = '#555';
    c.lineWidth = 2;
    c.strokeRect(1, 1, this.w - 2, this.h - 2);
    c.fillStyle = '#888';
    c.font = Math.max(10, Math.floor(this.h / 4)) + 'px sans-serif';
    c.textAlign = 'center';
    c.textBaseline = 'middle';
    c.fillText(this.label, this.w / 2, this.h / 2);
  }

  resize(w, h) {
    const img = this.ctx.getImageData(0, 0, this.w, this.h);
    this.w = Math.max(1, w | 0);
    this.h = Math.max(1, h | 0);
    this.canvas.width = this.w;
    this.canvas.height = this.h;
    this.ctx = this.canvas.getContext('2d');
    this.ctx.imageSmoothingEnabled = false;
    this.ctx.putImageData(img, 0, 0);
  }

  clear(color) {
    this.ctx.fillStyle = color || 'rgba(0,0,0,0)';
    this.ctx.clearRect(0, 0, this.w, this.h);
    if (color) {
      this.ctx.fillStyle = color;
      this.ctx.fillRect(0, 0, this.w, this.h);
    }
  }

  paint_rect(x, y, w, h, color) {
    this.ctx.fillStyle = color;
    this.ctx.fillRect(x, y, w, h);
  }

  paint_circle(cx, cy, r, color) {
    this.ctx.fillStyle = color;
    this.ctx.beginPath();
    this.ctx.arc(cx, cy, r, 0, Math.PI * 2);
    this.ctx.fill();
  }

  paint_round_rect(x, y, w, h, r, color) {
    const c = this.ctx;
    c.fillStyle = color;
    c.beginPath();
    c.moveTo(x + r, y);
    c.lineTo(x + w - r, y);
    c.quadraticCurveTo(x + w, y, x + w, y + r);
    c.lineTo(x + w, y + h - r);
    c.quadraticCurveTo(x + w, y + h, x + w - r, y + h);
    c.lineTo(x + r, y + h);
    c.quadraticCurveTo(x, y + h, x, y + h - r);
    c.lineTo(x, y + r);
    c.quadraticCurveTo(x, y, x + r, y);
    c.closePath();
    c.fill();
  }

  draw_label(text, color, font_size) {
    const c = this.ctx;
    c.fillStyle = color || '#fff';
    c.font = (font_size || Math.max(10, Math.floor(this.h / 3))) + 'px sans-serif';
    c.textAlign = 'center';
    c.textBaseline = 'middle';
    c.fillText(text, this.w / 2, this.h / 2);
  }

  to_data_url() {
    return this.canvas.toDataURL('image/png');
  }
}

class sprite_atlas {
  constructor() {
    this.cells = [];
    this.padding = SPRITE_BORDER;
    this.outer_padding = SPRITE_OUTER_BORDER;
    this.bg_color = 'rgba(0,0,0,0)';
    this.atlas_w = 256;
    this.atlas_h = 256;
    this.auto_pack = true;
  }

  add_cell(cell) {
    this.cells.push(cell);
    if (this.auto_pack) this.pack();
    return cell;
  }

  remove_cell(id) {
    this.cells = this.cells.filter((c) => c.id !== id);
    if (this.auto_pack) this.pack();
  }

  get_cell(id) {
    return this.cells.find((c) => c.id === id) || null;
  }

  clear() {
    this.cells = [];
    this.atlas_w = 256;
    this.atlas_h = 256;
  }

  /**
   * Simple row-based packer with required sprite borders.
   * Layout: outer_padding, then cells with padding between them.
   */
  pack() {
    if (this.cells.length === 0) {
      this.atlas_w = 256;
      this.atlas_h = 256;
      return;
    }

    const pad = this.padding;
    const outer = this.outer_padding;
    let x = outer;
    let y = outer;
    let row_h = 0;
    let max_w = outer;
    let max_h = outer;

    /* estimate a reasonable row width from the largest cell */
    let max_cell_w = 0;
    this.cells.forEach((c) => {
      if (c.w > max_cell_w) max_cell_w = c.w;
    });
    const target_row_w = Math.max(256, max_cell_w * 4 + pad * 5 + outer * 2);

    this.cells.forEach((cell) => {
      if (x + cell.w + outer > target_row_w && x > outer) {
        x = outer;
        y += row_h + pad;
        row_h = 0;
      }
      cell.x = x;
      cell.y = y;
      x += cell.w + pad;
      if (cell.h > row_h) row_h = cell.h;
      if (x > max_w) max_w = x;
      if (y + cell.h > max_h) max_h = y + cell.h;
    });

    this.atlas_w = max_w + outer - pad + outer;
    this.atlas_h = max_h + outer;
    /* clamp to at least 1px */
    this.atlas_w = Math.max(1, this.atlas_w);
    this.atlas_h = Math.max(1, this.atlas_h);
  }

  /** Render all cells into an offscreen canvas and return it. */
  render() {
    if (this.auto_pack) this.pack();
    const canvas = document.createElement('canvas');
    canvas.width = this.atlas_w;
    canvas.height = this.atlas_h;
    const ctx = canvas.getContext('2d');
    ctx.imageSmoothingEnabled = false;

    if (this.bg_color && this.bg_color !== 'rgba(0,0,0,0)') {
      ctx.fillStyle = this.bg_color;
      ctx.fillRect(0, 0, this.atlas_w, this.atlas_h);
    } else {
      ctx.clearRect(0, 0, this.atlas_w, this.atlas_h);
    }

    this.cells.forEach((cell) => {
      ctx.drawImage(cell.canvas, cell.x, cell.y);
    });

    return canvas;
  }

  to_data_url() {
    return this.render().toDataURL('image/png');
  }

  /** Produce a minimal overlay JSON skeleton using current cell mappings. */
  to_config_json(default_w, default_h) {
    const elements = this.cells.map((cell, i) => ({
      id: cell.id,
      type: 0 /* TEXTURE placeholder; user remaps in editor */,
      z_level: 1,
      pos: [cell.x, cell.y],
      mapping: [cell.x, cell.y, cell.w, cell.h],
    }));
    return {
      default_width: default_w || (this.cells[0] ? this.cells[0].w : 64),
      default_height: default_h || (this.cells[0] ? this.cells[0].h : 64),
      space_h: 0,
      space_v: 0,
      elements: elements,
    };
  }

  /** Export cell metadata useful for copy/paste into CCT element editor. */
  export_mappings() {
    return this.cells.map((c) => ({
      id: c.id,
      label: c.label,
      u: c.x,
      v: c.y,
      w: c.w,
      h: c.h,
    }));
  }
}

class sprite_painter {
  constructor(canvas_id) {
    this.canvas = document.getElementById(canvas_id);
    if (!this.canvas) return;
    this.ctx = this.canvas.getContext('2d');
    this.ctx.imageSmoothingEnabled = false;
    this.cell = null;
    this.tool = 'pencil'; /* pencil | eraser | fill | rect | circle | eyedropper */
    this.color = '#ffffff';
    this.brush_size = 1;
    this.drawing = false;
    this.last_x = 0;
    this.last_y = 0;
    this.rect_start = null;
    this.zoom = 4;
    this.checker_a = '#1a1a1e';
    this.checker_b = '#222228';
    this._bind_events();
  }

  _bind_events() {
    if (!this.canvas) return;
    this.canvas.addEventListener('mousedown', (e) => this._on_down(e));
    this.canvas.addEventListener('mousemove', (e) => this._on_move(e));
    this.canvas.addEventListener('mouseup', (e) => this._on_up(e));
    this.canvas.addEventListener('mouseleave', (e) => this._on_up(e));
    this.canvas.addEventListener('contextmenu', (e) => e.preventDefault());
  }

  set_cell(cell) {
    this.cell = cell;
    this._resize_canvas();
    this.redraw();
  }

  set_tool(tool) {
    this.tool = tool;
  }

  set_color(color) {
    this.color = color;
  }

  set_brush_size(size) {
    this.brush_size = Math.max(1, size | 0);
  }

  set_zoom(z) {
    this.zoom = Math.max(1, Math.min(32, z | 0));
    this._resize_canvas();
    this.redraw();
  }

  _resize_canvas() {
    if (!this.cell || !this.canvas) return;
    this.canvas.width = this.cell.w * this.zoom;
    this.canvas.height = this.cell.h * this.zoom;
  }

  _pixel_pos(e) {
    const rect = this.canvas.getBoundingClientRect();
    const scale_x = this.canvas.width / rect.width;
    const scale_y = this.canvas.height / rect.height;
    const cx = (e.clientX - rect.left) * scale_x;
    const cy = (e.clientY - rect.top) * scale_y;
    return {
      x: Math.floor(cx / this.zoom),
      y: Math.floor(cy / this.zoom),
    };
  }

  _draw_pixel(x, y, color) {
    if (!this.cell) return;
    const s = this.brush_size;
    const half = Math.floor(s / 2);
    this.cell.ctx.fillStyle = color;
    for (let dy = 0; dy < s; dy++) {
      for (let dx = 0; dx < s; dx++) {
        const px = x - half + dx;
        const py = y - half + dy;
        if (px >= 0 && py >= 0 && px < this.cell.w && py < this.cell.h) {
          if (this.tool === 'eraser') {
            this.cell.ctx.clearRect(px, py, 1, 1);
          } else {
            this.cell.ctx.fillRect(px, py, 1, 1);
          }
        }
      }
    }
  }

  _flood_fill(sx, sy, fill_color) {
    if (!this.cell) return;
    const w = this.cell.w;
    const h = this.cell.h;
    const img = this.cell.ctx.getImageData(0, 0, w, h);
    const data = img.data;
    const idx = (x, y) => (y * w + x) * 4;

    const target = [
      data[idx(sx, sy)],
      data[idx(sx, sy) + 1],
      data[idx(sx, sy) + 2],
      data[idx(sx, sy) + 3],
    ];

    /* parse fill_color (#rrggbb or rgba) */
    const tmp = document.createElement('canvas');
    tmp.width = tmp.height = 1;
    const tctx = tmp.getContext('2d');
    tctx.fillStyle = fill_color;
    tctx.fillRect(0, 0, 1, 1);
    const fc = tctx.getImageData(0, 0, 1, 1).data;
    const fill = [fc[0], fc[1], fc[2], fc[3]];

    if (
      target[0] === fill[0] &&
      target[1] === fill[1] &&
      target[2] === fill[2] &&
      target[3] === fill[3]
    )
      return;

    const stack = [[sx, sy]];
    const visited = new Uint8Array(w * h);

    while (stack.length) {
      const [x, y] = stack.pop();
      if (x < 0 || y < 0 || x >= w || y >= h) continue;
      const vi = y * w + x;
      if (visited[vi]) continue;
      const i = idx(x, y);
      if (
        data[i] !== target[0] ||
        data[i + 1] !== target[1] ||
        data[i + 2] !== target[2] ||
        data[i + 3] !== target[3]
      )
        continue;
      visited[vi] = 1;
      data[i] = fill[0];
      data[i + 1] = fill[1];
      data[i + 2] = fill[2];
      data[i + 3] = fill[3];
      stack.push([x + 1, y], [x - 1, y], [x, y + 1], [x, y - 1]);
    }

    this.cell.ctx.putImageData(img, 0, 0);
  }

  _sample_color(x, y) {
    if (!this.cell) return this.color;
    const img = this.cell.ctx.getImageData(x, y, 1, 1).data;
    const hex =
      '#' +
      [img[0], img[1], img[2]]
        .map((v) => v.toString(16).padStart(2, '0'))
        .join('');
    return hex;
  }

  _on_down(e) {
    if (!this.cell) return;
    const p = this._pixel_pos(e);
    this.drawing = true;
    this.last_x = p.x;
    this.last_y = p.y;

    if (this.tool === 'eyedropper') {
      this.color = this._sample_color(p.x, p.y);
      const picker = document.getElementById('sprite-color');
      if (picker) picker.value = this.color;
      this.drawing = false;
      return;
    }

    if (this.tool === 'fill') {
      this._flood_fill(p.x, p.y, this.color);
      this.drawing = false;
      this.redraw();
      if (sprite_creator) sprite_creator.refresh_atlas_preview();
      return;
    }

    if (this.tool === 'rect' || this.tool === 'circle') {
      this.rect_start = { x: p.x, y: p.y };
      return;
    }

    this._draw_pixel(p.x, p.y, this.color);
    this.redraw();
  }

  _on_move(e) {
    if (!this.drawing || !this.cell) return;
    const p = this._pixel_pos(e);

    if (this.tool === 'rect' || this.tool === 'circle') {
      this.redraw();
      /* preview shape on top */
      const ctx = this.ctx;
      ctx.save();
      ctx.strokeStyle = this.color;
      ctx.lineWidth = this.zoom;
      const x0 = this.rect_start.x * this.zoom + this.zoom / 2;
      const y0 = this.rect_start.y * this.zoom + this.zoom / 2;
      const x1 = p.x * this.zoom + this.zoom / 2;
      const y1 = p.y * this.zoom + this.zoom / 2;
      if (this.tool === 'rect') {
        ctx.strokeRect(
          Math.min(x0, x1),
          Math.min(y0, y1),
          Math.abs(x1 - x0),
          Math.abs(y1 - y0)
        );
      } else {
        const rx = Math.abs(x1 - x0) / 2;
        const ry = Math.abs(y1 - y0) / 2;
        ctx.beginPath();
        ctx.ellipse(
          (x0 + x1) / 2,
          (y0 + y1) / 2,
          rx,
          ry,
          0,
          0,
          Math.PI * 2
        );
        ctx.stroke();
      }
      ctx.restore();
      return;
    }

    /* Bresenham line between last and current */
    let x0 = this.last_x;
    let y0 = this.last_y;
    const x1 = p.x;
    const y1 = p.y;
    const dx = Math.abs(x1 - x0);
    const dy = Math.abs(y1 - y0);
    const sx = x0 < x1 ? 1 : -1;
    const sy = y0 < y1 ? 1 : -1;
    let err = dx - dy;
    while (true) {
      this._draw_pixel(x0, y0, this.color);
      if (x0 === x1 && y0 === y1) break;
      const e2 = 2 * err;
      if (e2 > -dy) {
        err -= dy;
        x0 += sx;
      }
      if (e2 < dx) {
        err += dx;
        y0 += sy;
      }
    }
    this.last_x = p.x;
    this.last_y = p.y;
    this.redraw();
  }

  _on_up(e) {
    if (!this.drawing || !this.cell) {
      this.drawing = false;
      return;
    }
    const p = this._pixel_pos(e);

    if ((this.tool === 'rect' || this.tool === 'circle') && this.rect_start) {
      const x0 = Math.min(this.rect_start.x, p.x);
      const y0 = Math.min(this.rect_start.y, p.y);
      const x1 = Math.max(this.rect_start.x, p.x);
      const y1 = Math.max(this.rect_start.y, p.y);
      const w = x1 - x0 + 1;
      const h = y1 - y0 + 1;
      if (this.tool === 'rect') {
        this.cell.paint_rect(x0, y0, w, h, this.color);
      } else {
        const cx = x0 + w / 2;
        const cy = y0 + h / 2;
        const r = Math.max(w, h) / 2;
        this.cell.paint_circle(cx, cy, r, this.color);
      }
      this.rect_start = null;
    }

    this.drawing = false;
    this.redraw();
    if (sprite_creator) sprite_creator.refresh_atlas_preview();
  }

  redraw() {
    if (!this.cell || !this.canvas) return;
    const ctx = this.ctx;
    const z = this.zoom;
    const w = this.cell.w;
    const h = this.cell.h;

    /* checkerboard background for transparency */
    for (let y = 0; y < h; y++) {
      for (let x = 0; x < w; x++) {
        ctx.fillStyle = (x + y) % 2 === 0 ? this.checker_a : this.checker_b;
        ctx.fillRect(x * z, y * z, z, z);
      }
    }

    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(this.cell.canvas, 0, 0, w * z, h * z);

    /* pixel grid when zoomed in */
    if (z >= 4) {
      ctx.strokeStyle = 'rgba(255,255,255,0.08)';
      ctx.lineWidth = 1;
      for (let x = 0; x <= w; x++) {
        ctx.beginPath();
        ctx.moveTo(x * z + 0.5, 0);
        ctx.lineTo(x * z + 0.5, h * z);
        ctx.stroke();
      }
      for (let y = 0; y <= h; y++) {
        ctx.beginPath();
        ctx.moveTo(0, y * z + 0.5);
        ctx.lineTo(w * z, y * z + 0.5);
        ctx.stroke();
      }
    }
  }
}

class sprite_creator_ui {
  constructor() {
    this.atlas = new sprite_atlas();
    this.painter = null;
    this.selected_id = null;
    this.default_cell_w = 64;
    this.default_cell_h = 64;
  }

  init() {
    this.painter = new sprite_painter('sprite-paint-canvas');
    this._bind_ui();
    this.refresh_cell_list();
    this.refresh_atlas_preview();
  }

  _bind_ui() {
    const $id = (id) => document.getElementById(id);

    const tool_btns = document.querySelectorAll('[data-sprite-tool]');
    tool_btns.forEach((btn) => {
      btn.addEventListener('click', () => {
        tool_btns.forEach((b) => b.classList.remove('active'));
        btn.classList.add('active');
        if (this.painter) this.painter.set_tool(btn.dataset.spriteTool);
      });
    });

    const color = $id('sprite-color');
    if (color)
      color.addEventListener('input', () => {
        if (this.painter) this.painter.set_color(color.value);
      });

    const brush = $id('sprite-brush-size');
    if (brush)
      brush.addEventListener('input', () => {
        if (this.painter) this.painter.set_brush_size(parseInt(brush.value, 10));
        const lbl = $id('sprite-brush-size-label');
        if (lbl) lbl.textContent = brush.value + 'px';
      });

    const zoom = $id('sprite-zoom');
    if (zoom)
      zoom.addEventListener('input', () => {
        if (this.painter) this.painter.set_zoom(parseInt(zoom.value, 10));
        const lbl = $id('sprite-zoom-label');
        if (lbl) lbl.textContent = zoom.value + 'x';
      });

    const add_btn = $id('sprite-add-cell');
    if (add_btn) add_btn.addEventListener('click', () => this.add_cell());

    const del_btn = $id('sprite-delete-cell');
    if (del_btn) del_btn.addEventListener('click', () => this.delete_selected());

    const clear_btn = $id('sprite-clear-cell');
    if (clear_btn)
      clear_btn.addEventListener('click', () => this.clear_selected());

    const apply_size = $id('sprite-apply-cell-size');
    if (apply_size)
      apply_size.addEventListener('click', () => this.apply_cell_size());

    const export_png = $id('sprite-export-png');
    if (export_png)
      export_png.addEventListener('click', () => this.export_png());

    const export_json = $id('sprite-export-json');
    if (export_json)
      export_json.addEventListener('click', () => this.export_json());

    const use_in_cct = $id('sprite-use-in-cct');
    if (use_in_cct)
      use_in_cct.addEventListener('click', () => this.use_in_cct());

    const close_btn = $id('sprite-close');
    if (close_btn) close_btn.addEventListener('click', () => this.hide());

    const open_btn = $id('open-sprite-creator');
    if (open_btn) open_btn.addEventListener('click', () => this.show());

    const tpl_kb = $id('sprite-template-keyboard');
    if (tpl_kb)
      tpl_kb.addEventListener('click', () => this.apply_keyboard_template());

    const tpl_gp = $id('sprite-template-gamepad');
    if (tpl_gp)
      tpl_gp.addEventListener('click', () => this.apply_gamepad_template());

    const tpl_mouse = $id('sprite-template-mouse');
    if (tpl_mouse)
      tpl_mouse.addEventListener('click', () => this.apply_mouse_template());

    const bg = $id('sprite-atlas-bg');
    if (bg)
      bg.addEventListener('input', () => {
        this.atlas.bg_color = bg.value === '#000000' && bg.dataset.transparent === '1'
          ? 'rgba(0,0,0,0)'
          : bg.value;
        /* allow explicit transparent via checkbox */
        const tr = $id('sprite-atlas-transparent');
        if (tr && tr.checked) this.atlas.bg_color = 'rgba(0,0,0,0)';
        else if (tr && !tr.checked) this.atlas.bg_color = bg.value;
        this.refresh_atlas_preview();
      });

    const tr = $id('sprite-atlas-transparent');
    if (tr)
      tr.addEventListener('change', () => {
        const bgEl = $id('sprite-atlas-bg');
        if (tr.checked) this.atlas.bg_color = 'rgba(0,0,0,0)';
        else if (bgEl) this.atlas.bg_color = bgEl.value;
        this.refresh_atlas_preview();
      });
  }

  show() {
    const dlg = document.getElementById('sprite-creator-dialog');
    if (dlg) {
      dlg.style.display = 'flex';
      if (this.atlas.cells.length === 0) this.add_cell('key_1', 'A');
      this.refresh_cell_list();
      this.refresh_atlas_preview();
      if (this.selected_id) {
        const cell = this.atlas.get_cell(this.selected_id);
        if (cell && this.painter) this.painter.set_cell(cell);
      }
    }
  }

  hide() {
    const dlg = document.getElementById('sprite-creator-dialog');
    if (dlg) dlg.style.display = 'none';
  }

  add_cell(id, label) {
    const w_el = document.getElementById('sprite-cell-w');
    const h_el = document.getElementById('sprite-cell-h');
    const id_el = document.getElementById('sprite-cell-id');
    const label_el = document.getElementById('sprite-cell-label');

    const w = w_el ? parseInt(w_el.value, 10) || 64 : this.default_cell_w;
    const h = h_el ? parseInt(h_el.value, 10) || 64 : this.default_cell_h;
    const cell_id =
      id ||
      (id_el && id_el.value.trim()
        ? id_el.value.trim()
        : 'sprite_' + (this.atlas.cells.length + 1));
    const cell_label =
      label ||
      (label_el && label_el.value.trim()
        ? label_el.value.trim()
        : cell_id);

    if (this.atlas.get_cell(cell_id)) {
      alert('A sprite with id "' + cell_id + '" already exists.');
      return null;
    }

    const cell = new sprite_cell(cell_id, cell_label, w, h);
    this.atlas.add_cell(cell);
    this.select_cell(cell_id);
    this.refresh_cell_list();
    this.refresh_atlas_preview();
    return cell;
  }

  select_cell(id) {
    this.selected_id = id;
    const cell = this.atlas.get_cell(id);
    if (cell && this.painter) this.painter.set_cell(cell);

    const id_el = document.getElementById('sprite-cell-id');
    const label_el = document.getElementById('sprite-cell-label');
    const w_el = document.getElementById('sprite-cell-w');
    const h_el = document.getElementById('sprite-cell-h');
    if (cell) {
      if (id_el) id_el.value = cell.id;
      if (label_el) label_el.value = cell.label;
      if (w_el) w_el.value = cell.w;
      if (h_el) h_el.value = cell.h;
    }

    document.querySelectorAll('.sprite-cell-item').forEach((el) => {
      el.classList.toggle('active', el.dataset.cellId === id);
    });
  }

  delete_selected() {
    if (!this.selected_id) return;
    this.atlas.remove_cell(this.selected_id);
    this.selected_id = this.atlas.cells.length
      ? this.atlas.cells[0].id
      : null;
    if (this.selected_id) this.select_cell(this.selected_id);
    else if (this.painter) this.painter.set_cell(null);
    this.refresh_cell_list();
    this.refresh_atlas_preview();
  }

  clear_selected() {
    const cell = this.atlas.get_cell(this.selected_id);
    if (!cell) return;
    cell.clear();
    if (this.painter) this.painter.redraw();
    this.refresh_atlas_preview();
  }

  apply_cell_size() {
    const cell = this.atlas.get_cell(this.selected_id);
    if (!cell) return;
    const w_el = document.getElementById('sprite-cell-w');
    const h_el = document.getElementById('sprite-cell-h');
    const w = w_el ? parseInt(w_el.value, 10) || cell.w : cell.w;
    const h = h_el ? parseInt(h_el.value, 10) || cell.h : cell.h;
    cell.resize(w, h);
    if (this.painter) this.painter.set_cell(cell);
    this.atlas.pack();
    this.refresh_atlas_preview();
  }

  refresh_cell_list() {
    const list = document.getElementById('sprite-cell-list');
    if (!list) return;
    list.innerHTML = '';
    this.atlas.cells.forEach((cell) => {
      const item = document.createElement('div');
      item.className = 'sprite-cell-item' + (cell.id === this.selected_id ? ' active' : '');
      item.dataset.cellId = cell.id;
      item.innerHTML =
        '<span class="sprite-cell-thumb-wrap"><canvas class="sprite-cell-thumb" width="32" height="32"></canvas></span>' +
        '<span class="sprite-cell-meta"><strong>' +
        this._esc(cell.id) +
        '</strong><small>' +
        cell.w +
        '×' +
        cell.h +
        ' @ (' +
        cell.x +
        ',' +
        cell.y +
        ')</small></span>';
      const thumb = item.querySelector('.sprite-cell-thumb');
      const tctx = thumb.getContext('2d');
      tctx.imageSmoothingEnabled = false;
      tctx.fillStyle = '#111';
      tctx.fillRect(0, 0, 32, 32);
      tctx.drawImage(cell.canvas, 0, 0, 32, 32);
      item.addEventListener('click', () => this.select_cell(cell.id));
      list.appendChild(item);
    });

    const info = document.getElementById('sprite-atlas-info');
    if (info) {
      info.textContent =
        this.atlas.cells.length +
        ' sprite(s) · atlas ' +
        this.atlas.atlas_w +
        '×' +
        this.atlas.atlas_h +
        ' px · ' +
        SPRITE_BORDER +
        'px border';
    }
  }

  refresh_atlas_preview() {
    const preview = document.getElementById('sprite-atlas-preview');
    if (!preview) return;
    const rendered = this.atlas.render();
    preview.width = rendered.width;
    preview.height = rendered.height;
    const ctx = preview.getContext('2d');
    ctx.imageSmoothingEnabled = false;
    /* checkerboard */
    const cs = 8;
    for (let y = 0; y < preview.height; y += cs) {
      for (let x = 0; x < preview.width; x += cs) {
        ctx.fillStyle = ((x / cs + y / cs) | 0) % 2 === 0 ? '#1a1a1e' : '#222228';
        ctx.fillRect(x, y, cs, cs);
      }
    }
    ctx.drawImage(rendered, 0, 0);

    /* draw cell bounds */
    ctx.strokeStyle = 'rgba(100, 180, 255, 0.5)';
    ctx.lineWidth = 1;
    this.atlas.cells.forEach((cell) => {
      ctx.strokeRect(cell.x + 0.5, cell.y + 0.5, cell.w - 1, cell.h - 1);
      if (cell.id === this.selected_id) {
        ctx.strokeStyle = 'rgba(255, 200, 50, 0.9)';
        ctx.strokeRect(cell.x + 0.5, cell.y + 0.5, cell.w - 1, cell.h - 1);
        ctx.strokeStyle = 'rgba(100, 180, 255, 0.5)';
      }
    });

    this.refresh_cell_list();
  }

  export_png() {
    const url = this.atlas.to_data_url();
    const a = document.createElement('a');
    a.href = url;
    a.download = 'overlay-texture.png';
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
  }

  export_json() {
    const w_el = document.getElementById('sprite-cell-w');
    const h_el = document.getElementById('sprite-cell-h');
    const dw = w_el ? parseInt(w_el.value, 10) : 64;
    const dh = h_el ? parseInt(h_el.value, 10) : 64;
    const json = this.atlas.to_config_json(dw, dh);
    const blob = new Blob([JSON.stringify(json, null, 4)], {
      type: 'application/json',
    });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'overlay-sprites.json';
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
  }

  use_in_cct() {
    const url = this.atlas.to_data_url();
    atlas = new Image();
    atlas.src = url;
    atlas.onload = () => {
      const mappings = this.atlas.export_mappings();
      window._sprite_mappings = mappings;
      console.log('Sprite atlas loaded into CCT. Cell mappings:', mappings);

      if (typeof apply_sprite_defaults_to_setup === 'function')
        apply_sprite_defaults_to_setup();

      let applied = 0;
      if (typeof apply_sprite_mappings === 'function' && cfg && cfg.elements.length)
        applied = apply_sprite_mappings(mappings);

      this.hide();

      let msg =
        'Texture atlas loaded into the config editor (' +
        this.atlas.atlas_w +
        '×' +
        this.atlas.atlas_h +
        ' px, ' +
        this.atlas.cells.length +
        ' sprites).';
      if (applied > 0)
        msg += ' Updated UV mappings on ' + applied + ' existing element(s).';
      else
        msg +=
          ' Drop or create elements and set their U/V/W/H from the atlas preview, or import the mapping JSON.';
      alert(msg);
    };
  }

  /* ---- templates (filled in later commits; stubs here) ---- */

  apply_keyboard_template() {
    if (typeof sprite_templates !== 'undefined' && sprite_templates.keyboard) {
      sprite_templates.keyboard(this);
    } else {
      console.warn('Keyboard template not loaded yet');
    }
  }

  apply_gamepad_template() {
    if (typeof sprite_templates !== 'undefined' && sprite_templates.gamepad) {
      sprite_templates.gamepad(this);
    } else {
      console.warn('Gamepad template not loaded yet');
    }
  }

  apply_mouse_template() {
    if (typeof sprite_templates !== 'undefined' && sprite_templates.mouse) {
      sprite_templates.mouse(this);
    } else {
      console.warn('Mouse template not loaded yet');
    }
  }

  _esc(s) {
    const d = document.createElement('div');
    d.textContent = s;
    return d.innerHTML;
  }
}

function open_sprite_creator() {
  if (!sprite_creator) {
    sprite_creator = new sprite_creator_ui();
    sprite_creator.init();
  }
  sprite_creator.show();
}

function close_sprite_creator() {
  if (sprite_creator) sprite_creator.hide();
}
