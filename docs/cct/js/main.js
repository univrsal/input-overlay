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

var cfg_name = 'overlay.json';
var atlas = null;
var cfg = null;
var edit = null;
var main_painter = null;
var editor_painter = null;
var pad = null;

function convert_old_gamepad_codes() {
  cfg.elements.forEach((e) => {
    if (e.type() === element_types.GAMEPAD_BUTTON) {
      if (legacy2sdl2.has(e.data.code)) {
        console.log('Converting legacy gamepad code');
        e.data.code = legacy2sdl2.get(e.data.code);
      }
    }
  });
}

$(function () {
  main_painter = new painter('main-canvas', (p, c) => cfg.draw(p, c));
  editor_painter = new painter('editor-canvas', (p, c) => edit.draw(p, c));
  pad = new gamepad();
  cfg = new config('#main-canvas', main_painter);
  edit = new editor('#editor-canvas', editor_painter);

  cfg.add_load_callback(() => edit.on_config_load());
  cfg.add_load_callback(convert_old_gamepad_codes);

  main_painter.get_context().imageSmoothingEnabled = false;
  editor_painter.get_context().imageSmoothingEnabled = false;

  new drop_area('config-drop-area').handlers.push((files) => {
    if (files && files.length > 0) {
      let reader = new FileReader();
      reader.readAsDataURL(files[0]);
      reader.onloadend = function () {
        let split = reader.result.split(',');
        if (split.length < 1) return;
        let b64 = split[1];
        let str = atob(b64);
        let j = JSON.parse(str);
        if (j) {
          cfg_name = files[0].name;
          cfg.load_from_json(j);
          $('#default-width').val(j.default_width);
          $('#default-height').val(j.default_height);
          $('#horizontal-offset').val(j.space_h);
          $('#vertical-offset').val(j.space_v);
        }
      };
    }
  });

  new drop_area('texture-drop-area', 'image/*').handlers.push((files) => {
    if (files && files.length > 0) {
      let reader = new FileReader();
      reader.readAsDataURL(files[0]);
      reader.onloadend = function () {
        atlas = new Image();
        atlas.src = reader.result;
      };
    }
  });

  setTimeout(() => {
    main_painter.resize_canvas(); // Run once to get correct window size
    editor_painter.resize_canvas();
  }, 100);

  setTimeout(() => {
    let c = $('#main-canvas-container')[0];
    let e = $('#element-dialog')[0];
    c.style.pointerEvents = 'none';
    c.classList.add('blurred');
    e.style.pointerEvents = 'none';
    e.classList.add('blurred');
    main_painter.enabled = false;
    cfg.enabled = false;
  }, 200);
});
