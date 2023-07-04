/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 Alex <uni@vrsal.xyz>.
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

var key2vc = new Map();
var mouse2vc = new Map();
var gamepad2vc = new Map();
var vc2gamepad = new Map();

$(function() {
    let vc = (id, vc) => key2vc.set(id, vc);
    let gp = (id, vc) => {
        gamepad2vc.set(id, vc);
        vc2gamepad.set(vc, id);
    };

    // some keys are postfixed with their DOM3 location
    // No postfix = location 0
    // 1 = Left, 2 = Right, 3 = Numpad
    vc("KeyA", 0x001E);
    vc("KeyB", 0x0030);
    vc("KeyC", 0x002E);
    vc("KeyD", 0x0020);
    vc("KeyE", 0x0012);
    vc("KeyF", 0x0021);
    vc("KeyG", 0x0022);
    vc("KeyH", 0x0023);
    vc("KeyI", 0x0017);
    vc("KeyJ", 0x0024);
    vc("KeyK", 0x0025);
    vc("KeyL", 0x0026);
    vc("KeyM", 0x0032);
    vc("KeyN", 0x0031);
    vc("KeyO", 0x0018);
    vc("KeyP", 0x0019);
    vc("KeyQ", 0x0010);
    vc("KeyR", 0x0013);
    vc("KeyS", 0x001F);
    vc("KeyT", 0x0014);
    vc("KeyU", 0x0016);
    vc("KeyV", 0x002F);
    vc("KeyW", 0x0011);
    vc("KeyX", 0x002D);
    vc("KeyY", 0x0015);
    vc("KeyZ", 0x002C);
    vc("Digit1", 0x0002);
    vc("Digit2", 0x0003);
    vc("Digit3", 0x0004);
    vc("Digit4", 0x0005);
    vc("Digit5", 0x0006);
    vc("Digit6", 0x0007);
    vc("Digit7", 0x0008);
    vc("Digit8", 0x0009);
    vc("Digit9", 0x000A);
    vc("Digit0", 0x000B);
    vc("F1", 0x003B);
    vc("F2", 0x003C);
    vc("F3", 0x003D);
    vc("F4", 0x003E);
    vc("F5", 0x003F);
    vc("F6", 0x0040);
    vc("F7", 0x0041);
    vc("F8", 0x0042);
    vc("F9", 0x0043);
    vc("F10", 0x0044);
    vc("F11", 0x0057);
    vc("F12", 0x0058);
    vc("F13", 0x005B);
    vc("F14", 0x005C);
    vc("F15", 0x005D);
    vc("F16", 0x0063);
    vc("F17", 0x0064);
    vc("F18", 0x0065);
    vc("F19", 0x0066);
    vc("F20", 0x0067);
    vc("F21", 0x0068);
    vc("F22", 0x0069);
    vc("F23", 0x006A);
    vc("F24", 0x006B);
    vc("Escape", 0x0001);
    vc("Minus", 0x000C);
    vc("Equal", 0x000D);
    vc("Backspace", 0x000E);
    vc("Tab", 0x000F);
    vc("CapsLock", 0x003A);
    vc("BracketRight", 0x001A);
    vc("BracketLeft", 0x001B);
    vc("Backslash", 0x002B);
    vc("Semicolon", 0x0027);
    vc("Quote", 0x0028);
    vc("Enter", 0x001C);
    vc("Comma", 0x0033);
    vc("Period", 0x0034);
    vc("Slash", 0x0035);
    vc("Space", 0x0039);
    vc("PrintScreen", 0x0E37);
    vc("ScrolLock", 0x0046);
    vc("Pause", 0x0E45);
    vc("IntlBackslash", 0x0E46);
    vc("Insert", 0x0E52);
    vc("Delete", 0x0E53);
    vc("Home", 0x0E47);
    vc("End", 0x0E4F);
    vc("PageUp", 0x0E49);
    vc("PageDown", 0x0E51);
    vc("ArrowUp", 0xE048);
    vc("ArrowLeft", 0xE04B);
    vc("Clear", 0xE04C);
    vc("ArrowRight", 0xE04D);
    vc("ArrowDown", 0xE050);

    vc("NumpadEquals", 0x0E0D);
    vc("NumLock", 0x0045);
    vc("NumpadSubtract", 0x0E35);
    vc("NumpadMultiply", 0x0037);
    vc("NumpadSubtract", 0x004A);
    vc("NumpadAdd", 0x004E);
    vc("NumpadEnter", 0x0E1C);
    vc("NumpadDecimal", 0x0053);

    vc("Numpad1", 0x004F);
    vc("Numpad2", 0x0050);
    vc("Numpad3", 0x0051);
    vc("Numpad4", 0x004B);
    vc("Numpad5", 0x004C);
    vc("Numpad6", 0x004D);
    vc("Numpad7", 0x0047);
    vc("Numpad8", 0x0048);
    vc("Numpad9", 0x0049);
    vc("Numpad0", 0x0052);

    vc("NumpadEnd", 0xEE4F);
    vc("NumpadArrowDown", 0xEE50);
    vc("NumpadPageDown", 0xEE51);
    vc("NumpadArrowLeft", 0xEE4B);
    vc("NumpadClear", 0xEE4C);
    vc("NumpadArrowRight", 0xEE4D);
    vc("NumpadHome", 0xEE47);
    vc("NumpadArrowUp", 0xEE48);
    vc("NumpadPageUp", 0xEE49);
    vc("NumpadInsert", 0xEE52);
    vc("NumpadDelete", 0xEE53);

    vc("ShiftLeft", 0x002A);
    vc("ShiftRight", 0x0036);
    vc("ControlLeft", 0x001D);
    vc("ControlRight", 0x0E1D);
    vc("AltLeft", 0x0038);
    vc("AltRight", 0x0E38);
    vc("MetaLeft", 0x0E5B);
    vc("MetaRight", 0x0E5C);
    vc("ContextMenu", 0x0E5D);

    mouse2vc.set(0, 1);
    mouse2vc.set(1, 3);
    mouse2vc.set(2, 2);
    mouse2vc.set(3, 4);
    mouse2vc.set(4, 5);

    gp(0, 0xEC00);
    gp(1, 0xEC01);
    gp(2, 0xEC02);
    gp(3, 0xEC03);
    gp(4, 0xEC04);
    gp(5, 0xEC05);
    gp(8, 0xEC06);
    gp(9, 0xEC07);
    gp(10, 0xEC09);
    gp(11, 0xEC10);
    gp(12, 0xEC13);
    gp(13, 0xEC14);
    gp(14, 0xEC11);
    gp(15, 0xEC12);
    gp(16, 0xEC08);
});

function gamepad_to_vc(button)
{
    if (gamepad2vc.has(button))
        return gamepad2vc.get(button);
    return 0;
}

function gamepad_from_vc(vc)
{
    if (vc2gamepad.has(vc))
        return vc2gamepad.get(vc);
    return 0;
}

function mouse_to_vc(e)
{
    let button = e.button;

    if (mouse2vc.has(button))
        return mouse2vc.get(button);
    return 0;
}

function key_to_vc(e)
{
    let key = e.code;

    if (key.indexOf("Numpad") >= 0 && /^[0-9]$/g.test(key.slice(-1))) {
        // Numpad has special handling
        // all keys Numpad1 through Numpad9 should use the key
        // so that we can tell numpad buttons with numlock on apart from
        // numpad buttons with numpad off
        key = key.slice(0, -1) + e.key;
    }

    if (key === "NumpadDecimal" && e.keyCode === 46)
        key = "NumpadDelete";

    if (key2vc.has(key))
        return key2vc.get(key);
    return 0;
}
