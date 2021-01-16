class vec2 {
    constructor(x, y)
    {
        this.x = x;
        this.y = y;
    }

    addv(other) { return new vec2(this.x + other.x, this.y + other.y); }

    add(x, y) { return new vec2(this.x + x, this.y + y); }

    subv(other) { return new vec2(this.x - other.x, this.y - other.y); }

    sub(x, y) { return new vec2(this.x - x, this.y - y); }

    lt(x, y) { return this.x < x && this.y < y; }
    ltv(v) { return this.x < v.x && this.y < v.y; }

    max(x = 0, y = 0)
    {
        this.x = Math.max(this.x, x);
        this.y = Math.max(this.y, y);
    }
};

class r4 {
    constructor(x, y, w, h)
    {
        this.x = x;
        this.y = y;
        this.w = w;
        this.h = h;
    }

    is_point_inside(v) { return this.x <= v.x && this.y <= v.y && v.x <= this.x + this.w && v.y <= this.y + this.h; }

    union(other)
    {
        this.x = Math.min(other.x, this.x);
        this.y = Math.min(other.x, this.y);
        this.w = other.w + this.w - Math.abs(this.w - other.w);
        this.h = other.h + this.h - Math.abs(this.h - other.h);
    }

    is_empty() { return this.w === 0 && this.h === 0; }
}

function r2(_w, _h)
{
    return {w: _w, h: _h};
}