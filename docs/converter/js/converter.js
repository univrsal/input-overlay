$(() => {
    new drop_area("config-drop-area").handlers.push((files) => {
        if (files && files.length > 0) {
            let reader = new FileReader();
            reader.readAsDataURL(files[0]);
            reader.onloadend = function() {
                let split = reader.result.split(',');
                if (split.length < 1)
                    return;
                let b64 = split[1];
                let str = atob(b64);
                let cfg = parse_legacy_config(str);
                let json = convert_legacy_cfg(cfg);
            };
        }
    });
});

function parse_legacy_config(str) {
    str = str.replace('\r', '');
    lines = str.split('\n');
    lines = $(lines).filter((e, i) => i.length > 0 && !i.trim().startsWith('#'));
    
    cfg = {};
    $(lines).each((e, i) => {
        switch(i[0]) {
            case '0':
            case '1':
            case '2':
            case '3':
                i = i.substr(2);
        }
        splits = i.split('=');
        key = splits[0];
        val = splits[1];
        cfg[key] = val;
    });

    console.log(cfg);
    return cfg;
}

function convert_legacy_cfg(cfg) {
    if (cfg.layout_type !== '2') {
        alert('Converter only works with keyboard layouts');
        return;
    }

    let csv_to_num = (obj) => obj.split(',').each((i, e) => cols.push(toNumber(e)));
    let csv_to_str = (obj) => obj.split(',').each((i, e) => cols.push(e));

    let kc = toNumber(cfg.key_count);
    let kw = toNumber(cfg.key_abs_w);
    let kh = toNumber(cfg.key_abs_h);
    let rows = csv_to_num(cfg.key_rows);
    let cols = csv_to_num(cfg.key_cols);
    let h = csv_to_num(cfg.key_height);
    let w = csv_to_num(cfg.key_width);
    let codes = csv_to_num(cfg.key_order);
    let vspace = toNumber(cfg.key_space_v);
    let hspace = toNumber(cfg.key_space_h);
    let textures_per_row = toNumber(cfg.texture_w);
    let texture_v_space = toNumber(cfg.texture_v_space);
    let u = 0;
    let v = 0;
    let layout = {};
    layout.elements = [];
    layout.default_height = kh;
    layout.default_width = kw;
    layout.flags = 0;
    layout.space_v = vspace;
    layout.space_h = hspace;

    for (let i = 0; i < kc; i++) {
        let key = {};
        key.id = `key_{i}`;
        key.type = 1;
        key.z_level = 1;
        key.pos = [ (cols[i] + vspace) * kw, (rows[i] + hspace) * kh ];
        key.mapping = [
            1 + 
        ];
        
        if (i % textures_per_row === 0) {
            u = 0;
            v += texture_v_space;
        }
        layout.elements.push(key);
    }

    return layout;
}
