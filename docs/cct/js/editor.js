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

var leniency = 10;

class editor {
  constructor(canvas_id, painter) {
    this.canvas_id = canvas_id;
    this.painter = painter;
    this.move_flags = 0x0;
    this.drag_offset = new vec2();
    this.drag_start = new vec2();
    this.selecting = false;
    this.mouse_pos = new vec2();
    this.mouse_over = false;
    this.shift_pressed = false;
    this.selection_rect = new r4();

    $(canvas_id).on('mousemove', (e) => this.move(e, this.painter.cs()));
    $(canvas_id).on('mouseup', (e) => this.mouseup(e, this.painter.cs()));
    $(canvas_id).on('mousedown', (e) => this.mousedown(e, this.painter.cs()));
    $(window).on('keydown', (e) => this.on_button(e, true));
    $(window).on('keyup', (e) => this.on_button(e, false));
    $('#editor-element-u').on('keyup', (_e) =>
      this.update_selection_from_text()
    );
    $('#editor-element-v').on('keyup', (_e) =>
      this.update_selection_from_text()
    );
    $('#editor-element-w').on('keyup', (_e) =>
      this.update_selection_from_text()
    );
    $('#editor-element-h').on('keyup', (_e) =>
      this.update_selection_from_text()
    );

    let hex_numeric = $('.hex-numeric')[0];
    if (hex_numeric) {
      // jquery doesn't forward the location variable
      hex_numeric.addEventListener('keydown', (e) => {
        if (
          $('#editor-element-record-code')[0].checked &&
          current_type === element_types.KEYBOARD_KEY
        ) {
          e.target.value = '0x' + key_to_vc(e).toString(16).toUpperCase();
          e.preventDefault();
          return false;
        }
        let text = e.target.value + e.key;
        // Only allow hex numbers and 0x prefix and arrow keys
        if (
          e.key.length > 1 &&
          !/^Arrow/.test(e.key) &&
          !/^0x[0-9a-fA-F]+$/g.test(text)
        ) {
          e.target.value = '0x0';
          e.preventDefault();
          return false;
        }
      });
      hex_numeric.addEventListener('mouseup', (e) => {
        if (
          $('#editor-element-record-code')[0].checked &&
          current_type === element_types.MOUSE_BUTTON
        ) {
          // Scroll wheel pastes text on linux so we delay the action a bit;
          let val = '0x' + mouse_to_vc(e).toString(16).toUpperCase();
          setTimeout(() => {
            e.target.value = val;
          }, 50);
        }
      });
    }

    pad.on('button', (pad, index, btn) =>
      this.on_gamepad_button(pad, index, btn)
    );
  }

  on_gamepad_button(_pad, index, btn) {
    if (
      $('#editor-element-record-code').val() === 'on' &&
      btn.value > 0 &&
      current_type === element_types.GAMEPAD_BUTTON
    ) {
      let vc = '0x' + gamepad_to_vc(index).toString(16).toUpperCase();
      $('.hex-numeric').val(vc);
      console.log($('.hex-numeric').val());
    }
  }

  on_config_load() {
    if (
      cfg.data.default_height !== undefined &&
      cfg.data.default_width !== undefined
    ) {
      this.selection_rect = new r4(
        1,
        1,
        cfg.data.default_width,
        cfg.data.default_height
      );
    } else {
      this.selection_rect = new r4(1, 1, 100, 100);
    }
    this.update_selection_values();
  }

  on_button(event, down) {
    this.shift_pressed = event.shiftKey;
  }

  set_cursor(c) {
    $(this.canvas_id).css({ cursor: c });
  }

  update_selection_from_text() {
    this.selection_rect.x = parseInt($('#editor-element-u').val());
    this.selection_rect.y = parseInt($('#editor-element-v').val());
    this.selection_rect.w = parseInt($('#editor-element-w').val());
    this.selection_rect.h = parseInt($('#editor-element-h').val());
  }

  move(event, cs) {
    let tv = cs.translate_point_to_cs(event.offsetX, event.offsetY);
    this.mouse_pos.x = event.offsetX;
    this.mouse_pos.y = event.offsetY;

    if (this.selecting) {
      let new_selection = this.selection_rect.copy();

      if (this.move_flags === 0) {
        // Rubber band selection
        new_selection.from_points(this.drag_start, tv);
      } else {
        if (this.move_flags & 0b0001) new_selection.h = tv.y - new_selection.y;
        if (this.move_flags & 0b0010) {
          new_selection.h += new_selection.y - tv.y;
          new_selection.y = tv.y;
        }

        if (this.move_flags & 0b0100) new_selection.w = tv.x - new_selection.x;
        if (this.move_flags & 0b1000) {
          new_selection.w += new_selection.x - tv.x;
          new_selection.x = tv.x;
        }

        if (this.move_flags & 0b10000) {
          new_selection.x = tv.x - this.drag_offset.x;
          new_selection.y = tv.y - this.drag_offset.y;
        }

        // prevent the selection from getting to small
        if (new_selection.w < leniency + 3 || new_selection.x < 0) {
          new_selection.w = this.selection_rect.w;
          new_selection.x = this.selection_rect.x;
        }
        if (new_selection.h < leniency + 3 || new_selection.y < 0) {
          new_selection.h = this.selection_rect.h;
          new_selection.y = this.selection_rect.y;
        }
      }

      this.update_selection_values();
      this.selection_rect = new_selection;
      return;
    }

    let flags = 0b0000; // left, right, up, down
    let l = leniency / (cs.scale / 2);
    let cursor_box = this.selection_rect.grow(l);
    this.mouse_over = cs.is_mouse_over2(event);

    if (this.mouse_over) {
      if (cursor_box.is_point_inside(tv)) {
        if (Math.abs(tv.x - this.selection_rect.x) < l) flags |= 0b1000;
        else if (
          Math.abs(tv.x - (this.selection_rect.w + this.selection_rect.x)) < l
        )
          flags |= 0b0100;
        if (Math.abs(tv.y - this.selection_rect.y) < l) flags |= 0b0010;
        else if (
          Math.abs(tv.y - (this.selection_rect.h + this.selection_rect.y)) < l
        )
          flags |= 0b0001;
      }

      if (flags === 0 && this.selection_rect.is_point_inside(tv))
        flags = 0b10000; // move cursor
    }

    if (flags & 0b1000) {
      if (flags & 0b0010) {
        // top left
        this.set_cursor('nw-resize');
      } else if (flags & 0b0001) {
        // bottom left
        this.set_cursor('sw-resize');
      } else {
        // left
        this.set_cursor('w-resize');
      }
    } else if (flags & 0b0100) {
      if (flags & 0b0010) {
        // top right
        this.set_cursor('ne-resize');
      } else if (flags & 0b0001) {
        // bottom right
        this.set_cursor('se-resize');
      } else {
        // right
        this.set_cursor('e-resize');
      }
    } else if (flags & 0b0001) {
      this.set_cursor('s-resize');
    } else if (flags & 0b0010) {
      this.set_cursor('n-resize');
    } else if (flags & 0b10000) {
      this.set_cursor('move');
    } else {
      if (cs.dragging) {
        this.set_cursor('grabbing');
      } else {
        if (this.mouse_over) this.set_cursor('crosshair');
        else this.set_cursor('auto');
      }
    }
    this.move_flags = flags;
  }

  mouseup(_event, _cs) {
    this.selecting = false;
  }

  update_selection_values() {
    $('#editor-element-u').val(this.selection_rect.x);
    $('#editor-element-v').val(this.selection_rect.y);
    $('#editor-element-w').val(this.selection_rect.w);
    $('#editor-element-h').val(this.selection_rect.h);
  }

  mousedown(event, cs) {
    if (event.button == 0 && this.mouse_over) {
      let tv = cs.translate_point_to_cs(event.offsetX, event.offsetY);
      this.drag_offset.x = tv.x - this.selection_rect.x;
      this.drag_offset.y = tv.y - this.selection_rect.y;
      this.drag_start = tv;
      this.selecting = true;
    }
  }

  draw(painter) {
    if (atlas === null)
      // Don't draw if image hasn't loaded yet
      return;
    let ctx = painter.get_context();
    let cs = painter.cs();

    ctx.save();
    ctx.rect(cs.origin.x, cs.origin.y, cs.dimensions.w, cs.dimensions.h);
    ctx.clip();
    painter.image_crop(
      atlas,
      cs.origin.x - cs.offset.x,
      cs.origin.y - cs.offset.y,
      atlas.width * cs.scale,
      atlas.height * cs.scale,
      0,
      0,
      atlas.width,
      atlas.height
    );

    // Draw selection
    let r = cs.translate_rect_to_screen(this.selection_rect);
    painter.rect_outline(
      r.x + 0.5,
      r.y + 0.5,
      r.w - 1,
      r.h - 1,
      1,
      '#ff0000ff'
    );

    if (this.mouse_over && !this.shift_pressed) {
      if (this.move_flags === 0) {
        // draw help lines at mouse pos
        painter.dashed_line(
          this.mouse_pos.x - 0.5,
          cs.origin.y,
          this.mouse_pos.x - 0.5,
          cs.origin.y + cs.dimensions.h,
          1
        );
        painter.dashed_line(
          cs.origin.x,
          this.mouse_pos.y - 0.5,
          cs.origin.x + cs.dimensions.w,
          this.mouse_pos.y - 0.5,
          1
        );
      } else {
        // draw help lines around selection
        painter.dashed_line(
          r.x - 0.5,
          cs.origin.y,
          r.x - 0.5,
          cs.origin.y + cs.dimensions.h,
          1
        );
        painter.dashed_line(
          r.x + r.w - 0.5,
          cs.origin.y,
          r.x + r.w - 0.5,
          cs.origin.y + cs.dimensions.h,
          1
        );
        painter.dashed_line(
          cs.origin.x,
          r.y - 0.5,
          cs.origin.x + cs.dimensions.w,
          r.y - 0.5,
          1
        );
        painter.dashed_line(
          cs.origin.x,
          r.y + r.h - 0.5,
          cs.origin.x + cs.dimensions.w,
          r.y + r.h - 0.5,
          1
        );
      }
    }
    ctx.restore();
  }
}
