class vec2 {
    constructor(x = 0, y = 0)
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
    constructor(x = 0, y = 0, w = 0, h = 0)
    {
        this.x = x;
        this.y = y;
        this.w = w;
        this.h = h;
    }

    get_left() { return this.w < 0 ? this.x + this.w : this.x; }

    get_right() { return this.w >= 0 ? this.x + this.w : this.x; }

    get_top() { return this.h < 0 ? this.y + this.h : this.y; }

    get_bottom() { return this.h >= 0 ? this.y + this.h : this.y; }

    is_point_inside(v)
    {
        return v.x >= this.get_left() && v.x <= this.get_right() && v.y >= this.get_top() && v.y <= this.get_bottom();
    }

    /* true when this is inside other */
    is_inside(other)
    {
        return other.get_left() <= this.get_left() && other.get_right() >= this.get_right() &&
               other.get_top() <= this.get_top() && other.get_bottom() >= this.get_bottom();
    }

    union(other)
    {
        this.x = Math.min(other.get_left(), this.get_left());
        this.y = Math.min(other.get_top(), this.get_top());
        this.w = Math.max(other.get_right(), this.get_right()) - this.x;
        this.h = Math.max(other.get_bottom(), this.get_bottom()) - this.y;
    }

    max(x = 0, y = 0)
    {
        this.x = Math.max(this.x, x);
        this.y = Math.max(this.y, y);
    }

    is_empty() { return this.w === 0 && this.h === 0; }
}

class r2 {
    constructor(w = 0, h = 0)
    {
        this.w = w;
        this.h = h;
    }
}