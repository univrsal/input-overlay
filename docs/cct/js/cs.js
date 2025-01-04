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

class cs {
  constructor(canvas_id) {
    this.origin = new vec2(100, 100);
    this.offset = new vec2();
    this.orig_offset = new vec2();
    this.drag_start = new vec2();
    this.dragging = false;
    this.scale = 1;
    this.dimensions = new r2();
    this.step = 10;
    this.canvas_id = canvas_id;

    $(canvas_id).on('wheel', (e) => this.scroll(e));
    $(canvas_id).on('click', (e) => this.click(e));
    $(canvas_id).on('mousemove', (e) => this.move(e));
    $(canvas_id).on('mouseup', (e) => this.mouseup());
    $(canvas_id).on('mouseleave', () => this.mouseup());
    $(canvas_id).on('contextmenu', (e) => {
      this.click(e);
      e.preventDefault();
      return false;
    });
  }

  draw(painter) {
    // Axis labels
    painter.text(
      'y in pixels',
      this.origin.x - 17,
      this.origin.y - 20,
      90,
      'right',
      lineColor,
      15
    );
    painter.text(
      'x in pixels',
      this.origin.x - 22,
      this.origin.y - 9,
      0,
      'right',
      lineColor,
      15
    );
    // Draw axis
    painter.line(
      0,
      this.origin.y - 0.5,
      this.dimensions.w + this.origin.x,
      this.origin.y - 0.5,
      1
    );
    painter.line(
      this.origin.x - 0.5,
      0,
      this.origin.x - 0.5,
      this.origin.y + this.dimensions.h,
      1
    );

    let ctx = painter.get_context();

    ctx.save();
    ctx.rect(
      this.origin.x,
      0,
      this.dimensions.w,
      this.dimensions.h + this.origin.y
    );
    ctx.clip();

    // Draw scale
    let counter = 0;

    for (
      var x = this.origin.x - this.offset.x;
      x < this.dimensions.w + this.origin.x;
      x += this.step * this.scale
    ) {
      let div = counter % this.step === 0;
      let size = div ? 5.5 : 2.5;

      if (div && counter > 0) {
        painter.text(counter * this.step, x - 5.5, this.origin.y - 9, 90);
        painter.line(
          x - 0.5,
          this.origin.y,
          x - 0.5,
          this.origin.y + this.dimensions.h,
          1,
          darkLineColor
        );
      }

      painter.line(
        x - 0.5,
        this.origin.y - size,
        x - 0.5,
        this.origin.y + size,
        1
      );
      counter++;
    }
    ctx.restore();

    ctx.save();
    ctx.rect(
      0,
      this.origin.y,
      this.dimensions.w + this.origin.x,
      this.dimensions.h
    );
    ctx.clip();

    counter = 0;
    for (
      var y = this.origin.y - this.offset.y;
      y < this.dimensions.h + this.origin.y;
      y += this.step * this.scale
    ) {
      let div = counter % this.step === 0;
      let size = div ? 5.5 : 2.5;

      if (div && counter > 0) {
        painter.text(counter * this.step, this.origin.x - 9, y + 2.5);
        painter.line(
          this.origin.x,
          y - 0.5,
          this.origin.x + this.dimensions.w,
          y - 0.5,
          1,
          darkLineColor
        );
      }

      painter.line(
        this.origin.x - size,
        y - 0.5,
        this.origin.x + size,
        y - 0.5,
        1
      );
      counter++;
    }
    ctx.restore();
  }

  is_mouse_over(event) {
    return this.origin.lt(event.clientX, event.clientY);
  }

  is_mouse_over2(event) {
    return this.origin.lt(event.offsetX, event.offsetY);
  }

  /* screen space to coordinate space */
  translate_point_to_cs(x, y) {
    return new vec2(
      Math.round((x - this.origin.x + this.offset.x) / this.scale),
      Math.round((y - this.origin.y + this.offset.y) / this.scale)
    );
  }

  /* coordinate space to screen space */
  translate_rect_to_screen(r) {
    return new r4(
      r.x * this.scale - this.offset.x + this.origin.x,
      r.y * this.scale - this.offset.y + this.origin.y,
      r.w * this.scale,
      r.h * this.scale
    );
  }

  translate_point_to_screen(x, y) {
    return new vec2(
      x * this.scale - this.offset.x + this.origin.x,
      y * this.scale - this.offset.y + this.origin.y
    );
  }

  click(event) {
    if (event.button === 2 && this.is_mouse_over(event)) {
      this.drag_start = new vec2(event.clientX, event.clientY);
      this.orig_offset = this.offset;
      this.dragging = true;
    }
  }

  mouseup() {
    this.dragging = false;
  }

  scroll(event) {
    let old_mouse = this.translate_point_to_cs(event.clientX, event.clientY);
    let old_scale = this.scale;

    if (event.originalEvent.deltaY < 0) {
      this.scale = Math.min(8, this.scale + 1);
    } else {
      this.scale = Math.max(1, this.scale - 1);
    }

    // Adjust to mouse position
    let new_mouse = this.translate_point_to_cs(event.clientX, event.clientY);
    let offset_factor = this.scale / old_scale;
    this.offset = this.offset.scale(this.scale / old_scale);
    //this.offset = this.offset.sub((new_mouse.x - old_mouse.x) * offset_factor, (new_mouse.x - old_mouse.y) * offset_factor);

    this.offset.limit(0, 0, 3000 * this.scale, 3000 * this.scale);
  }

  move(event) {
    if (this.dragging) {
      this.offset = this.orig_offset.addv(
        this.drag_start.sub(event.clientX, event.clientY)
      );
      this.offset.limit(0, 0, 3000 * this.scale, 3000 * this.scale);
    }
  }

  resize(painter) {
    this.dimensions = new r2(
      $(this.canvas_id).attr('width') - this.origin.x,
      $(this.canvas_id).attr('height') - this.origin.y
    );
  }
}
