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

function download_json(file_name, content) {
  var json_data = new Blob([JSON.stringify(content, null, 4)], {
    type: 'text/json',
  });
  if (window.navigator && window.navigator.msSaveOrOpenBlob) {
    // for IE
    window.navigator.msSaveOrOpenBlob(json_data, file_name);
  } else {
    // for Non-IE (chrome, firefox etc.)
    var a = document.createElement('a');
    document.body.appendChild(a);
    a.style = 'display: none';
    var csvUrl = URL.createObjectURL(json_data);
    a.href = csvUrl;
    a.download = file_name;
    a.click();
    URL.revokeObjectURL(a.href);
    a.remove();
  }
}
