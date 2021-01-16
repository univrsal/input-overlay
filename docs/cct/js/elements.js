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

    draw(painter, cs) {}
    on_button_input(vc, state) {}
};

class texture extends element {

    draw(painter, cs)
    {
        // default draw handling
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

    draw(painter, cs)
    {
        if (this.pressed) {
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
