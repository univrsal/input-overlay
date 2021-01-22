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
    BUTTON: 1,
    WHEEL: 2,
    /* MOUSE_STATS, */
    ANALOG_STICK: 3,
    ET_TRIGGER: 4,
    /* Shows game pad number 1 through 4 */
    ET_GAMEPAD_ID: 5,
    ET_DPAD_STICK: 6,
};

let constants = {texture_space: 3};

let element_map = new Map();

class element {
    constructor(json) { this.data = json; }

    draw(painter) {}
    on_button_input(vc, state) {}

    scaled_dim(cs)
    {
        return new r4(cs.origin.x - cs.offset.x + this.data.pos[0] * cs.scale,
                      cs.origin.y - cs.offset.y + this.data.pos[1] * cs.scale, this.data.mapping[2] * cs.scale,
                      this.data.mapping[3] * cs.scale);
    }

    dim() { return new r4(this.data.pos[0], this.data.pos[1], this.data.mapping[2], this.data.mapping[3]); }

    x() { return this.data.pos[0]; }
    y() { return this.data.pos[1]; }
    u() { return this.data.mapping[0]; }
    v() { return this.data.mapping[1]; }
    w() { return this.data.mapping[2]; }
    h() { return this.data.mapping[3]; }

    set_pos(x, y)
    {
        this.data.pos[0] = x;
        this.data.pos[1] = y;
    }

    layer() { return this.data["z_level"]; }
};

class texture extends element {

    draw(painter)
    {
        // default draw handling
        let cs = painter.cs();
        painter.image_crop(atlas, cs.origin.x - cs.offset.x + this.data.pos[0] * cs.scale,
                           cs.origin.y - cs.offset.y + this.data.pos[1] * cs.scale, this.data.mapping[2] * cs.scale,
                           this.data.mapping[3] * cs.scale, this.data.mapping[0], this.data.mapping[1],
                           this.data.mapping[2], this.data.mapping[3]);
    }
}

class button extends texture {
    constructor(json)
    {
        super(json);
        this.pressed = false;
    }

    draw(painter)
    {
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

    on_button_input(vc, state)
    {
        if (vc == this.data["code"])
            this.pressed = state;
    }
}

element_map.set(element_types.BUTTON, json => { return new button(json); });
element_map.set(element_types.TEXTURE, json => { return new element(json); });

function create_element(json)
{
    if (element_map.has(json["type"]))
        return element_map.get(json["type"])(json);
    return null;
}
