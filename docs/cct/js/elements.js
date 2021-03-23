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

let element_types = {
    TEXTURE: 0,
    KEYBOARD_KEY: 1,
    GAMEPAD_BUTTON: 2,
    MOUSE_BUTTON: 3,
    WHEEL: 4,
    ANALOG_STICK: 5,
    TRIGGER: 6,
    /* Shows game pad number 1 through 4 */
    GAMEPAD_ID: 7,
    DPAD_STICK: 8,
    MOUSE_MOVEMENT: 9
};

let constants = { texture_space: 3 };

let element_map = new Map();

class element {
    constructor(json) { this.data = json; }

    draw(painter) { }
    on_button_input(vc, state) { }

    scaled_dim(cs) {
        return new r4(cs.origin.x - cs.offset.x + this.data.pos[0] * cs.scale,
            cs.origin.y - cs.offset.y + this.data.pos[1] * cs.scale, this.data.mapping[2] * cs.scale,
            this.data.mapping[3] * cs.scale);
    }
    
    read_data_from_gui()
    {
        this.data.id = $("#editor-element-id").val();
        this.data.z_level = parseInt($("#editor-element-layer").val());
    }

    write_data_to_gui()
    {
        $("#editor-element-id").val(this.data.id);
        $("#editor-element-layer").val(this.data.z_level);
    }

    dim() { return new r4(this.data.pos[0], this.data.pos[1], this.data.mapping[2], this.data.mapping[3]); }

    set_dim(pos, uvwh) {
        this.data.pos[0] = pos.x;
        this.data.pos[1] = pos.y;
        this.data.mapping[0] = uvwh.x;
        this.data.mapping[1] = uvwh.y;
        this.data.mapping[2] = uvwh.w;
        this.data.mapping[3] = uvwh.h;
    }

    type() { return 'element'; }
    id() { return this.data.id; }
    x() { return this.data.pos[0]; }
    y() { return this.data.pos[1]; }
    u() { return this.data.mapping[0]; }
    v() { return this.data.mapping[1]; }
    w() { return this.data.mapping[2]; }
    h() { return this.data.mapping[3]; }
    set_id(id) { this.data.id = id; }

    set_pos(x, y) {
        this.data.pos[0] = x;
        this.data.pos[1] = y;
    }

    layer() { return this.data.z_level; }
    set_layer(layer) { this.data.z_level = layer; }
};

class texture extends element {
    type() { return 'texture'; }
    
    draw(painter) {
        // default draw handling
        let cs = painter.cs();
        painter.image_crop(atlas, cs.origin.x - cs.offset.x + this.data.pos[0] * cs.scale,
            cs.origin.y - cs.offset.y + this.data.pos[1] * cs.scale, this.data.mapping[2] * cs.scale,
            this.data.mapping[3] * cs.scale, this.data.mapping[0], this.data.mapping[1],
            this.data.mapping[2], this.data.mapping[3]);
    }

    read_data_from_gui() {
        super.read_data_from_gui();
        this.data.mapping = [
            parseInt($("#editor-element-u").val()),
            parseInt($("#editor-element-v").val()),
            parseInt($("#editor-element-w").val()),
            parseInt($("#editor-element-h").val())
        ];
    }

    write_data_to_gui() {
        super.write_data_to_gui();
        $("#editor-element-u").val(this.data.mapping[0]);
        $("#editor-element-v").val(this.data.mapping[1]);
        $("#editor-element-w").val(this.data.mapping[2]);
        $("#editor-element-h").val(this.data.mapping[3]);
    }
};

class button extends texture {
    constructor(json) {
        super(json);
        this.pressed = false;
    }

    type() { return 'button'; }

    draw(painter) {
        if (this.pressed) {
            let cs = painter.cs();
            painter.image_crop(atlas, cs.origin.x - cs.offset.x + this.data.pos[0] * cs.scale,
                cs.origin.y - cs.offset.y + this.data.pos[1] * cs.scale, this.data.mapping[2] * cs.scale,
                this.data.mapping[3] * cs.scale, this.data.mapping[0],
                this.data.mapping[1] + constants.texture_space + this.data.mapping[3],
                this.data.mapping[2], this.data.mapping[3]);
        } else {
            super.draw(painter, cs);
        }
    }
};

class keyboard_button extends button {
    constructor(json) { super(json); }

    type() { return 'keyboard_button'; }

    on_button_input(vc, state) {
        if (vc == this.data.code)
            this.pressed = state;
    }

    read_data_from_gui() {
        super.read_data_from_gui();
        this.data.code = parseInt($('#editor-element-keycode').val(), 16);
    }

    write_data_to_gui() {
        super.write_data_to_gui();
        $('#editor-element-keycode').val('0x' + this.data.code.toString(16));
    }
};

element_map.set(element_types.KEYBOARD_KEY, json => { return new keyboard_button(json); });
element_map.set(element_types.TEXTURE, json => { return new texture(json); });

function create_element(json) {
    if (element_map.has(json.type))
        return element_map.get(json.type)(json);
    return null;
}
