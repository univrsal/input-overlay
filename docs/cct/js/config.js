function to_vc(key)
{
    /* clang-format off */
    switch(key) {
        case "a": return 0x001E;
        case "b": return 0x0030;
        case "c": return 0x002E;
        case "d": return 0x0020;
        case "e": return 0x0012;
        case "f": return 0x0021;
        case "g": return 0x0022;
        case "h": return 0x0023;
        case "i": return 0x0017;
        case "j": return 0x0024;
        case "k": return 0x0025;
        case "l": return 0x0026;
        case "m": return 0x0032;
        case "n": return 0x0031;
        case "o": return 0x0018;
        case "p": return 0x0019;
        case "q": return 0x0010;
        case "r": return 0x0013;
        case "s": return 0x001F;
        case "t": return 0x0014;
        case "u": return 0x0016;
        case "v": return 0x002F;
        case "w": return 0x0011;
        case "x": return 0x002D;
        case "y": return 0x0015;
        case "z": return 0x002C;
        default: return 0;
    }
    /* clang-format on */
}

var config = {
    data: {},
    elements: [],
    selected_elements: [],
    selecting: false,
    dragging: false,
    drag_selection: new r4(), // Mouse dragged selection in screen space
    selection_rect: new r4(), // Actual selected element(s) in coordinate space
    drag_offset: new vec2(),  // MousePos - SelectionRect (unscaled)
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
        if (this.selecting && !this.drag_selection.is_empty()) {
            painter.rect_outline(this.drag_selection.x - 0.5, this.drag_selection.y - 0.5, this.drag_selection.w,
                                 this.drag_selection.h);
        }

        if (!this.selection_rect.is_empty()) {
            let r = cs.translate_rect_to_screen(this.selection_rect);
            painter.rect_outline(r.x + 0.5, r.y + 0.5, r.w - 1, r.h - 1, 1, "#ff0000ff");
        }

        ctx.restore();
    },

    on_button(event, state) {
        let vc = to_vc(event.key);
        this.elements.forEach(element => element.on_button_input(vc, state));
    },

    mouseup(event, cs) {
        this.selecting = false;
        this.dragging = false;
    },

    start_dragging(event, cs) {
        let tv = cs.translate_point_to_cs(event.clientX, event.clientY);
        this.drag_offset.x = tv.x - this.selection_rect.x;
        this.drag_offset.y = tv.y - this.selection_rect.y;
        this.dragging = true;
        this.selecting = false;
    },

    mousedown(event, cs) {
        if (event.button == 0 && cs.is_mouse_over(event)) {
            let r = cs.translate_rect_to_screen(this.selection_rect);
            let m = new vec2(event.clientX, event.clientY);
            if (r.is_point_inside(m)) {
                this.start_dragging(event, cs);
            } else {
                // Element selection
                this.selected_elements = [];
                this.selection_rect = new r4();

                // Select individual element
                this.elements.forEach(e => {
                    if (e.scaled_dim(cs).is_point_inside(m)) {
                        this.selected_elements.push(e);
                        this.selection_rect = e.dim();
                        return false;
                    }
                });

                // No element was directly selected -> allow drag selection
                if (this.selection_rect.is_empty()) {
                    this.selecting = true;
                    this.dragging = false;
                    this.drag_selection.x = event.clientX;
                    this.drag_selection.y = event.clientY;
                    this.drag_selection.w = 0;
                    this.drag_selection.h = 0;
                } else {
                    this.start_dragging(event, cs);
                }
            }
        }
    },

    move(event, cs) {
        if (this.selecting) {
            this.drag_selection.w = event.clientX - this.drag_selection.x;
            this.drag_selection.h = event.clientY - this.drag_selection.y;

            let selected_elements = [];
            let rect = new r4();
            let first = true;
            let pos = new vec2(event.clientX, event.clientY);
            this.elements.forEach(e => {
                if (e.scaled_dim(cs).is_inside(this.drag_selection)) {
                    selected_elements.push(e);
                    if (first) {
                        rect = e.dim();
                        first = false;
                    } else {
                        rect.union(e.dim());
                    }
                }
            });

            this.selected_elements = selected_elements;
            this.selection_rect = rect;
        } else if (this.dragging) {
            let tv = cs.translate_point_to_cs(event.clientX, event.clientY);
            this.selection_rect.x = tv.x - this.drag_offset.x;
            this.selection_rect.y = tv.y - this.drag_offset.y;
            this.selection_rect.max();
        }
    }
}