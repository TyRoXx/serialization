#ifndef SERIALIZATION_SINK_HPP_1FA67032_F7EB_48C8_97D7_659F27F90D72
#define SERIALIZATION_SINK_HPP_1FA67032_F7EB_48C8_97D7_659F27F90D72


#include <szn/util.hpp>
#include <algorithm>
#include <vector>
#include <string>
#include <iterator>
#include <boost/typeof/typeof.hpp>
#include <boost/move/move.hpp>


namespace szn
{
	struct sink
	{
		virtual ~sink();
		virtual void write(const char *data, std::size_t n) = 0;
	};

	/// does nothing when being written to
	struct null_sink : sink
	{
		virtual void write(const char *data, std::size_t n) SZN_OVERRIDE;
	};

	/// copies the data byte-wise to an output iterator.
	/// Works with std::ostreambuf_iterator<char>, std::back_inserter etc.
	template <class OutputIterator>
	struct iterator_sink : sink
	{
		template <class T>
		explicit iterator_sink(BOOST_FWD_REF(T) begin)
			: m_pos(boost::forward<T>(begin))
		{
		}

		virtual void write(const char *data, std::size_t n) SZN_OVERRIDE
		{
			std::copy(data, data + n, m_pos);
		}

	private:

		BOOST_COPYABLE_AND_MOVABLE(IteratorSink)

		OutputIterator m_pos;
	};

	/// creates a sink from an output iterator for chars
	template <class OutputIterator>
	iterator_sink<OutputIterator> makeIteratorSink(OutputIterator begin)
	{
		return iterator_sink<OutputIterator>(boost::move(begin));
	}

	template <class Byte,
			  class Allocator>
	iterator_sink<std::back_insert_iterator<std::vector<Byte, Allocator> > >
	makeContainerSink(std::vector<Byte, Allocator> &destination,
					  typename boost::enable_if_c<sizeof(Byte) == 1, void>::type * = NULL)
	{
		return makeIteratorSink(std::back_inserter(destination));
	}

	template <class Byte,
			  class Traits,
			  class Allocator>
	iterator_sink<std::back_insert_iterator<std::basic_string<Byte, Traits, Allocator> > >
	makeContainerSink(std::basic_string<Byte, Traits, Allocator> &destination,
					  typename boost::enable_if_c<sizeof(Byte) == 1, void>::type * = NULL)
	{
		return makeIteratorSink(std::back_inserter(destination));
	}
}

BOOST_TYPEOF_REGISTER_TEMPLATE(::szn::iterator_sink, 1)


#endif
