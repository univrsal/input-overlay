function to_vc(key)
{
    switch (key) {
    case "a":
        return 0x001E;
    case "b":
        return 0x0030;
    case "c":
        return 0x002E;
    case "d":
        return 0x0020;
    case "e":
        return 0x0012;
    case "f":
        return 0x0021;
    case "g":
        return 0x0022;
    case "h":
        return 0x0023;
    case "i":
        return 0x0017;
    case "j":
        return 0x0024;
    case "k":
        return 0x0025;
    case "l":
        return 0x0026;
    case "m":
        return 0x0032;
    case "n":
        return 0x0031;
    case "o":
        return 0x0018;
    case "p":
        return 0x0019;
    case "q":
        return 0x0010;
    case "r":
        return 0x0013;
    case "s":
        return 0x001F;
    case "t":
        return 0x0014;
    case "u":
        return 0x0016;
    case "v":
        return 0x002F;
    case "w":
        return 0x0011;
    case "x":
        return 0x002D;
    case "y":
        return 0x0015;
    case "z":
        return 0x002C;
    default:
        return 0;
    }
}

var config = {
    data: {},
    elements: [],

    load_from_json(json) {
        this.data = json;
        this.data["elements"].forEach(data => {
            let new_element = create_element(data);
            if (new_element !== null)
                this.elements.push(new_element);
        });
    },

    draw(painter, cs) {
        let ctx = painter.get_context();
        ctx.save();
        ctx.rect(cs.origin.x, cs.origin.y, cs.dimensions.w, cs.dimensions.h);
        ctx.clip();
        this.elements.forEach(element => element.draw(painter, cs));
        ctx.restore();
    },

    on_button(event, state) {
        let vc = to_vc(event.key);
        this.elements.forEach(element => element.on_button_input(vc, state));
    }
}