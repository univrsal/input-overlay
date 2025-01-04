/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.cc/alex/input-overlay
 * Copyright 2025 univrsal <uni@vrsal.xyz>.
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

var drop_areas = new Map();

function preventDefaults(e) {
  e.preventDefault();
  e.stopPropagation();
}

function handle_file_selected(files, id) {
  if (drop_areas.has(id)) drop_areas.get(id).on_select(files);
}

class drop_area {
  constructor(
    id,
    type = 'application/json',
    label = 'Select or drag file here'
  ) {
    drop_areas.set(id, this);
    this.area = document.getElementById(id);
    // Prevent default drag behaviors
    ['dragenter', 'dragover', 'dragleave', 'drop'].forEach((eventName) => {
      this.area.addEventListener(eventName, preventDefaults, false);
      document.body.addEventListener(eventName, preventDefaults, false);
    });

    // Highlight drop area when item is dragged over it
    ['dragenter', 'dragover'].forEach((eventName) => {
      this.area.addEventListener(eventName, (e) => this.highlight(e), false);
    });
    ['dragleave', 'drop'].forEach((eventName) => {
      this.area.addEventListener(eventName, (e) => this.unhighlight(e), false);
    });

    // Handle dropped files
    this.area.addEventListener('drop', (e) => this.handle_drop(e), false);

    // Handle file select
    this.area.innerHTML = `
    <form>
    <input type="file" id="${id}-input" accept="${type}" onchange="handle_file_selected(this.files, '${id}')">
    <label class="button" id="${id}-label" for="${id}-input">${label}</label>
    <p style="display: inline" id="${id}-file-label"></p>
    </form>
    `;
    this.label = document.getElementById(id + '-file-label');
    this.handlers = [];
  }

  on_select(files) {
    this.handlers.forEach((h) => h(files));
    this.label.innerText = files[0].name;
  }

  highlight(e) {
    this.area.classList.add('highlight');
  }

  unhighlight(e) {
    this.area.classList.remove('highlight');
  }

  handle_drop(e) {
    var dt = e.dataTransfer;
    var files = dt.files;

    this.on_select(files);
  }
}
