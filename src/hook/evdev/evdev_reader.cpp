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

#include "evdev_reader.hpp"

#include "evdev_keycode_map.hpp"
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include "util/log.h"

EvdevReader::EvdevReader() = default;

EvdevReader::~EvdevReader()
{
    stop();
}

void EvdevReader::start(const std::string &device_path)
{
    stop();

    // Create an eventfd used to wake the reader thread during stop().
    stop_fd_ = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (stop_fd_ < 0) {
        berr("Failed to create eventfd for evdev reader");
        return;
    }

    running_ = true;
    thread_ = std::thread(&EvdevReader::reader_thread, this, device_path);
}

void EvdevReader::stop()
{
    running_ = false;

    // Signal the eventfd so poll() in the reader thread wakes up immediately.
    if (stop_fd_ >= 0) {
        uint64_t val = 1;
        write(stop_fd_, &val, sizeof(val));
    }

    if (thread_.joinable()) {
        thread_.join();
    }

    // Close the stop eventfd after the thread has exited.
    if (stop_fd_ >= 0) {
        close(stop_fd_);
        stop_fd_ = -1;
    }

    // Close the device fd if the thread didn't already.
    int fd_copy = fd_.exchange(-1);
    if (fd_copy >= 0) {
        close(fd_copy);
    }
}

bool EvdevReader::is_running() const
{
    return running_;
}

bool EvdevReader::is_key_pressed(uint16_t evdev_code) const
{
    if (evdev_code > EVDEV_KEY_MAX)
        return false;
    std::lock_guard<std::mutex> lock(mutex_);
    auto uihook_code = evdev_to_uiohook(evdev_code);
    if (key_state.find(uihook_code) != key_state.end()) {
        return key_state.at(uihook_code);
    }
    return false;
}

int32_t EvdevReader::get_rel_state(uint16_t rel_code) const
{
    if (rel_code > EVDEV_REL_MAX)
        return 0;
    std::lock_guard<std::mutex> lock(mutex_);
    return rel_state_[rel_code];
}

void EvdevReader::set_rel_state(uint16_t rel_code, int32_t value)
{
    if (rel_code > EVDEV_REL_MAX)
        return;
    std::lock_guard<std::mutex> lock(mutex_);
    rel_state_[rel_code] = value;
}

int32_t EvdevReader::take_rel_state_locked(uint16_t rel_code)
{
    if (rel_code > EVDEV_REL_MAX)
        return 0;
    const auto value = rel_state_[rel_code];
    rel_state_[rel_code] = 0;
    return value;
}

void EvdevReader::reader_thread(const std::string &path)
{
    int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        running_ = false;
        return;
    }
    fd_.store(fd);

    struct pollfd pfds[2];
    pfds[0].fd = fd;
    pfds[0].events = POLLIN;
    pfds[1].fd = stop_fd_;
    pfds[1].events = POLLIN;

    struct input_event ev {};

    while (running_) {
        int ret = poll(pfds, 2, -1);
        if (ret < 0) {
            if (errno == EINTR)
                continue;
            break; // poll error
        }

        // Check if we were asked to stop (eventfd became readable).
        if (pfds[1].revents & POLLIN)
            break;

        // Check for errors/hangup on the device fd.
        if (pfds[0].revents & (POLLERR | POLLHUP | POLLNVAL))
            break;

        // Read all available events.
        if (pfds[0].revents & POLLIN) {
            while (true) {
                ssize_t n = read(fd, &ev, sizeof(ev));
                if (n != static_cast<ssize_t>(sizeof(ev)))
                    break; // EAGAIN (no more data) or real error

                std::lock_guard lock(mutex_);
                auto uihook_code = evdev_to_uiohook(ev.code);
                bdebug("Read evdev event: type %u, code %u, value %d, uihook: 0x%X", ev.type, ev.code, ev.value,
                      uihook_code);
                switch (ev.type) {
                case EV_KEY:
                    if (ev.code <= EVDEV_KEY_MAX) {
                        auto mouse_button = evdev_mouse_to_uiohook(ev.code);
                        if (ev.value == 1) { // pressed
                            if (mouse_button != 0)
                                mouse_state[mouse_button] = true;
                            else
                                key_state[uihook_code] = true;
                            bdebug("Key pressed: evdev code %u, uiohook code %u", ev.code, uihook_code);
                        } else if (ev.value == 0) { // released
                            if (mouse_button != 0)
                                mouse_state[mouse_button] = false;
                            else
                                key_state[uihook_code] = false;
                            bdebug("Key released: evdev code %u, uiohook code %u", ev.code, uihook_code);
                        }
                        // value == 2 is repeat — we keep the key as pressed.
                    }
                    break;

                case EV_REL:
                    if (ev.code <= EVDEV_REL_MAX) {
                        rel_state_[ev.code] += ev.value;
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }

    // Close the device fd.
    int expected = fd;
    if (fd_.compare_exchange_strong(expected, -1)) {
        close(fd);
    }
    running_ = false;
}
