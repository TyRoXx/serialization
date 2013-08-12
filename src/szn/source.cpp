#include "source.hpp"


namespace szn
{
	source::~source()
	{
	}


	void empty_source::load(length_type)
	{
	}

	length_type empty_source::size()
	{
		return 0;
	}

	char empty_source::get(length_type)
	{
		throw std::runtime_error("szn::EmptySource cannot be read from");
	}

	void empty_source::drop(length_type)
	{
	}

	const char *empty_source::data()
	{
		return NULL;
	}

	bool empty_source::is_stable() const
	{
		return true;
	}


	void zero_source::load(length_type n)
	{
		(void)n;
	}

	length_type zero_source::size()
	{
		return 1;
	}

	char zero_source::get(length_type index)
	{
		(void)index;
		return 0;
	}

	void zero_source::drop(length_type n)
	{
		(void)n;
	}

	const char *zero_source::data()
	{
		return "";
	}

	bool zero_source::is_stable() const
	{
		return true;
	}


	stream_source::stream_source(std::istream &in)
		: m_in(in)
		, m_size(0)
	{
	}

	void stream_source::load(length_type n)
	{
		size_t const clamped_n = static_cast<size_t>(std::min<length_type>(n, m_buffer.max_size()));
		size_t const prev_size = m_buffer.size();
		if (clamped_n > prev_size)
		{
			m_buffer.resize(clamped_n);
			size_t const bytes_to_read = clamped_n - prev_size;
			m_in.read(&m_buffer.front() + prev_size, bytes_to_read);
			std::streamsize const read = m_in.gcount();
			assert(static_cast<size_t>(read) <= bytes_to_read);
			m_buffer.resize(prev_size + static_cast<size_t>(read));
		}
		m_size = std::min(clamped_n, m_buffer.size());
	}

	length_type stream_source::size()
	{
		return m_size;
	}

	char stream_source::get(length_type index)
	{
		assert(index < m_size);
		assert(index < m_buffer.size());
		return m_buffer[static_cast<size_t>(index)];
	}

	void stream_source::drop(length_type n)
	{
		assert(n <= m_size);
		assert(n <= m_buffer.size());
		m_buffer.erase(m_buffer.begin(), m_buffer.begin() + static_cast<ptrdiff_t>(n));
		m_size -= n;
	}

	const char *stream_source::data()
	{
		return m_buffer.empty() ? NULL : &m_buffer.front();
	}

	bool stream_source::is_stable() const
	{
		return false;
	}
}
