/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 Alex <uni@vrsal.xyz>.
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

var cfg_name = "overlay.json";
var atlas = null;
var cfg = null;
var main_painter = null;
var pad = null;

$(function() {
    main_painter = new painter("canvas", (p, c) => cfg.draw(p, c));
    pad = new gamepad();
    cfg = new config("#canvas", main_painter);
    main_painter.load_image("./texture.png").then(function(img) { atlas = img; });
    cfg.load_from_json(JSON.parse(preset));

    main_painter.get_context().imageSmoothingEnabled = false;
    setTimeout(() => {
        main_painter.resize_canvas(); // Run once to get correct window size
    }, 100);
});
