#ifndef SERIALIZATION_SINK_H
#define SERIALIZATION_SINK_H


#include "util.h"
#include <algorithm>


namespace szn
{
	struct Sink
	{
		virtual ~Sink();
		virtual void write(const char *data, std::size_t n) = 0;
	};

	/// does nothing when being written to
	struct NullSink : Sink
	{
		virtual void write(const char *data, std::size_t n) SZN_OVERRIDE;
	};

	/// copies the data byte-wise to an output iterator.
	/// Works with std::ostreambuf_iterator<char>, std::back_inserter etc.
	template <class OutputIterator>
	struct IteratorSink : Sink
	{
		template <class T>
		explicit IteratorSink(T &&begin)
			: m_pos(std::forward<T>(begin))
		{
		}

		virtual void write(const char *data, std::size_t n) SZN_OVERRIDE
		{
			std::copy(data, data + n, m_pos);
		}

	private:

		OutputIterator m_pos;
	};

	/// creates a sink from an output iterator for chars
	template <class OutputIterator>
	IteratorSink<OutputIterator> makeIteratorSink(OutputIterator begin)
	{
		return IteratorSink<OutputIterator>(std::move(begin));
	}

	template <class Byte,
			  class Allocator>
	auto makeContainerSink(std::vector<Byte, Allocator> &destination,
						   typename std::enable_if<sizeof(Byte) == 1, void>::type * = nullptr)
		-> IteratorSink<decltype(std::back_inserter(destination))>
	{
		return makeIteratorSink(std::back_inserter(destination));
	}

	template <class Byte,
			  class Traits,
			  class Allocator>
	auto makeContainerSink(std::basic_string<Byte, Traits, Allocator> &destination,
						   typename std::enable_if<sizeof(Byte) == 1, void>::type * = nullptr)
		-> IteratorSink<decltype(std::back_inserter(destination))>
	{
		return makeIteratorSink(std::back_inserter(destination));
	}
}


#endif
