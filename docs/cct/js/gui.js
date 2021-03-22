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

var current_type = -1;
var is_editing_element = false;

function apply_settings() {
    if (cfg !== null && cfg.selected_elements.length > 0) {

        if (!cfg.is_name_unique($("#selected-element-id").val())) {
            alert('The element id is not unique.');
            return;
        }

        if (cfg.selected_elements.length === 1) {
            // We only apply these settings if a single element is selected
            let pos = new vec2($("#selected-element-x").val(), $("#selected-element-y").val());
            let dim = new r4($("#selected-element-u").val(), $("#selected-element-v").val(), $("#selected-element-w").val(),
                $("#selected-element-h").val());

            dim.validate();
            pos.validate();

            cfg.selected_elements[0].set_dim(pos, dim);
            cfg.selected_elements[0].set_layer($("#selected-element-layer").val());
            cfg.set_selection(cfg.selected_elements[0]);
        } else {
            // Only set the layer if there's multiple elements selected
            cfg.selected_elements.forEach(element => {
                element.set_layer($("#selected-element-layer").val());
            });
        }
    }
}

function new_type_dropdown() {
    document.getElementById("type-dropdown").classList.toggle("show");
}

function show_property(prop) {
    prop.label.css({ "display": "flex" });
    if (prop.div)
        prop.div.css({ "display": "flex" });
}

function hide_property(prop) {
    if (prop.div)
        prop.label.css({ "display": "none" });
    if (prop.div)
        prop.div.css({ "display": "none" });
}

function set_description(desc_element, text) {
    desc_element.label[0].innerHTML = '<p>' + text + '</p>';
    show_property(desc_element)
}

function get_property(type) {
    return { label: $('#editor-element-' + type + '-label'), div: $('#editor-element-' + type + '-container') }
}

function type_from_string(type) {
    switch (type) {
        case 'mouse_button':
            return element_types.MOUSE_BUTTON;
        case 'keyboard_button':
            return element_types.KEYBOARD_KEY;
        case 'texture':
            return element_types.TEXTURE;
        case 'gamepad_button':
            return element_types.GAMEPAD_BUTTON;
        case 'analog_stick':
            return element_types.ANALOG_STICK;
        case 'trigger':
            return element_types.TRIGGER;
        case 'mouse_movement':
            return element_types.MOUSE_MOVEMENT;
        case 'mouse_wheel':
            return element_types.WHEEL;
        case 'player_id':
            return element_types.GAMEPAD_ID;
        default: return -1;
    }
}

function setup_editor(type) {
    let analog_stick_side = get_property('analog-stick-side');
    let analog_stick_radius = get_property('analog-stick-radius');
    let keycode = get_property('keycode');
    let trigger_dir = get_property('trigger-direction');
    let movement_type = get_property('movement-type');
    let description = get_property('description');
    let button = get_property('trigger-button');
    let record = get_property('record-code');

    current_type = type_from_string(type);
    // First hide all optional settings
    hide_property(analog_stick_side);
    hide_property(analog_stick_radius);
    hide_property(trigger_dir);
    hide_property(keycode);
    hide_property(movement_type);
    hide_property(description);
    hide_property(button);
    hide_property(record);

    switch (type) {
        case 'gamepad_button':
        case 'mouse_button':
        case 'keyboard_button': {
            show_property(keycode);
            show_property(record);
            break;
        }
        case 'analog_stick': {
            show_property(analog_stick_radius);
            show_property(analog_stick_side);
            break;
        }
        case 'trigger': {
            show_property(trigger_dir);
            show_property(button);
            set_description(description, `The trigger can either be a button, meaning that it'll
                only be either on or off, or it can display the position it is in by filling up
                in a direction.`);
            break;
        }
        case 'mouse_movement': {
            show_property(movement_type);
            break;
        }
        case 'mouse_wheel': {
            set_description(description, `The mouse wheel expects the following three textures 
                to be next to eachother in the atlas:<br>
                &nbsp;- neutral<br>
                &nbsp;- clicked<br>
                &nbsp;- scrolling up<br>
                &nbsp;- scrolling down<br>You only have to select the first one.`);
            break;
        }
        case 'player_id': {
            set_description(description, `Displays the controller id from one to four. Usually in the order they
            were connected in. The following textures have to be next to eachother in the atlas:<br>
            &nbsp;- Player 1<br>
            &nbsp;- Player 2<br>
            &nbsp;- Player 3<br>
            &nbsp;- Player 4<br>
            &nbsp;- <a href="https://i.imgur.com/LdH3nnz.png" target="_tab">Guide</a> pressed<br>
            You only have to select the first one.<br>`);
            break;
        }
    }
}

function open_editor(element_type, edit) {
    let d = $("#edit-element-dialog")[0];
    let c = $('#main-canvas-container')[0];
    let e = $('#element-dialog')[0];
    let title = $('#editor-title')[0];
    is_editing_element = edit;
    setup_editor(element_type);

    if (edit) {
        if (cfg.selected_elements.length > 0) {
            title.innerText = 'Edit ' + cfg.selected_elements[0].id();
        } else {
            alert('Please select at least one element first');
            return;
        }
    } else {
        let id = element_type.replace('_', ' ');
        title.innerText = 'Create new ' + id;
        $('#editor-element-id').val(id + cfg.elements.length);
    }

    if (d !== null) {
        d.style.opacity = 1;
        d.style.pointerEvents = "all";
        d.style.display = "block";
        c.style.pointerEvents = "none";
        c.classList.add("blurred");
        e.style.pointerEvents = "none";
        e.classList.add("blurred");
    }
    main_painter.enabled = false;
    cfg.enabled = false;
    editor_painter.resize_canvas();
}

function close_editor(handler) {
    main_painter.enabled = true;
    cfg.enabled = true;

    let d = $("#edit-element-dialog")[0];
    let c = $('#main-canvas-container')[0];
    let e = $('#element-dialog')[0];

    if (handler !== undefined)
        handler();
    if (d !== null) {
        d.style.opacity = 0;
        d.style.pointerEvents = "none";
        c.style.pointerEvents = "all";
        c.classList.remove("blurred");
        e.classList.remove("blurred");
        e.style.pointerEvents = "all";
    }
}


function apply_editor() {
    if (!cfg.is_name_unique($("#editor-element-id").val())) {
        alert("Element id isn't unique");
        return;
    }

    if (is_editing_element) {

    } else {
        let json = {
            type: current_type,
            mapping: [
                parseInt($("#editor-element-u").val()),
                parseInt($("#editor-element-v").val()),
                parseInt($("#editor-element-w").val()),
                parseInt($("#editor-element-h").val())
            ],
            pos: [0, 0],
            id: $("#editor-element-id").val(),
            z_level: parseInt($("#editor-element-layer").val())
        };

        let new_element = create_element(json);
        new_element.read_data_from_gui();
        cfg.elements.push(new_element);
    }
    close_editor();
}

$(function () {
    $(".coord").on("keydown", e => {
        if (!e)
            var e = window.event;
        if (e.keyCode == 13) {
            apply_settings();
            e.preventDefault();
        }
    });
    $("#ok").on("click", e => apply_settings());
    $("#del").on("click", e => {
        if (cfg.selected_elements.length > 1) {
            // Ask for comfirmation when deleting more than one element
            if (confirm("You are about to delete " + cfg.selected_elements.length +
                " elements. Are you sure?")) {
                cfg.delete_selection();
            }
        } else {
            cfg.delete_selection();
        }
    });

    $(window).on("click", e => {
        if (!e.target.matches('.dropbtn')) {
            let dropdowns = document.getElementsByClassName("dropdown-content");
            let i;
            for (i = 0; i < dropdowns.length; i++) {
                let openDropdown = dropdowns[i];
                if (openDropdown.classList.contains('show')) {
                    openDropdown.classList.remove('show');
                }
            }
        }
    });

    // setup numeric textboxes
    let numerics = $('.numeric');
    for (let i = 0; i < numerics.length; i++) {
        let e = numerics[i];
        $(e).on('keydown', e => {
            var k = e.which;
            console.log(k);
            /* numeric inputs can come from the keypad or the numeric row at the top
             * also arrows, home & end and backspace
             */
            if (!((k >= 48 && k <=57) || (k >= 96 && k <= 105) || (k >= 35 && k <= 40) || k === 8)) {
                e.preventDefault();
                return false;
            }
        });
    }
});