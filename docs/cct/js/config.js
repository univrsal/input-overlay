var config = {
    data: {},
    elements: [],

    load_from_json(json) {
        this.data = json;
        this.data["elements"].forEach(data => {
            let new_element = create_element(data);
            if (new_element !== null)
                this.elements.push(new_element);
        });
    },

    draw(painter, cs) {
        let ctx = painter.get_context();
        ctx.save();
        ctx.rect(cs.origin.x, cs.origin.y, cs.dimensions.w, cs.dimensions.h);
        ctx.clip();
        this.elements.forEach(element => { element.draw(painter, cs); });
        ctx.restore();
    }
}