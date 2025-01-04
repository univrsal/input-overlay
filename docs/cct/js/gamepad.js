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

class gamepad {
  constructor(interval = 25) {
    this.lastTimestamp = new Map();
    this.lastState = new Map();
    this.lastInput = null;
    this.connect = [];
    this.disconnect = [];
    this.button = []; // (gamepad, button index, button)
    this.axis = []; // (gamepad, axis index, new axis value)
    this.event = []; // (gamepad)
    this.threshold = 0.001; // If the value delta is higher than this we consider the value changed
    window.addEventListener('gamepadconnected', (event) =>
      this.onConnect(event)
    );
    window.addEventListener('gamepaddisconnected', (event) =>
      this.onConnect(event)
    );
    this.handle = setInterval(() => this.tick(), interval);
  }

  tick() {
    let pads = navigator.getGamepads();
    for (let i = 0; i < pads.length; pads++) {
      let pad = pads[i];
      if (!pad) continue;
      let lastTimeStamp = 0;
      let lastState = null;

      if (this.lastTimestamp.has(pad.index))
        lastTimeStamp = this.lastTimestamp.get(pad.index);
      if (this.lastState.has(pad.index))
        lastState = this.lastState.get(pad.index);

      if (pad && pad.timestamp > lastTimeStamp) {
        this.lastTimestamp.set(pad.index, pad.timestamp);
        if (lastState) {
          for (let j = 0; j < pad.axes.length; j++) {
            if (Math.abs(lastState.axes[j] - pad.axes[j]) > this.threshold)
              this.onAxis(pad, j);
          }

          for (let j = 0; j < pad.buttons.length; j++) {
            if (
              Math.abs(lastState.buttons[j].value - pad.buttons[j].value) >
              this.threshold
            )
              this.onButton(pad, j);
          }
        } else {
          for (let j = 0; j < pad.axes.length; j++) this.onAxis(pad, j);
          for (let j = 0; j < pad.buttons.length; j++) this.onButton(pad, j);
        }
        this.lastState.set(pad.index, pad);
      }
    }
  }

  onAxis(pad, index) {
    this.axis.forEach((h) => h(pad, index, pad.axes[index]));
    this.event.forEach((h) => h(pad));
    this.lastInput = pad;
  }

  onButton(pad, index) {
    this.button.forEach((h) => h(pad, index, pad.buttons[index]));
    this.event.forEach((h) => h(pad));
    this.lastInput = pad;
  }

  on(id, handler) {
    switch (id.toLowerCase()) {
      case 'connect':
        this.connect.push(handler);
        break;
      case 'disconnect':
        this.disconnect.push(handler);
        break;
      case 'button':
        this.button.push(handler);
        break;
      case 'move':
        this.axis.push(handler);
        break;
      case 'input':
        this.event.push(handler);
        break;
    }
  }

  onConnect(event) {
    this.connect.forEach((h) => h(event));
  }

  onDisconnect(event) {
    this.disconnect.forEach((h) => h(event));
  }
}
