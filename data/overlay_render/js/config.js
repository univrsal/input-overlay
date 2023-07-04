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

class config {

    constructor(canvas_id, painter)
    {
        this.data = {};
        this.elements = [];
        this.painter = painter;
        this.is_ctrl_down = false;
        this.internal_clipboard = null;
        this.enabled = true; // false when a dialog is open
        //$(canvas_id).on('mousemove', e => this.move(e, this.painter.cs()));
        $(canvas_id).on('mouseup', e => this.mouseup(e, this.painter.cs()));
        $(canvas_id).on('mousedown', e => this.mousedown(e, this.painter.cs()));
        $(canvas_id).on('wheel', e => this.on_scroll(e));
        $(window).on('keydown', e => this.on_button(e, true));
        $(window).on('keyup', e => this.on_button(e, false));
        pad.on('input', p => this.gamepad_input(p));
        this.load_callbacks = [];
        setInterval(() => this.tick(), 250);
    }

    tick() { this.elements.forEach(e => e.tick()); }

    on_scroll(e) { this.elements.forEach(element => element.on_scroll_input(e)); }

    add_elements(j)
    {
        if (j) {
            if (j.length > 0)
                this.selected_elements = [];
            j.forEach(e => {
                let orig_id = e.id.replace(/\d+$/, "");
                let counter = 0;
                let id = orig_id + counter;
                e.pos[1] += 4; // offset so you can see the pasted elements
                while (!this.is_name_unique(id))
                    id = orig_id + counter++;
                e.id = id;
                let new_element = create_element(e);
                this.elements.push(new_element);
                this.selected_elements.push(new_element);
            });
        }
    }

    add_load_callback(cb) { this.load_callbacks.push(cb); }

    load_from_json(json)
    {
        this.data = json;
        this.data["elements"].forEach(data => {
            let new_element = create_element(data);
            if (new_element !== null)
                this.elements.push(new_element);
        });
        this.sort_elements();
        this.load_callbacks.forEach(cb => cb());
    }

    sort_elements()
    {
        this.elements.sort((a, b) => { return a.layer() - b.layer(); });
    }

    draw(painter)
    {
        if (atlas === null) // Don't draw if image hasn't loaded yet
            return;

        this.elements.forEach(element => element.draw(painter));
    }

    delete_selection()
    {
        let new_elements = this.elements.filter(function(to_filter) {
            return this.selected_elements.find(function(to_find) { return to_filter.id() === to_find.id(); }) ===
                   undefined;
        }, this);
        this.elements = new_elements;
        this.deselect();
    }

    is_name_unique(name, editing_element)
    {
        let unique = true;
        this.elements.some(e => {
            if (e.id() === name) {
                if (!editing_element || e !== editing_element) {
                    unique = false;
                    return true;
                }
            }
            return false;
        });
        return unique;
    }

    gamepad_input(pad) { this.elements.forEach(e => e.on_gamepad_input(pad)); }

    on_button(event, state)
    {
        if (!this.enabled)
            return;
        if (event.key == 'Control')
            this.is_ctrl_down = state;

        let vc = key_to_vc(event);
        this.elements.forEach(element => element.on_keyboard_input(vc, state));
    }

    mouseup(event, cs)
    {
        this.selecting = false;
        this.dragging = false;
        let vc = mouse_to_vc(event);
        this.elements.forEach(element => element.on_mouse_input(vc, false));
    }

    mousedown(event, cs)
    {
        let vc = mouse_to_vc(event);
        this.elements.forEach(element => element.on_mouse_input(vc, true));
    }
}
