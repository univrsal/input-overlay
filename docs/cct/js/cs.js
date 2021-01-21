/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2021 univrsal <uni@vrsal.de>.
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

var cs = {
    origin: new vec2(100, 100),
    offset: new vec2(),
    orig_offset: new vec2(),
    drag_start: new vec2(),
    dragging: false,
    scale: 1,
    dimensions: new r2(),
    step: 10,

    draw(painter) {
        // Axis labels
        painter.text("y in pixels", this.origin.x - 17, this.origin.y - 20, 90, "right", lineColor, 15);
        painter.text("x in pixels", this.origin.x - 22, this.origin.y - 9, 0, "right", lineColor, 15);
        // Draw axis
        painter.line(0, this.origin.y - 0.5, this.dimensions.w + this.origin.x, this.origin.y - 0.5, 1);
        painter.line(this.origin.x - 0.5, 0, this.origin.x - 0.5, this.origin.y + this.dimensions.h, 1);

        let ctx = painter.get_context();

        ctx.save();
        ctx.rect(this.origin.x, 0, this.dimensions.w, this.dimensions.h + this.origin.y);
        ctx.clip();

        // Draw scale
        let counter = 0;

        for (var x = this.origin.x - this.offset.x; x < this.dimensions.w + this.origin.x;
             x += this.step * this.scale) {
            let div = counter % this.step === 0;
            let size = div ? 5.5 : 2.5;

            if (div && counter > 0) {
                painter.text(counter * this.step, x - 5.5, this.origin.y - 9, 90);
                painter.line(x - 0.5, this.origin.y, x - 0.5, this.origin.y + this.dimensions.h, 1, darkLineColor);
            }

            painter.line(x - 0.5, this.origin.y - size, x - 0.5, this.origin.y + size, 1);
            counter++;
        }
        ctx.restore();

        ctx.save();
        ctx.rect(0, this.origin.y, this.dimensions.w + this.origin.x, this.dimensions.h);
        ctx.clip();

        counter = 0;
        for (var y = this.origin.y - this.offset.y; y < this.dimensions.h + this.origin.y;
             y += this.step * this.scale) {
            let div = counter % this.step === 0;
            let size = div ? 5.5 : 2.5;

            if (div && counter > 0) {
                painter.text(counter * this.step, this.origin.x - 9, y + 2.5);
                painter.line(this.origin.x, y - 0.5, this.origin.x + this.dimensions.w, y - 0.5, 1, darkLineColor);
            }

            painter.line(this.origin.x - size, y - 0.5, this.origin.x + size, y - 0.5, 1);
            counter++;
        }
        ctx.restore();
    },

    is_mouse_over(event) { return this.origin.lt(event.clientX, event.clientY); },

    /* screen space to coordinate space */
    translate_point_to_cs(x, y) {
        return new vec2(Math.round((x - this.origin.x + this.offset.x) / this.scale),
                        Math.round((y - this.origin.y + this.offset.y) / this.scale));
    },

    /* coordinate space to screen space */
    translate_rect_to_screen(r) {
        return new r4(r.x * this.scale - this.offset.x + this.origin.x,
                      r.y * this.scale - this.offset.y + this.origin.y, r.w * this.scale, r.h * this.scale);
    },

    translate_point_to_screen(x, y) {
        return new vec2(x * this.scale - this.offset.x + this.origin.x, y * this.scale - this.offset.y + this.origin.y);
    },

    click(event) {
        if (event.button === 2 && this.is_mouse_over(event)) {
            this.drag_start = new vec2(event.clientX, event.clientY);
            this.orig_offset = this.offset;
            this.dragging = true;
        }
    },

    mouseup(event) { this.dragging = false; },

    scroll(event, config) {
        if (config.dragging || config.selecting)
            return;

        let old_mouse = this.translate_point_to_cs(event.clientX, event.clientY);
        let old_scale = this.scale;

        if (event.originalEvent.wheelDelta / 120 > 0) {
            this.scale = Math.min(8, cs.scale + 1);
        } else {
            this.scale = Math.max(1, cs.scale - 1);
        }

        // Adjust to mouse position
        let new_mouse = this.translate_point_to_cs(event.clientX, event.clientY);
        console.log("old: " + old_mouse.x + "/" + old_mouse.y + ", " + new_mouse.x + "/" + new_mouse.y + ", " +
                    (new_mouse.x - old_mouse.x) + "/" + (new_mouse.y - old_mouse.y));
        let offset_factor = cs.scale / old_scale;
        this.offset = this.offset.scale(cs.scale / old_scale);
        //this.offset = this.offset.sub((new_mouse.x - old_mouse.x) * offset_factor, (new_mouse.x - old_mouse.y) * offset_factor);

        this.offset.limit();
    },

    move(event) {
        if (this.dragging) {
            this.offset = this.orig_offset.addv(this.drag_start.sub(event.clientX, event.clientY));
            this.offset.limit();
        }
    },

    resize(painter) {
        this.dimensions =
            new r2($('#main-canvas').attr("width") - this.origin.x, $('#main-canvas').attr("height") - this.origin.y);
    }
};