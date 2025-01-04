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

function parseIntSafe(val, fallback = 0) {
  let parsed = parseInt(val);
  return isNaN(parsed) ? fallback : parsed;
}

class vec2 {
  constructor(x = 0, y = 0) {
    this.x = x;
    this.y = y;
  }

  addv(other) {
    return new vec2(this.x + other.x, this.y + other.y);
  }

  add(x, y) {
    return new vec2(this.x + x, this.y + y);
  }

  subv(other) {
    return new vec2(this.x - other.x, this.y - other.y);
  }

  sub(x, y) {
    return new vec2(this.x - x, this.y - y);
  }

  lt(x, y) {
    return this.x < x && this.y < y;
  }
  ltv(v) {
    return this.x < v.x && this.y < v.y;
  }

  scale(s) {
    return new vec2(this.x * s, this.y * s);
  }

  limit(min_x = 0, min_y = 0, max_x = 3000, max_y = 3000) {
    this.x = Math.min(Math.max(this.x, min_x), max_x);
    this.y = Math.min(Math.max(this.y, min_y), max_y);
  }

  validate() {
    this.x = parseIntSafe(this.x);
    this.y = parseIntSafe(this.y);
  }
}

class r4 {
  constructor(x = 0, y = 0, w = 0, h = 0) {
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
  }

  from_points(a, b) {
    this.x = Math.min(a.x, b.x);
    this.y = Math.min(a.y, b.y);
    this.w = Math.max(a.x, b.x) - this.x;
    this.h = Math.max(a.y, b.y) - this.y;
  }

  validate() {
    this.x = parseIntSafe(this.x);
    this.y = parseIntSafe(this.y);
    this.w = parseIntSafe(this.w);
    this.h = parseIntSafe(this.h);
  }

  reset() {
    this.x = 0;
    this.y = 0;
    this.w = 0;
    this.h = 0;
  }

  copy() {
    return new r4(this.x, this.y, this.w, this.h);
  }

  get_left() {
    return this.w < 0 ? this.x + this.w : this.x;
  }

  get_right() {
    return this.w >= 0 ? this.x + this.w : this.x;
  }

  get_top() {
    return this.h < 0 ? this.y + this.h : this.y;
  }

  get_bottom() {
    return this.h >= 0 ? this.y + this.h : this.y;
  }

  is_coord_inside(x, y) {
    return (
      x >= this.get_left() &&
      x <= this.get_right() &&
      y >= this.get_top() &&
      y <= this.get_bottom()
    );
  }

  is_point_inside(v) {
    return (
      v.x >= this.get_left() &&
      v.x <= this.get_right() &&
      v.y >= this.get_top() &&
      v.y <= this.get_bottom()
    );
  }

  /* true when this is inside other */
  is_inside(other) {
    return (
      other.get_left() <= this.get_left() &&
      other.get_right() >= this.get_right() &&
      other.get_top() <= this.get_top() &&
      other.get_bottom() >= this.get_bottom()
    );
  }

  union(other) {
    if (this.is_empty() && other.is_empty()) return;

    if (this.is_empty()) {
      this.x = other.x;
      this.y = other.y;
      this.w = other.w;
      this.h = other.h;
    }

    if (other.is_empty()) return;

    let tmin, tmax, omin, omax;

    /* Horizontal union */
    tmin = this.get_left();
    tmax = this.get_right();
    omin = other.get_left();
    omax = other.get_right();

    if (omin < tmin) tmin = omin;

    this.x = tmin;
    if (omax > tmax) tmax = omax;
    this.w = tmax - tmin;

    /* Vertical union */
    tmin = this.get_top();
    tmax = this.get_bottom();
    omin = other.get_top();
    omax = other.get_bottom();

    if (omin < tmin) tmin = omin;
    this.y = tmin;
    if (omax > tmax) tmax = omax;
    this.h = tmax - tmin;
  }

  grow(i) {
    return new r4(this.x - i, this.y - i, this.w + i * 2, this.h + i * 2);
  }

  max(x = 0, y = 0) {
    this.x = Math.max(this.x, x);
    this.y = Math.max(this.y, y);
  }

  maxw(w = 0, h = 0) {
    this.w = Math.max(this.w, w);
    this.h = Math.max(this.h, h);
  }

  is_empty() {
    return this.w === 0 && this.h === 0;
  }
}

class r2 {
  constructor(w = 0, h = 0) {
    this.w = w;
    this.h = h;
  }
}
