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
 * Quick-start sprite templates for the CCT sprite creator.
 * Each generator clears the current atlas and paints a set of starter sprites
 * using simple geometric shapes so users can refine them pixel-by-pixel.
 */

var sprite_templates = (function () {
  'use strict';

  const PALETTE = {
    key_bg: '#2b2b33',
    key_border: '#5a5a6a',
    key_top: '#3a3a48',
    key_text: '#e8e8f0',
    key_active_bg: '#4a6aaa',
    key_active_border: '#7aa0d8',
    key_active_text: '#ffffff',
    gp_face_a: '#3d8f4a',
    gp_face_b: '#b04040',
    gp_face_x: '#3a6aaa',
    gp_face_y: '#c9a030',
    gp_shoulder: '#4a4a58',
    gp_stick_base: '#2a2a32',
    gp_stick_knob: '#6a6a7a',
    mouse_body: '#2e2e38',
    mouse_btn: '#3a3a48',
    mouse_btn_press: '#5a7aaa',
    mouse_wheel: '#555566',
    mouse_scroll: '#8888aa',
  };

  function confirm_replace(ui, name) {
    if (ui.atlas.cells.length === 0) return true;
    return confirm(
      'Replace the current atlas with the ' +
        name +
        ' template? Unsaved sprite edits will be lost.'
    );
  }

  function paint_key_idle(cell, label) {
    const w = cell.w;
    const h = cell.h;
    const r = Math.max(4, Math.floor(Math.min(w, h) * 0.12));
    cell.clear();
    cell.paint_round_rect(0, 0, w, h, r, PALETTE.key_border);
    cell.paint_round_rect(2, 2, w - 4, h - 6, r - 1, PALETTE.key_bg);
    cell.paint_round_rect(3, 3, w - 6, h - 10, r - 2, PALETTE.key_top);
    cell.draw_label(label, PALETTE.key_text, Math.max(10, Math.floor(h * 0.32)));
  }

  function paint_key_pressed(cell, label) {
    const w = cell.w;
    const h = cell.h;
    const r = Math.max(4, Math.floor(Math.min(w, h) * 0.12));
    cell.clear();
    cell.paint_round_rect(0, 2, w, h - 2, r, PALETTE.key_active_border);
    cell.paint_round_rect(2, 4, w - 4, h - 6, r - 1, PALETTE.key_active_bg);
    cell.draw_label(label, PALETTE.key_active_text, Math.max(10, Math.floor(h * 0.32)));
  }

  function paint_face_button(cell, letter, color) {
    const w = cell.w;
    const h = cell.h;
    const cx = w / 2;
    const cy = h / 2;
    const r = Math.min(w, h) / 2 - 2;
    cell.clear();
    /* darker rim */
    cell.paint_circle(cx, cy + 1, r, '#1a1a20');
    cell.paint_circle(cx, cy, r, color);
    /* highlight */
    cell.ctx.globalAlpha = 0.25;
    cell.paint_circle(cx - r * 0.2, cy - r * 0.25, r * 0.45, '#ffffff');
    cell.ctx.globalAlpha = 1;
    cell.draw_label(letter, '#ffffff', Math.max(12, Math.floor(h * 0.4)));
  }

  function paint_shoulder(cell, label) {
    const w = cell.w;
    const h = cell.h;
    cell.clear();
    cell.paint_round_rect(0, 0, w, h, 6, '#1a1a20');
    cell.paint_round_rect(2, 2, w - 4, h - 4, 5, PALETTE.gp_shoulder);
    cell.draw_label(label, PALETTE.key_text, Math.max(10, Math.floor(h * 0.35)));
  }

  function paint_stick(cell, pressed) {
    const w = cell.w;
    const h = cell.h;
    const cx = w / 2;
    const cy = h / 2;
    const base_r = Math.min(w, h) / 2 - 1;
    const knob_r = base_r * (pressed ? 0.55 : 0.62);
    cell.clear();
    cell.paint_circle(cx, cy, base_r, PALETTE.gp_stick_base);
    cell.paint_circle(cx, cy, base_r - 3, '#22222a');
    cell.paint_circle(cx, cy, knob_r, pressed ? '#8a8a9a' : PALETTE.gp_stick_knob);
    cell.ctx.globalAlpha = 0.3;
    cell.paint_circle(cx - knob_r * 0.25, cy - knob_r * 0.3, knob_r * 0.4, '#fff');
    cell.ctx.globalAlpha = 1;
  }

  function paint_dpad_arm(cell, dir) {
    const w = cell.w;
    const h = cell.h;
    cell.clear();
    cell.paint_round_rect(1, 1, w - 2, h - 2, 3, PALETTE.key_border);
    cell.paint_round_rect(3, 3, w - 6, h - 6, 2, PALETTE.key_bg);
    /* simple chevron / arrow */
    const c = cell.ctx;
    c.fillStyle = PALETTE.key_text;
    c.beginPath();
    const cx = w / 2;
    const cy = h / 2;
    const s = Math.min(w, h) * 0.22;
    if (dir === 'up') {
      c.moveTo(cx, cy - s);
      c.lineTo(cx + s, cy + s * 0.6);
      c.lineTo(cx - s, cy + s * 0.6);
    } else if (dir === 'down') {
      c.moveTo(cx, cy + s);
      c.lineTo(cx + s, cy - s * 0.6);
      c.lineTo(cx - s, cy - s * 0.6);
    } else if (dir === 'left') {
      c.moveTo(cx - s, cy);
      c.lineTo(cx + s * 0.6, cy - s);
      c.lineTo(cx + s * 0.6, cy + s);
    } else if (dir === 'right') {
      c.moveTo(cx + s, cy);
      c.lineTo(cx - s * 0.6, cy - s);
      c.lineTo(cx - s * 0.6, cy + s);
    }
    c.closePath();
    c.fill();
  }

  function paint_mouse_body(cell) {
    const w = cell.w;
    const h = cell.h;
    cell.clear();
    cell.paint_round_rect(4, 2, w - 8, h - 4, Math.min(w, h) * 0.35, PALETTE.mouse_body);
    /* center seam */
    cell.ctx.strokeStyle = '#1a1a20';
    cell.ctx.lineWidth = 1;
    cell.ctx.beginPath();
    cell.ctx.moveTo(w / 2, 8);
    cell.ctx.lineTo(w / 2, h * 0.45);
    cell.ctx.stroke();
  }

  function paint_mouse_button(cell, side, pressed) {
    const w = cell.w;
    const h = cell.h;
    const color = pressed ? PALETTE.mouse_btn_press : PALETTE.mouse_btn;
    cell.clear();
    if (side === 'left') {
      cell.paint_round_rect(2, 2, w / 2 - 3, h * 0.55, 8, color);
    } else if (side === 'right') {
      cell.paint_round_rect(w / 2 + 1, 2, w / 2 - 3, h * 0.55, 8, color);
    } else {
      /* middle / whole */
      cell.paint_round_rect(2, 2, w - 4, h * 0.55, 8, color);
    }
  }

  function paint_mouse_wheel(cell, dir) {
    const w = cell.w;
    const h = cell.h;
    const cx = w / 2;
    cell.clear();
    cell.paint_round_rect(cx - 6, 6, 12, h * 0.4, 4, PALETTE.mouse_wheel);
    if (dir === 'up' || dir === 'both') {
      cell.ctx.fillStyle = PALETTE.mouse_scroll;
      cell.ctx.beginPath();
      cell.ctx.moveTo(cx, 4);
      cell.ctx.lineTo(cx + 5, 10);
      cell.ctx.lineTo(cx - 5, 10);
      cell.ctx.fill();
    }
    if (dir === 'down' || dir === 'both') {
      const y = h * 0.4 + 8;
      cell.ctx.fillStyle = PALETTE.mouse_scroll;
      cell.ctx.beginPath();
      cell.ctx.moveTo(cx, y + 6);
      cell.ctx.lineTo(cx + 5, y);
      cell.ctx.lineTo(cx - 5, y);
      cell.ctx.fill();
    }
  }

  function add_painted(ui, id, label, w, h, painter_fn) {
    const cell = new sprite_cell(id, label, w, h);
    cell.clear();
    painter_fn(cell);
    ui.atlas.add_cell(cell);
    return cell;
  }

  function finalize(ui, select_id) {
    ui.atlas.pack();
    ui.selected_id = select_id || (ui.atlas.cells[0] && ui.atlas.cells[0].id);
    if (ui.selected_id) ui.select_cell(ui.selected_id);
    ui.refresh_cell_list();
    ui.refresh_atlas_preview();
  }

  /**
   * WASD + modifiers + space: idle + pressed variants for each key.
   * Pressed sprites sit to the right of idle in the atlas (typical CCT layout).
   */
  function keyboard(ui) {
    if (!confirm_replace(ui, 'WASD keyboard')) return;

    ui.atlas.clear();
    const kw = 64;
    const kh = 64;
    const keys = [
      { id: 'key_q', label: 'Q' },
      { id: 'key_w', label: 'W' },
      { id: 'key_e', label: 'E' },
      { id: 'key_a', label: 'A' },
      { id: 'key_s', label: 'S' },
      { id: 'key_d', label: 'D' },
      { id: 'key_shift', label: '⇧' },
      { id: 'key_ctrl', label: 'Ctrl' },
      { id: 'key_space', label: '␣', w: 128, h: 48 },
      { id: 'key_esc', label: 'Esc' },
      { id: 'key_tab', label: 'Tab' },
      { id: 'key_f', label: 'F' },
      { id: 'key_r', label: 'R' },
      { id: 'key_c', label: 'C' },
      { id: 'key_v', label: 'V' },
      { id: 'key_1', label: '1' },
      { id: 'key_2', label: '2' },
      { id: 'key_3', label: '3' },
      { id: 'key_4', label: '4' },
      { id: 'key_5', label: '5' },
    ];

    keys.forEach((k) => {
      const w = k.w || kw;
      const h = k.h || kh;
      add_painted(ui, k.id, k.label, w, h, (cell) => paint_key_idle(cell, k.label));
      add_painted(ui, k.id + '_pressed', k.label, w, h, (cell) =>
        paint_key_pressed(cell, k.label)
      );
    });

    finalize(ui, 'key_w');
  }

  /**
   * Gamepad face buttons (A/B/X/Y), shoulders, sticks, d-pad arms.
   */
  function gamepad(ui) {
    if (!confirm_replace(ui, 'gamepad')) return;

    ui.atlas.clear();
    const bs = 56; /* face button size */
    const sh_w = 72;
    const sh_h = 32;
    const st = 64; /* stick size */
    const dp = 40; /* dpad arm */

    const faces = [
      { id: 'btn_a', letter: 'A', color: PALETTE.gp_face_a },
      { id: 'btn_b', letter: 'B', color: PALETTE.gp_face_b },
      { id: 'btn_x', letter: 'X', color: PALETTE.gp_face_x },
      { id: 'btn_y', letter: 'Y', color: PALETTE.gp_face_y },
    ];
    faces.forEach((f) => {
      add_painted(ui, f.id, f.letter, bs, bs, (cell) =>
        paint_face_button(cell, f.letter, f.color)
      );
      /* pressed = slightly brighter via second pass with white overlay baked in */
      add_painted(ui, f.id + '_pressed', f.letter, bs, bs, (cell) => {
        paint_face_button(cell, f.letter, f.color);
        cell.ctx.globalAlpha = 0.35;
        cell.paint_circle(bs / 2, bs / 2, bs / 2 - 2, '#ffffff');
        cell.ctx.globalAlpha = 1;
        cell.draw_label(f.letter, '#ffffff', Math.max(12, Math.floor(bs * 0.4)));
      });
    });

    ['lb', 'rb', 'lt', 'rt'].forEach((name) => {
      const label = name.toUpperCase();
      add_painted(ui, 'btn_' + name, label, sh_w, sh_h, (cell) =>
        paint_shoulder(cell, label)
      );
      add_painted(ui, 'btn_' + name + '_pressed', label, sh_w, sh_h, (cell) => {
        paint_shoulder(cell, label);
        cell.ctx.globalAlpha = 0.4;
        cell.paint_round_rect(2, 2, sh_w - 4, sh_h - 4, 5, '#ffffff');
        cell.ctx.globalAlpha = 1;
        cell.draw_label(label, PALETTE.key_text, Math.max(10, Math.floor(sh_h * 0.35)));
      });
    });

    add_painted(ui, 'stick_l', 'L', st, st, (cell) => paint_stick(cell, false));
    add_painted(ui, 'stick_l_pressed', 'L', st, st, (cell) => paint_stick(cell, true));
    add_painted(ui, 'stick_r', 'R', st, st, (cell) => paint_stick(cell, false));
    add_painted(ui, 'stick_r_pressed', 'R', st, st, (cell) => paint_stick(cell, true));

    ['up', 'down', 'left', 'right'].forEach((dir) => {
      add_painted(ui, 'dpad_' + dir, dir, dp, dp, (cell) => paint_dpad_arm(cell, dir));
    });

    add_painted(ui, 'btn_start', '☰', 48, 32, (cell) => paint_shoulder(cell, '☰'));
    add_painted(ui, 'btn_back', '⧉', 48, 32, (cell) => paint_shoulder(cell, '⧉'));

    finalize(ui, 'btn_a');
  }

  /**
   * Mouse body, L/R/M buttons (idle + pressed), scroll wheel.
   */
  function mouse(ui) {
    if (!confirm_replace(ui, 'mouse')) return;

    ui.atlas.clear();
    const mw = 80;
    const mh = 120;

    add_painted(ui, 'mouse_body', 'mouse', mw, mh, (cell) => paint_mouse_body(cell));

    add_painted(ui, 'mouse_left', 'LMB', mw, mh, (cell) => {
      paint_mouse_body(cell);
      paint_mouse_button(cell, 'left', false);
    });
    add_painted(ui, 'mouse_left_pressed', 'LMB', mw, mh, (cell) => {
      paint_mouse_body(cell);
      paint_mouse_button(cell, 'left', true);
    });

    add_painted(ui, 'mouse_right', 'RMB', mw, mh, (cell) => {
      paint_mouse_body(cell);
      paint_mouse_button(cell, 'right', false);
    });
    add_painted(ui, 'mouse_right_pressed', 'RMB', mw, mh, (cell) => {
      paint_mouse_body(cell);
      paint_mouse_button(cell, 'right', true);
    });

    add_painted(ui, 'mouse_middle', 'MMB', mw, mh, (cell) => {
      paint_mouse_body(cell);
      paint_mouse_button(cell, 'middle', false);
    });
    add_painted(ui, 'mouse_middle_pressed', 'MMB', mw, mh, (cell) => {
      paint_mouse_body(cell);
      paint_mouse_button(cell, 'middle', true);
    });

    const ww = 48;
    const wh = 64;
    add_painted(ui, 'scroll_idle', 'whl', ww, wh, (cell) => paint_mouse_wheel(cell, 'both'));
    add_painted(ui, 'scroll_up', '↑', ww, wh, (cell) => paint_mouse_wheel(cell, 'up'));
    add_painted(ui, 'scroll_down', '↓', ww, wh, (cell) => paint_mouse_wheel(cell, 'down'));

    /* small movement indicator dot */
    add_painted(ui, 'mouse_dot', '•', 24, 24, (cell) => {
      cell.clear();
      cell.paint_circle(12, 12, 8, '#ff4444');
      cell.ctx.globalAlpha = 0.5;
      cell.paint_circle(12, 12, 5, '#ffaaaa');
      cell.ctx.globalAlpha = 1;
    });

    finalize(ui, 'mouse_body');
  }

  return {
    keyboard: keyboard,
    gamepad: gamepad,
    mouse: mouse,
    /* expose painters for advanced/custom use */
    _paint: {
      key_idle: paint_key_idle,
      key_pressed: paint_key_pressed,
      face_button: paint_face_button,
      shoulder: paint_shoulder,
      stick: paint_stick,
      dpad_arm: paint_dpad_arm,
      mouse_body: paint_mouse_body,
      mouse_button: paint_mouse_button,
      mouse_wheel: paint_mouse_wheel,
    },
  };
})();
