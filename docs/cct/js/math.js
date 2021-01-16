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

    max(x = 0, y = 0)
    {
        this.x = Math.max(this.x, x);
        this.y = Math.max(this.y, y);
    }
};

function r2(_w, _h)
{
    return {w: _w, h: _h};
}