/*
   This file is part of input-overlay
   git.vrsal.xyz/alex/input-overlay
   Copyright 2022 univrsal <uni@vrsal.xyz>.
  
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 2 of the License.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
var converted = {};
var cfg_name = "";

$(() => {
    new drop_area("config-drop-area", "text/ini").handlers.push((files) => {
        if (files && files.length > 0) {
            let reader = new FileReader();
            reader.readAsDataURL(files[0]);
            reader.onloadend = function() {
                cfg_name = files[0].name + '.json';
                let split = reader.result.split(',');
                if (split.length < 1)
                    return;
                let b64 = split[1];
                let str = atob(b64);
                let cfg = parse_legacy_config(str);
                converted = convert_legacy_cfg(cfg);
            };
        }
    });

    $('#download').on('click', () => {download_json(cfg_name, converted)});
});

function parse_legacy_config(str)
{
    str = str.replaceAll('\r', '');
    lines = str.split('\n');
    lines = $(lines).filter((e, i) => i.length > 0 && !i.trim().startsWith('#'));

    cfg = {};
    $(lines).each((e, i) => {
        switch (i[0]) {
        case '0':
        case '1':
        case '2':
        case '3':
            i = i.substr(2);
        }
        splits = i.split('=');
        key = splits[0];
        val = splits[1];
        cfg[key] = val;
    });

    console.log(cfg);
    return cfg;
}

function convert_legacy_cfg(cfg)
{
    if (cfg.layout_type !== '2') {
        alert('Converter only works with keyboard layouts');
        return;
    }

    let csv_to_num =
        (obj) => {
            let result = [];
            obj.split(',').forEach((i, e) => result.push(Number(i)));
            return result;
        }

    let kc = Number(cfg.key_count);
    let kw = Number(cfg.key_abs_w);
    let kh = Number(cfg.key_abs_h);
    let num_rows = Number(cfg.key_rows);
    let num_cols = Number(cfg.key_cols);
    let h = csv_to_num(cfg.key_height);
    let w = csv_to_num(cfg.key_width);
    let cols = csv_to_num(cfg.key_col);
    let rows = csv_to_num(cfg.key_row);
    let codes = csv_to_num(cfg.key_order);
    let vspace = Number(cfg.key_space_v);
    let hspace = Number(cfg.key_space_h);
    let textures_per_row = Number(cfg.texture_w);
    let texture_v_space = Number(cfg.texture_v_space);
    let u = 0;
    let v = 0;
    let layout = {};
    let max_x = 100;
    let max_y = 100;
    let texture_row = 0;
    layout.elements = [];
    layout.default_height = kh;
    layout.default_width = kw;
    layout.flags = 0;
    layout.space_v = vspace;
    layout.space_h = hspace;

    for (let i = 0; i < kc; i++) {
        let key = {};
        key.id = `key_${i}`;
        key.type = 1;
        key.z_level = 1;
        key.pos = [cols[i] * kw + hspace * cols[i], rows[i] * kh + vspace * rows[i]];
        key.code = codes[i];
        key.mapping = [1 + u, v + 1, layout.default_width * w[i], layout.default_height * h[i]];

        if (key.mapping[0] + key.mapping[2] > max_x)
            max_x = key.mapping[0] + key.mapping[2];

        if (key.mapping[1] + key.mapping[3] > max_y)
            max_y = key.mapping[1] + key.mapping[3];

        if ((i + 1) % textures_per_row === 0) {
            u = 0;
            texture_row++;
            v += texture_v_space + 3 * texture_row;
        }
        u += layout.default_width * w[i] + 3;
        layout.elements.push(key);
    }
    layout.overlay_height = max_y;
    layout.overlay_width = max_x;
    return layout;
}
