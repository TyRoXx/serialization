#include "source.hpp"
#include <cassert>


namespace szn
{
	Source::~Source()
	{
	}

	MemorySource::MemorySource(const char *begin, const char *end)
		: m_begin(begin)
		, m_end(end)
	{
	}

	MemorySource::MemorySource(const unsigned char *begin, const unsigned char *end)
		: m_begin(reinterpret_cast<const char *>(begin))
		, m_end(reinterpret_cast<const char *>(end))
	{
	}

	MemorySource::MemorySource(const signed char *begin, const signed char *end)
		: m_begin(reinterpret_cast<const char *>(begin))
		, m_end(reinterpret_cast<const char *>(end))
	{
	}

	void MemorySource::load(std::size_t n)
	{
		(void)n;
		//nothing to do
	}

	std::size_t MemorySource::size()
	{
		return static_cast<std::size_t>(std::distance(m_begin, m_end));
	}

	char MemorySource::get(std::size_t index)
	{
		assert(index < size());
		return m_begin[index];
	}

	void MemorySource::drop(std::size_t n)
	{
		const auto rest = size();
		const auto dropped = std::min(rest, n);
		std::advance(m_begin, dropped);
	}

	const char *MemorySource::data()
	{
		return m_begin;
	}

	bool MemorySource::isStable() const
	{
		return true;
	}
}
