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

function apply_settings() {
    if (cfg !== null && cfg.selected_elements.length === 1) { // We only apply settings if a single element is selected
        let pos = new vec2($("#selected-element-x").val(), $("#selected-element-y").val());
        let dim = new r4($("#selected-element-u").val(), $("#selected-element-v").val(), $("#selected-element-w").val(),
            $("#selected-element-h").val());

        dim.validate();
        pos.validate();

        cfg.selected_elements[0].set_dim(pos, dim);
        cfg.set_selection(cfg.selected_elements[0]);
    }
}

function new_type_dropdown() {
    document.getElementById("type-dropdown").classList.toggle("show");
}

function open_editor(element_type) {
    var d = document.getElementById("edit-element-dialog");

    if (d !== null) {
        d.style.opacity = 1;
        d.style.pointerEvents = "all";
        d.style.display = "block";
        main.style.pointerEvents = "none";
        main.classList.add("blurred");
        bg.classList.add("blurred");
    }
}

function close_dialog(handler) {
    var d = document.getElementById("edit-element-dialog");
    if (handler !== undefined)
        handler();
    if (d !== null) {
        d.style.opacity = 0;
        d.style.pointerEvents = "none";
        main.style.pointerEvents = "all";
        main.classList.remove("blurred");
        bg.classList.remove("blurred");
    }
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
        if (cfg.selected_elements.length < 2)
            cfg.delete_selection();
    });
    $(window).on("click", e => {
        if (!e.target.matches('.dropbtn')) {
            var dropdowns = document.getElementsByClassName("dropdown-content");
            var i;
            for (i = 0; i < dropdowns.length; i++) {
                var openDropdown = dropdowns[i];
                if (openDropdown.classList.contains('show')) {
                    openDropdown.classList.remove('show');
                }
            }
        }
    });
});