/*************************************************************************
 * This file is part of input-overlay
 * github.con/univrsal/input-overlay
 * Copyright 2020 univrsal <uni@vrsal.cf>.
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

#include <malloc.h>
#include <cassert>

typedef unsigned char byte;
class buffer {
	byte *m_buf = nullptr;
	std::size_t m_length = 0, m_write_pos = 0, m_read_pos = 0;

public:
	buffer(std::size_t len = 0)
	{
		if (len > 0)
			resize(len);
	}

	~buffer()
	{
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

	void resize(std::size_t new_size)
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

	void read(void **dest, size_t size)
	{
		if (size + m_read_pos < m_length) {
			*dest = reinterpret_cast<void *>(m_buf + m_read_pos);
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
		if (sizeof(T) + m_read_pos < m_length) {
			auto result = reinterpret_cast<T *>(m_buf + m_read_pos);
			m_read_pos += sizeof(T);
			return result;
		}
		return nullptr;
	}

	byte &operator[](std::size_t idx) { return m_buf[idx]; }

	const byte &operator[](std::size_t idx) const { return m_buf[idx]; }

	void to_packet(udp_packet &p)
	{
		p.len = m_write_pos;
		p.data = m_buf;
		p.maxlen = m_length;
	}

	size_t length() { return m_length; }
	size_t write_pos() { return m_write_pos; }
	byte *get() { return m_buf; }
};