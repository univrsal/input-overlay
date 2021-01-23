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

$(function() {
    $(".coord").on("keydown", e => {
        if (!e)
            var e = window.event;
        if (e.keyCode == 13) {
            if (cfg !== null &&
                cfg.selected_elements.length === 1) { // We only apply settings if a single element is selected
                let pos = new vec2($("#selected-element-x").val(), $("#selected-element-y").val());
                let dim = new r4($("#selected-element-u").val(), $("#selected-element-v").val(),
                                 $("#selected-element-w").val(), $("#selected-element-h").val());

                dim.validate();
                pos.validate();

                cfg.selected_elements[0].set_dim(pos, dim);
                cfg.set_selection(cfg.selected_elements[0]);
            }
            e.preventDefault();
        }
    });
});