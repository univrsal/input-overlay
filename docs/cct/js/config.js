var config = {
    elements: [],
    load_from_json(json) {
        this.elements = json["elements"];
    },
    draw(painter, cs) {
        let ctx = painter.get_context();
        ctx.save();
        ctx.rect(cs.origin.x, cs.origin.y, cs.dimensions.w, cs.dimensions.h);
        ctx.clip();
        this.elements.forEach(element => {
                painter.image_crop(atlas,
                cs.origin.x - cs.offset.x + element.pos[0] * cs.scale,
                cs.origin.y - cs.offset.y + element.pos[1] * cs.scale,
                element.mapping[2] * cs.scale , element.mapping[3] * cs.scale,
                element.mapping[0], element.mapping[1],
                element.mapping[2], element.mapping[3]);
        });
        ctx.restore();
    }
}