/**
 * This file is part of input-overlay
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
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

    virtual void clear() = 0;
};
