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

var cfg_name = 'overlay.json'
var atlas = null;
var cfg = null;
var edit = null;
var main_painter = null;
var editor_painter = null;
var pad = null;

$(function() {
    main_painter = new painter("main-canvas", (p, c) => cfg.draw(p, c));
    editor_painter = new painter("editor-canvas", (p, c) => edit.draw(p, c));
    pad = new gamepad();
    cfg = new config("#main-canvas", main_painter);
    edit = new editor("#editor-canvas", editor_painter);

    cfg.add_load_callback(() => edit.on_config_load());

    main_painter.resize_canvas(); // Run once to get correct window size
    editor_painter.resize_canvas();

    main_painter.load_image("https://raw.githubusercontent.com/univrsal/input-overlay/master/docs/cct/res/wasd.png")
        .then(function(img) { atlas = img; });
    main_painter.get_context().imageSmoothingEnabled = false;
    editor_painter.get_context().imageSmoothingEnabled = false;
    $.getJSON('https://raw.githubusercontent.com/univrsal/input-overlay/master/docs/cct/res/wasd.json',
              function(data) { cfg.load_from_json(data); });
});