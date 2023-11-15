/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/input-overlay
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
    MOUSE_MOVEMENT: 9,
};

let overlay_flags = {
    LEFT_STICK: 1 << 0,
    RIGHT_STICK: 1 << 1,
    GAMEPAD: 1 << 2,
    MOUSE: 1 << 3,
};

let constants = { texture_space: 3 };

let element_map = new Map();

function type_from_string(type) {
    switch (type) {
        case "mouse_button":
            return element_types.MOUSE_BUTTON;
        case "keyboard_button":
            return element_types.KEYBOARD_KEY;
        case "texture":
            return element_types.TEXTURE;
        case "gamepad_button":
            return element_types.GAMEPAD_BUTTON;
        case "analog_stick":
            return element_types.ANALOG_STICK;
        case "trigger":
            return element_types.TRIGGER;
        case "mouse_wheel":
            return element_types.WHEEL;
        case "player_id":
            return element_types.GAMEPAD_ID;
        case "dpad":
            return element_types.DPAD_STICK;
        case "mouse_movement":
            return element_types.MOUSE_MOVEMENT;
        default:
            return -1;
    }
}

function type_from_id(type) {
    switch (type) {
        case element_types.MOUSE_BUTTON:
            return "mouse_button";
        case element_types.KEYBOARD_KEY:
            return "keyboard_button";
        case element_types.TEXTURE:
            return "texture";
        case element_types.GAMEPAD_BUTTON:
            return "gamepad_button";
        case element_types.ANALOG_STICK:
            return "analog_stick";
        case element_types.TRIGGER:
            return "trigger";
        case element_types.MOUSE_MOVEMENT:
            return "mouse_movement";
        case element_types.WHEEL:
            return "mouse_wheel";
        case element_types.GAMEPAD_ID:
            return "player_id";
        default:
            return "";
    }
}

class element {
    constructor(json) { this.data = json; }

    draw(painter) { }
    on_keyboard_input(vc, state) { }
    on_mouse_input(vc, state) { }
    on_gamepad_input(pad) { }
    on_scroll_input(event) { }

    tick() { }

    read_data_from_gui() {
        this.data.id = $("#editor-element-id").val();
        this.data.z_level = parseInt($("#editor-element-layer").val());
    }

    write_data_to_gui() {
        $("#editor-element-id").val(this.data.id);
        $("#editor-element-layer").val(this.data.z_level);
    }

    dim() { return new r4(this.x(), this.y(), this.w(), this.h()); }

    set_dim(pos, uvwh) {
        this.data.pos[0] = pos.x;
        this.data.pos[1] = pos.y;
        this.data.mapping[0] = uvwh.x;
        this.data.mapping[1] = uvwh.y;
        this.data.mapping[2] = uvwh.w;
        this.data.mapping[3] = uvwh.h;
    }

    type() { return this.data.type; }
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
}

class texture extends element {
    draw(painter) {
        // default draw handling

        painter.image_crop(atlas, this.x(),
            this.y(), this.w(), this.h(),
            this.u(), this.v(), this.w(), this.h());
    }

    read_data_from_gui() {
        super.read_data_from_gui();
        this.data.mapping = [
            parseInt($("#editor-element-u").val()),
            parseInt($("#editor-element-v").val()),
            parseInt($("#editor-element-w").val()),
            parseInt($("#editor-element-h").val()),
        ];
    }

    write_data_to_gui() {
        super.write_data_to_gui();
        $("#editor-element-u").val(this.u());
        $("#editor-element-v").val(this.v());
        $("#editor-element-w").val(this.w());
        $("#editor-element-h").val(this.h());
    }
}

class button extends texture {
    constructor(json) {
        super(json);
        this.pressed = false;
    }

    draw(painter) {
        if (this.pressed) {
            painter.image_crop(atlas, this.x(),
                this.y(), this.w(),
                this.h(), this.u(), this.v() + constants.texture_space + this.h(), this.w(),
                this.h());
        } else {
            super.draw(painter);
        }
    }

    read_data_from_gui() {
        super.read_data_from_gui();
        this.data.code = parseInt($("#editor-element-keycode").val(), 16);
    }

    write_data_to_gui() {
        super.write_data_to_gui();
        $("#editor-element-keycode").val("0x" + this.data.code.toString(16).toUpperCase());
    }
}

class keyboard_button extends button {
    on_keyboard_input(vc, state) {
        if (vc === this.data.code)
            this.pressed = state;
    }
}

class mouse_button extends button {
    on_mouse_input(vc, state) {
        if (vc === this.data.code)
            this.pressed = state;
    }
}

class gamepad_button extends button {
    on_gamepad_input(pad) {
        let btn = gamepad_from_vc(this.data.code);

        if (btn < pad.buttons.length)
            this.pressed = pad.buttons[btn].pressed;
    }
}

class mouse_wheel extends texture {
    constructor(json) {
        super(json);
        this.direction = 0;
        this.pressed = false;
        this.lastWheelInput = Date.now();
    }

    on_mouse_input(vc, state) {
        if (vc === 3)
            this.pressed = state;
    }

    on_scroll_input(event) {
        if (event.originalEvent.deltaY < 0) {
            this.direction = -1;
        } else {
            this.direction = 1;
        }
        this.lastWheelInput = Date.now();
    }

    tick() {
        if (Date.now() - this.lastWheelInput > 250)
            this.direction = 0;
    }

    draw(painter) {

        super.draw(painter);
        if (this.pressed) {
            painter.image_crop(atlas, this.x(),
                this.y(), this.w(),
                this.h(), this.u() + constants.texture_space + this.w(), this.v(), this.w(),
                this.h());
        }

        if (this.direction === 1) {
            painter.image_crop(atlas, this.x(),
                this.y(), this.w(),
                this.h(), this.u() + (constants.texture_space + this.w()) * 2, this.v(),
                this.w(), this.h());
        } else if (this.direction === -1) {
            painter.image_crop(atlas, this.x(),
                this.y(), this.w(),
                this.h(), this.u() + (constants.texture_space + this.w()) * 3, this.v(),
                this.w(), this.h());
        }
    }
}

class analog_stick extends texture {
    draw(painter) {

        let xPos = 0;
        let yPos = 0;
        let pressed = false;
        if (pad.lastInput) {
            if (this.data.side === 0) {
                // left
                xPos = pad.lastInput.axes[0] * this.data.stick_radius;
                yPos = pad.lastInput.axes[1] * this.data.stick_radius;
                pressed = pad.lastInput.buttons[10].pressed;
            } else {
                // right
                xPos = pad.lastInput.axes[2] * this.data.stick_radius;
                yPos = pad.lastInput.axes[3] * this.data.stick_radius;
                pressed = pad.lastInput.buttons[11].pressed;
            }

            if (pressed) {
                painter.image_crop(atlas, (this.x() + xPos),
                    (this.y() + yPos), this.w(),
                    this.h(), this.u(), this.v() + constants.texture_space + this.h(),
                    this.w(), this.h());
            } else {
                painter.image_crop(atlas, (this.x() + xPos),
                    (this.y() + yPos), this.w(),
                    this.h(), this.u(), this.v(), this.w(), this.h());
            }
        } else {
            super.draw(painter);
        }
    }

    read_data_from_gui() {
        super.read_data_from_gui();
        this.data.stick_radius = parseInt($("#editor-element-analog-stick-radius").val());
        this.data.side = parseInt($("#editor-element-analog-stick-side").val());
    }

    write_data_to_gui() {
        super.write_data_to_gui();
        $("#editor-element-analog-stick-radius").val(this.data.stick_radius);
        $("#editor-element-analog-stick-side").val(this.data.side);
    }

    is_left_stick() { return this.side === 0; }
}

class gamepad_id extends texture {
    constructor(json) {
        super(json);
        this.pressed = false;
    }

    on_gamepad_input(pad) {
        let btn = gamepad_from_vc(this.data.code);

        if (pad.buttons.length > 16)
            this.pressed = pad.buttons[16].pressed;
    }

    draw(painter) {

        if (pad.lastInput) {
            if (this.pressed) {
                painter.image_crop(atlas, this.x(),
                    this.y(), this.w(),
                    this.h(), this.u() + (constants.texture_space + this.w()) * 4, this.v(),
                    this.w(), this.h());
            }
            painter.image_crop(atlas, this.x(),
                this.y(), this.w(),
                this.h(),
                this.u() + (constants.texture_space + this.w()) * pad.lastInput.index, this.v(),
                this.w(), this.h());
        } else {
            painter.image_crop(atlas, this.x(),
                this.y(), this.w(),
                this.h(), this.u(), this.v(), this.w(), this.h());
        }
    }
}

class trigger extends texture {
    draw(painter) {

        let progress = 0;
        let pressed = false;

        if (pad.lastInput) {
            if (this.data.side === 0) {
                // left
                progress = pad.lastInput.buttons[6].value;
                pressed = pad.lastInput.buttons[6].pressed;
            } else {
                // right
                progress = pad.lastInput.buttons[7].value;
                pressed = pad.lastInput.buttons[7].pressed;
            }

            if (this.data.trigger_mode) {
                if (pressed) {
                    painter.image_crop(atlas, this.x(),
                        this.y(), this.w(),
                        this.h(), this.u(), this.v() + constants.texture_space + this.h(),
                        this.w(), this.h());
                } else {
                    painter.image_crop(atlas, this.x(),
                        this.y(), this.w(),
                        this.h(), this.u(), this.v(), this.w(), this.h());
                }
            } else {
                super.draw(painter);
                if (this.data.direction === 1) {
                    // fill upwards
                    let offset = (1 - progress) * this.h();
                    painter.image_crop(atlas, this.x(),
                        (this.y() + offset), this.w(),
                        this.h() * progress, this.u(),
                        this.v() + this.h() + constants.texture_space + offset, this.w(),
                        this.h() * progress);
                } else if (this.data.direction === 2) {
                    // fill downwards
                    painter.image_crop(atlas, this.x(),
                        this.y(), this.w(),
                        this.h() * progress, this.u(),
                        this.v() + constants.texture_space + this.h(), this.w(), this.h() * progress);
                } else if (this.data.direction === 3) {
                    // fill to the left
                    let offset = (1 - progress) * this.w();
                    painter.image_crop(atlas, (this.x() + offset),
                        this.y(), this.w() * progress,
                        this.h(), this.u() + offset,
                        this.v() + constants.texture_space + this.h(), this.w() * progress, this.h());
                } else if (this.data.direction === 4) {
                    // fill to the right
                    painter.image_crop(atlas, this.x(),
                        this.y(), this.w() * progress,
                        this.h(), this.u(), this.v() + constants.texture_space + this.h(),
                        this.w() * progress, this.h());
                }
            }
        } else {
            super.draw(painter);
        }
    }

    read_data_from_gui() {
        super.read_data_from_gui();
        this.data.direction = parseInt($("#editor-element-trigger-direction-side").val());
        this.data.trigger_mode = $("#editor-element-trigger-button")[0].checked;
        this.data.side = parseInt($("#editor-element-analog-stick-side").val());
    }

    write_data_to_gui() {
        super.write_data_to_gui();
        $("#editor-element-trigger-direction-side").val(this.data.direction);
        $("#editor-element-trigger-button")[0].checked = this.data.trigger_mode ? true : false;
        $("#editor-element-analog-stick-side").val(this.data.side);
    }
}

class dpad extends texture {
    constructor(json) {
        super(json);
        this.offset = 0;
    }
    on_gamepad_input(pad) {
        let up = pad.buttons[12].pressed;
        let down = pad.buttons[13].pressed;
        let left = pad.buttons[14].pressed;
        let right = pad.buttons[15].pressed;

        if (left) {
            if (up)
                this.offset = 5;
            else if (down)
                this.offset = 7;
            else
                this.offset = 1;
        } else if (right) {
            if (up)
                this.offset = 6;
            else if (down)
                this.offset = 8;
            else
                this.offset = 2;
        } else if (up) {
            if (left)
                this.offset = 5;
            else if (right)
                this.offset = 6;
            else
                this.offset = 3;
        } else if (down) {
            if (left)
                this.offset = 7;
            else if (right)
                this.offset = 8;
            else
                this.offset = 4;
        } else {
            this.offset = 0;
        }
    }

    draw(painter) {

        painter.image_crop(atlas, this.x(),
            this.y(), this.w(), this.h(),
            this.u() + (constants.texture_space + this.w()) * this.offset, this.v(), this.w(), this.h());
    }
}
class mouse_movement extends texture {
    // TODO: Actually visualize this in the editor
    read_data_from_gui() {
        super.read_data_from_gui();
        this.data.mouse_type = parseInt($("#editor-element-movement-type").val());
    }

    write_data_to_gui() {
        super.write_data_to_gui();
        $("#editor-element-movement-type").val(this.data.mouse_type);
    }
}
element_map.set(element_types.KEYBOARD_KEY, (json) => { return new keyboard_button(json); });
element_map.set(element_types.MOUSE_BUTTON, (json) => { return new mouse_button(json); });
element_map.set(element_types.TEXTURE, (json) => { return new texture(json); });
element_map.set(element_types.GAMEPAD_BUTTON, (json) => { return new gamepad_button(json); });
element_map.set(element_types.WHEEL, (json) => { return new mouse_wheel(json); });
element_map.set(element_types.ANALOG_STICK, (json) => { return new analog_stick(json); });
element_map.set(element_types.TRIGGER, (json) => { return new trigger(json); });
element_map.set(element_types.GAMEPAD_ID, (json) => { return new gamepad_id(json); });
element_map.set(element_types.DPAD_STICK, (json) => { return new dpad(json); });
element_map.set(element_types.MOUSE_MOVEMENT, (json) => { return new mouse_movement(json); });

function create_element(json) {
    if (element_map.has(json.type))
        return element_map.get(json.type)(json);
    return null;
}
