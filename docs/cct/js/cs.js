var cs = {
    origin: new vec2(100, 100),
    offset: new vec2(0, 0),
    orig_offset: new vec2(0, 0),
    drag_start: new vec2(0, 0),
    dragging: false,
    scale: 1,
    dimensions: r2(0, 0),
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

    click(event) {
        if (event.button === 2 && this.origin.lt(event.clientX, event.clientY)) {
            this.drag_start = new vec2(event.clientX, event.clientY);
            this.orig_offset = this.offset;
            this.dragging = true;
        }
    },

    mouseup(event) { this.dragging = false; },

    scroll(event) {
        let old_x = (event.clientX - cs.origin.x) + this.offset.x / this.scale;
        let old_y = (event.clientY - cs.origin.y) + this.offset.y / this.scale;

        if (event.originalEvent.wheelDelta / 120 > 0) {
            this.scale = Math.min(8, cs.scale + 1);
        } else {
            this.scale = Math.max(1, cs.scale - 1);
        }

        let new_x = (event.clientX - this.origin.x) + this.offset.x / this.scale;
        let new_y = (event.clientY - this.origin.y) + this.offset.y / this.scale;

        console.log("Orig X: " + old_x + ", new x: " + new_x + ", offset x:" + (new_x - old_x));
        this.offset = this.offset.add((new_x - old_x), (new_y - old_y));
        this.offset.max();
    },

    move(event) {
        if (this.dragging) {
            this.offset = this.orig_offset.addv(this.drag_start.sub(event.clientX, event.clientY));
            this.offset.max();
        }
    },

    resize(painter) {
        this.dimensions =
            r2($('#main-canvas').attr("width") - this.origin.x, $('#main-canvas').attr("height") - this.origin.y);
    }
};