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

class config {
  constructor(canvas_id, painter) {
    this.data = {};
    this.elements = [];
    this.selected_elements = [];
    this.selecting = false;
    this.dragging = false;
    this.allow_arrow_movement = true;
    this.drag_selection = new r4(); // Mouse dragged selection in screen space
    this.selection_rect = new r4(); // Actual selected element(s) in coordinate space
    this.drag_offset = new vec2(); // MousePos - SelectionRect (unscaled)
    this.painter = painter;
    this.is_ctrl_down = false;
    this.internal_clipboard = null;
    this.grid = new vec2();
    this.enabled = true; // false when a dialog is open
    $(canvas_id).on('mousemove', (e) => this.move(e, this.painter.cs()));
    $(canvas_id).on('mouseup', (e) => this.mouseup(e, this.painter.cs()));
    $(canvas_id).on('mousedown', (e) => this.mousedown(e, this.painter.cs()));
    $(canvas_id).on('wheel', (e) => this.on_scroll(e));
    $(window).on('copy', () => this.on_copy());
    $(window).on('paste', () => this.on_paste());
    $(window).on('keydown', (e) => this.on_button(e, true));
    $(window).on('keyup', (e) => this.on_button(e, false));
    pad.on('input', (p) => this.gamepad_input(p));
    this.load_callbacks = [];
    setInterval(() => this.tick(), 250);
  }

  tick() {
    this.elements.forEach((e) => e.tick());
  }

  on_scroll(e) {
    this.elements.forEach((element) => element.on_scroll_input(e));
  }

  on_copy() {
    if (!this.enabled) return;
    let data = [];
    this.selected_elements.forEach((e) => data.push(e.data));
    this.internal_clipboard = data;
    navigator.clipboard.writeText(JSON.stringify(data));
  }

  add_elements(j) {
    if (j) {
      if (j.length > 0) this.selected_elements = [];
      j.forEach((e) => {
        let orig_id = e.id.replace(/\d+$/, '');
        let counter = 0;
        let id = orig_id + counter;
        e.pos[1] += 4; // offset so you can see the pasted elements
        while (!this.is_name_unique(id)) id = orig_id + counter++;
        e.id = id;
        let new_element = create_element(e);
        this.elements.push(new_element);
        this.selected_elements.push(new_element);
      });
    }
  }

  on_paste() {
    if (!this.enabled) return;
    let cfg = this;
    navigator.clipboard
      .readText()
      .then((text) => {
        cfg.add_elements(JSON.parse(text));
      })
      .catch((err) => {
        console.log('Failed to read clipboard text: ' + err);
        // fallback to internal clip board
        this.add_elements(this.internal_clipboard);
      });
  }

  add_load_callback(cb) {
    this.load_callbacks.push(cb);
  }

  load_from_json(json) {
    this.data = json;
    this.data['elements'].forEach((data) => {
      let new_element = create_element(data);
      if (new_element !== null) this.elements.push(new_element);
    });
    this.grid.x = this.data.default_width + this.data.space_h;
    this.grid.y = this.data.default_height + this.data.space_v;
    this.sort_elements();
    this.load_callbacks.forEach((cb) => cb());
  }

  write_to_json() {
    this.data.flags = 0;
    this.data.overlay_width = 0;
    this.data.overlay_height = 0;
    this.data['elements'] = this.elements.map((e) => e.data);
    this.elements.forEach((e) => {
      switch (e.type()) {
        case element_types.ANALOG_STICK:
          if (e.is_left_stick()) this.data.flags |= overlay_flags.LEFT_STICK;
          else this.data.flags |= overlay_flags.RIGHT_STICK;
        case element_types.GAMEPAD_BUTTON:
        case element_types.ANALOG_STICK:
        case element_types.TRIGGER:
        case element_types.DPAD_STICK:
        case element_types.GAMEPAD_ID:
          this.data.flags |= overlay_flags.GAMEPAD;
          break;
        case element_types.WHEEL:
        case element_types.MOUSE_MOVEMENT:
        case element_types.MOUSE_BUTTON:
          this.data.flags |= overlay_flags.MOUSE;
          break;
      }
      this.data.overlay_width = Math.max(
        this.data.overlay_width,
        e.x() + e.w()
      );
      this.data.overlay_height = Math.max(
        this.data.overlay_height,
        e.y() + e.h()
      );
    });
    return this.data;
  }

  sort_elements() {
    this.elements.sort((a, b) => {
      return a.layer() - b.layer();
    });
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

    if (this.grid.x > 0) {
      for (
        var x = cs.origin.x - cs.offset.x;
        x < cs.dimensions.w + cs.origin.x;
        x += this.grid.x * cs.scale
      ) {
        painter.line(
          x - 0.5,
          cs.origin.y,
          x - 0.5,
          cs.origin.y + cs.dimensions.h,
          1,
          brightLineColor
        );
      }
    }
    if (this.grid.y > 0) {
      for (
        var y = cs.origin.y - cs.offset.y;
        y < cs.dimensions.h + cs.origin.y;
        y += this.grid.y * cs.scale
      ) {
        painter.line(
          cs.origin.x,
          y - 0.5,
          cs.origin.x + cs.dimensions.w,
          y - 0.5,
          1,
          brightLineColor
        );
      }
    }

    this.elements.forEach((element) => element.draw(painter));
    if (this.selecting && !this.drag_selection.is_empty()) {
      painter.rect_outline(
        this.drag_selection.x - 0.5,
        this.drag_selection.y - 0.5,
        this.drag_selection.w,
        this.drag_selection.h
      );
    }

    if (!this.selection_rect.is_empty()) {
      let r = cs.translate_rect_to_screen(this.selection_rect);
      painter.rect_outline(
        r.x + 0.5,
        r.y + 0.5,
        r.w - 1,
        r.h - 1,
        1,
        '#ff0000ff'
      );
    }
    ctx.restore();
  }

  delete_selection() {
    let new_elements = this.elements.filter(function (to_filter) {
      return (
        this.selected_elements.find(function (to_find) {
          return to_filter.id() === to_find.id();
        }) === undefined
      );
    }, this);
    this.elements = new_elements;
    this.deselect();
  }

  is_name_unique(name, editing_element) {
    let unique = true;
    this.elements.some((e) => {
      if (e.id() === name) {
        if (!editing_element || e !== editing_element) {
          unique = false;
          return true;
        }
      }
      return false;
    });
    return unique;
  }

  gamepad_input(pad) {
    this.elements.forEach((e) => e.on_gamepad_input(pad));
  }

  on_button(event, state) {
    if (!this.enabled) return;
    if (event.key == 'Control') this.is_ctrl_down = state;

    let vc = key_to_vc(event);
    this.elements.forEach((element) => element.on_keyboard_input(vc, state));
    if (this.selected_elements.length > 0 && state) {
      let old_pos = new vec2(this.selection_rect.x, this.selection_rect.y);
      let moved = false;

      switch (event.key) {
        case 'ArrowUp':
          moved = true;
          this.selection_rect.y--;
          break;
        case 'ArrowDown':
          moved = true;
          this.selection_rect.y++;
          break;
        case 'ArrowLeft':
          moved = true;
          this.selection_rect.x--;
          break;
        case 'ArrowRight':
          moved = true;
          this.selection_rect.x++;
          break;
        case 'Delete':
          if (document.activeElement === document.body) {
            if (this.selected_elements.length > 1) {
              // Ask for comfirmation when deleting more than one element
              if (
                confirm(
                  'You are about to delete ' +
                    this.selected_elements.length +
                    ' elements. Are you sure?'
                )
              ) {
                this.delete_selection();
              }
            } else {
              this.delete_selection();
            }
          }
          break;
      }

      if (moved) {
        this.selection_rect.max();

        let diff = old_pos.sub(this.selection_rect.x, this.selection_rect.y);
        this.selected_elements.forEach((e) => {
          let d = e.dim();
          e.set_pos(d.x - diff.x, d.y - diff.y);
        });
        if (this.selected_elements.length < 2)
          this.select_element(this.selected_elements[0]);
      }
    }
  }

  mouseup(event, cs) {
    this.selecting = false;
    this.dragging = false;
    let vc = mouse_to_vc(event);
    this.elements.forEach((element) => element.on_mouse_input(vc, false));
  }

  start_dragging(event, cs) {
    let tv = cs.translate_point_to_cs(event.clientX, event.clientY);
    this.drag_offset.x = tv.x - this.selection_rect.x;
    this.drag_offset.y = tv.y - this.selection_rect.y;
    this.dragging = true;
    this.selecting = false;
  }

  set_selection(element) {
    this.selection_rect = element.dim();
  }

  mousedown(event, cs) {
    let vc = mouse_to_vc(event);
    this.elements.forEach((element) => element.on_mouse_input(vc, true));

    if (event.button == 0 && cs.is_mouse_over(event)) {
      let r = cs.translate_rect_to_screen(this.selection_rect);
      let m = new vec2(event.clientX, event.clientY);
      if (r.is_point_inside(m)) {
        this.start_dragging(event, cs);
      } else {
        // Element selection
        if (!this.is_ctrl_down) {
          this.selected_elements = [];
          this.selection_rect = new r4();

          this.deselect();
        }

        // Array is sorted lowest to highest, so the highest layer is drawn
        // last, but for clicking we want the highest layer first
        this.elements
          .slice()
          .reverse()
          .some((e) => {
            if (e.scaled_dim(cs).is_point_inside(m)) {
              this.selected_elements.push(e);
              this.selection_rect.union(e.dim());
              this.select_element(e);
              return true;
            }
            return false;
          });

        // No element was directly selected -> allow drag selection
        if (this.selection_rect.is_empty()) {
          this.selecting = true;
          this.dragging = false;
          this.drag_selection.x = event.clientX;
          this.drag_selection.y = event.clientY;
          this.drag_selection.w = 0;
          this.drag_selection.h = 0;
        } else {
          this.start_dragging(event, cs);
        }
      }
    }
  }

  deselect() {
    $('#selected-element-x').val(0);
    $('#selected-element-y').val(0);
    $('#selected-element-w').val(0);
    $('#selected-element-h').val(0);
    $('#selected-element-u').val(0);
    $('#selected-element-v').val(0);
    $('#selected-element-id').val('');
    $('#selected-element-layer').val(0);
    this.selected_elements = [];
    this.selection_rect.reset();
  }

  select_element(e) {
    $('#selected-element-x').val(e.x());
    $('#selected-element-y').val(e.y());
    $('#selected-element-w').val(e.w());
    $('#selected-element-h').val(e.h());
    $('#selected-element-u').val(e.u());
    $('#selected-element-v').val(e.v());
    $('#selected-element-id').val(e.id());
    $('#selected-element-layer').val(e.layer());
  }

  move(event, cs) {
    if (this.selecting) {
      this.drag_selection.w = event.clientX - this.drag_selection.x;
      this.drag_selection.h = event.clientY - this.drag_selection.y;

      let selected_elements = [];
      let rect = new r4();
      let first = true;
      this.elements.forEach((e) => {
        if (e.scaled_dim(cs).is_inside(this.drag_selection)) {
          selected_elements.push(e);
          if (first) {
            rect = e.dim();
            first = false;
          } else {
            rect.union(e.dim());
          }
        }
      });

      this.selected_elements = selected_elements;
      this.selection_rect = rect;
    } else if (this.dragging) {
      let tv = cs.translate_point_to_cs(event.clientX, event.clientY);
      let old_pos = new vec2(this.selection_rect.x, this.selection_rect.y);
      this.selection_rect.x = tv.x - this.drag_offset.x;
      this.selection_rect.y = tv.y - this.drag_offset.y;
      this.selection_rect.max();

      let diff = old_pos.sub(this.selection_rect.x, this.selection_rect.y);
      this.selected_elements.forEach((e) => {
        let d = e.dim();
        e.set_pos(d.x - diff.x, d.y - diff.y);
      });
      if (this.selected_elements.length < 2)
        this.select_element(this.selected_elements[0]);
    }
  }
}
