# Wayland evdev validation

This branch contains an experimental validation patch for the `evdev` Wayland input path.

## Tested environment

Validated locally on:

- CachyOS
- COSMIC Wayland session
- NVIDIA proprietary driver
- OBS Studio 32.1.2, custom COSMIC dockfix build
- `input-overlay` `evdev` branch plus this patch

## What was validated

- Keyboard overlay using an evdev keyboard device
- Mouse overlay using an evdev mouse device
- Mouse button press/release state
- Relative mouse movement accumulation
- OBS preview no longer freezes after avoiding a nested evdev mutex lock

The local test used these devices:

```text
/dev/input/event2  # SONiX USB DEVICE keyboard
/dev/input/event5  # USB Gaming Mouse
```

Stable `/dev/input/by-id` paths are preferred when available because `eventN` numbers can change after reboot or USB replug.

## Known limitations

- Wayland/evdev support is still experimental.
- OBS must have read permission for the selected `/dev/input/event*` device.
- Device paths are hardware-specific.
- This has been validated on one local CachyOS COSMIC Wayland/NVIDIA setup so far.
- More testing is needed on other compositors, distributions, and devices.

## Why this patch exists

The upstream `evdev` branch handled keyboard state but did not fully merge mouse state into the overlay data path. An initial local mouse patch exposed a deadlock: `overlay::refresh_data()` held the evdev reader mutex and then called helpers that attempted to lock the same mutex again. This branch adds mouse state handling and consumes relative movement while already holding the reader lock.
