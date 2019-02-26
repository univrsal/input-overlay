/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

class input_entry;

class handler
{
public:
    virtual ~handler() = default;
    virtual void update() = 0;
    virtual void tick(float seconds) = 0;
    virtual void swap(input_entry* current) = 0;
    virtual void render() = 0;
};
