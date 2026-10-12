/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.cc/alex/input-overlay
 * based on https://codeberg.org/MaxCross/obs-input-overlay-zig
 * Copyright 2026 univrsal <uni@vrsal.cc>.
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

#pragma once

#include <atomic>
#include <bitset>
#include <unordered_map>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>

/// Maximum evdev key code value we track (matches KEY_MAX in the Linux kernel).
constexpr size_t EVDEV_KEY_MAX = 0x2FF;

/// Maximum relative axis code we track (matches REL_MAX).
constexpr size_t EVDEV_REL_MAX = 0x0F;

/// Reads raw input_event structs from an evdev device file and maintains
/// live key/button and relative-axis state that can be queried from any
/// thread.
class EvdevReader {
public:
    EvdevReader();
    ~EvdevReader();

    EvdevReader(const EvdevReader &) = delete;
    EvdevReader &operator=(const EvdevReader &) = delete;

    /// (Re)start reading from a new device path.  Stops any running reader
    /// first.
    void start(const std::string &device_path);

    /// Stop the reader thread and close the device.
    void stop();

    /// Returns true while the reader thread is alive.
    bool is_running() const;

    /// Query whether a key/button with the given evdev code is currently
    /// pressed.
    bool is_key_pressed(uint16_t evdev_code) const;

    /// Query the most recent value for a relative axis (REL_X, REL_Y, etc.).
    int32_t get_rel_state(uint16_t rel_code) const;

    /// Reset a relative axis value to zero (useful for one-shot events like
    /// scroll).
    void set_rel_state(uint16_t rel_code, int32_t value);

    /// Return and reset a relative axis value while the caller already holds
    /// mutex_.
    int32_t take_rel_state_locked(uint16_t rel_code);

    mutable std::mutex mutex_;
    std::unordered_map<uint16_t, bool> key_state{};
    std::unordered_map<uint16_t, bool> mouse_state{};

private:
    void reader_thread(const std::string &path);

    int32_t rel_state_[EVDEV_REL_MAX + 1]{};

    std::thread thread_;
    std::atomic<bool> running_{false};
    std::atomic<int> fd_{-1};
    int stop_fd_{-1}; ///< eventfd used to wake the reader thread on shutdown
};
