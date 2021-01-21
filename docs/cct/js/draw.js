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

var backgroundColor = "#333333";
var lineColor = "#ffffffff";
var darkLineColor = "#555555";
var atlas = null;
var canvas = "#main-canvas";

var painter = {
    scale: 1,

    load_image(url) {
        return new Promise(r => {
            let i = new Image();
            i.onload = (() => r(i));
            i.src = url;
        });
    },

    get_context() { return $(canvas)[0].getContext('2d'); },

    fill(w, h, color) { painter.rect(0, 0, w, h, color); },

    rect(x, y, w, h, color) {
        let context = this.get_context();
        context.beginPath();
        context.rect(x, y, w, h);
        context.fillStyle = color;
        context.fill();
    },

    rect_outline(x, y, w, h, width = 1, color = lineColor) {
        let context = this.get_context();
        context.beginPath();
        context.rect(x, y, w, h);
        context.strokeStyle = color;
        context.lineWidth = width;
        context.stroke();
    },

    text(str, x, y, rotate = 0, align = "right", color = lineColor, size = 13, font = "Arial") {
        let ctx = this.get_context();
        ctx.save();
        ctx.translate(x, y);
        if (rotate > 0)
            ctx.rotate(rotate * Math.PI / 180);
        ctx.textAlign = align;
        ctx.font = size + "px " + font;
        ctx.fillStyle = color;
        ctx.fillText(str, 0, 0);
        ctx.restore();
    },

    line(x1, y1, x2, y2, width = 2, color = lineColor) {
        let ctx = this.get_context();
        ctx.beginPath();
        ctx.moveTo(x1, y1);
        ctx.lineTo(x2, y2);
        ctx.strokeStyle = color;
        ctx.lineWidth = width;
        ctx.stroke();
    },

    image(i, x, y) {
        let context = this.get_context();
        context.drawImage(i, x, y);
    },

    image_crop(i, x, y, w, h, cx, cy, cw, ch) {
        let context = this.get_context();
        context.drawImage(i, cx, cy, cw, ch, x * this.scale, y * this.scale, w * this.scale, h * this.scale);
    },

    resize_canvas() {
        $(canvas).css({"height": window.innerHeight, "width": window.innerWidth});
        $(canvas).attr("width", window.innerWidth);
        $(canvas).attr("height", window.innerHeight);
        painter.fill(window.innerWidth, window.innerHeight, backgroundColor);
        cs.resize(painter);
        painter.get_context().imageSmoothingEnabled = false;
    },

    draw_function() {
        c = $(canvas)[0];
        painter.fill(c.width, c.height, backgroundColor);
        cs.draw(painter);
        if (atlas !== null)
            config.draw(painter, cs);
    },

    update(time) {
        painter.draw_function();
        requestAnimationFrame(painter.update); // get next frame
    },
};

requestAnimationFrame(painter.update); // start animation

$(function() {
    $(window).on('resize', painter.resize_canvas);
    $(canvas).on('mousewheel', e => cs.scroll(e, config));
    $(canvas).on('click', e => cs.click(e));
    $(canvas).on('mousemove', e => {
        cs.move(e);
        config.move(e, cs);
    });
    $(canvas).on('mouseup', e => {
        cs.mouseup(e);
        config.mouseup(e, cs);
    });
    $(canvas).on('mousedown', e => config.mousedown(e, cs));
    $(window).on('keydown', e => config.on_button(e, true));
    $(window).on('keyup', e => config.on_button(e, false));
    $(canvas).on('contextmenu', function(e) {
        cs.click(e);
        e.preventDefault();
        return false;
    });

    painter.resize_canvas(); // Run once to get correct window size
    painter.load_image("https://raw.githubusercontent.com/univrsal/input-overlay/master/docs/cct/res/wasd.png")
        .then(function(img) { atlas = img; });
    painter.get_context().imageSmoothingEnabled = false;

    $.getJSON('https://raw.githubusercontent.com/univrsal/input-overlay/master/docs/cct/res/wasd.json',
              function(data) { config.load_from_json(data); });
});
