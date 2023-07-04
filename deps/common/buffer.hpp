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

#pragma once
#include <malloc.h>
#include <cassert>
#include <cstring>

typedef unsigned char byte;
class buffer {
    byte *m_buf = nullptr;
    size_t m_length = 0, m_write_pos = 0, m_read_pos = 0;
    bool m_owned{true};

public:
    buffer(byte *data, size_t len) : m_buf(data), m_length(len), m_owned(false) {}

    buffer(size_t len = 0)
    {
        if (len > 0)
            resize(len);
    }

    ~buffer()
    {
        if (m_owned)
            free(m_buf);
        m_buf = nullptr;
        m_length = 0;
        m_write_pos = 0;
    }

    void reset()
    {
        m_read_pos = 0;
        m_write_pos = 0;
    }

    void resize(size_t new_size)
    {
        assert(new_size < 0xffff);
        m_buf = reinterpret_cast<byte *>(realloc(m_buf, new_size));
        if (m_buf)
            m_length = new_size;
    }

    void write(const void *data, size_t size)
    {
        if (m_write_pos + size >= m_length)
            resize(m_write_pos + size * 1.5);
        memcpy(m_buf + m_write_pos, data, size);
        m_write_pos += size;
    }

    byte *read(size_t size)
    {
        if (size + m_read_pos <= m_length) {
            auto *result = m_buf + m_read_pos;
            m_read_pos += size;
            return result;
        }
        return nullptr;
    }

    void read(void **dest, size_t size)
    {
        if (size + m_read_pos <= m_length) {
            *dest = reinterpret_cast<void *>(m_buf + m_read_pos);
            m_read_pos += size;
        }
    }

    template<class T> T *write(const T &val)
    {
        if (m_write_pos + sizeof(T) >= m_length)
            resize(m_write_pos + sizeof(T) * 1.5);
        memcpy(m_buf + m_write_pos, &val, sizeof(T));
        auto *res = reinterpret_cast<T *>(m_buf + m_write_pos);
        m_write_pos += sizeof(T);
        return res;
    }

    template<class T> T *read()
    {
        if (sizeof(T) + m_read_pos <= m_length) {
            auto result = reinterpret_cast<T *>(m_buf + m_read_pos);
            m_read_pos += sizeof(T);
            return result;
        }
        return nullptr;
    }

    byte &operator[](size_t idx) { return m_buf[idx]; }

    const byte &operator[](size_t idx) const { return m_buf[idx]; }

    size_t length() const { return m_length; }
    size_t write_pos() const { return m_write_pos; }
    size_t read_pos() const { return m_read_pos; }
    size_t data_left() const { return m_length - m_read_pos; }
    byte *get() { return m_buf; }
    template<class T> T get() { return reinterpret_cast<T>(m_buf); }
};
