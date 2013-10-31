#ifndef SERIALIZATION_SINK_HPP_1FA67032_F7EB_48C8_97D7_659F27F90D72
#define SERIALIZATION_SINK_HPP_1FA67032_F7EB_48C8_97D7_659F27F90D72


#include <szn/util.hpp>
#include <algorithm>
#include <vector>
#include <string>
#include <iterator>
#include <stdexcept>
#include <boost/typeof/typeof.hpp>
#include <boost/move/move.hpp>
#include <boost/type_traits/decay.hpp>


namespace szn
{
	struct sink
	{
		virtual ~sink();
		virtual void write(const char *data, std::size_t n) = 0;
	};

	inline sink::~sink()
	{
	}

	/// does nothing when being written to
	struct null_sink : sink
	{
		virtual void write(const char *data, std::size_t n) SZN_OVERRIDE;
	};

	inline void null_sink::write(const char *data, std::size_t n)
	{
		(void)data;
		(void)n;
	}

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

		BOOST_COPYABLE_AND_MOVABLE(iterator_sink)

		OutputIterator m_pos;
	};

	template <class Container>
	struct insertion_sink : sink
	{
		insertion_sink()
		    : m_destination(NULL)
		{
		}

		explicit insertion_sink(Container &destination)
		    : m_destination(&destination)
		{
		}

		virtual void write(const char *data, std::size_t n) SZN_OVERRIDE
		{
			m_destination->insert(m_destination->end(), data, data + n);
		}

	private:

		Container *m_destination;
	};

	/// creates a sink from an output iterator for chars
	template <class OutputIterator>
	iterator_sink<OutputIterator> make_iterator_sink(OutputIterator begin)
	{
		return iterator_sink<OutputIterator>(boost::move(begin));
	}

	template <class Byte, class Allocator>
	insertion_sink<std::vector<Byte, Allocator> >
	make_container_sink(std::vector<Byte, Allocator> &destination,
	                    typename boost::enable_if_c<sizeof(Byte) == 1, void>::type * = NULL)
	{
		return insertion_sink<std::vector<Byte, Allocator> >(destination);
	}

	template <class Byte, class Traits, class Allocator>
	iterator_sink<std::back_insert_iterator<std::basic_string<Byte, Traits, Allocator> > >
	make_container_sink(std::basic_string<Byte, Traits, Allocator> &destination,
	                    typename boost::enable_if_c<sizeof(Byte) == 1, void>::type * = NULL)
	{
		return make_iterator_sink(std::back_inserter(destination));
	}

	struct counting_sink : sink
	{
		counting_sink()
			: m_count(0)
		{
		}

		length_type count() const
		{
			return m_count;
		}

		virtual void write(const char *data, std::size_t n) SZN_OVERRIDE
		{
			(void)data;
			m_count += n;
		}

	private:

		length_type m_count;
	};

	template <class NextSink = sink>
	struct counting_sink_adaptor : counting_sink
	{
		explicit counting_sink_adaptor(NextSink &next)
			: m_next(next)
		{
		}

		virtual void write(const char *data, std::size_t n) SZN_OVERRIDE
		{
			m_next.write(data, n);
			counting_sink::write(data, n);
		}

	private:

		NextSink &m_next;
	};

	struct stream_sink SZN_FINAL : sink
	{
		explicit stream_sink(std::ostream &out)
			: m_out(out)
		{
		}

		virtual void write(const char *data, std::size_t n) SZN_OVERRIDE
		{
			m_out.write(data, n);
		}

	private:

		std::ostream &m_out;
	};

	struct failing_sink SZN_FINAL : sink
	{
		virtual void write(const char *data, std::size_t n) SZN_OVERRIDE
		{
			(void)data;
			(void)n;
			throw std::logic_error("You cannot write to an szn::failing_sink");
		}
	};

	template <class WriteFunction>
	struct function_sink SZN_FINAL : sink
	{
		function_sink()
		    : m_function(WriteFunction())
		{
		}

		template <class F>
		function_sink(F &&function)
		    : m_function(std::forward<F>(function))
		{
		}

		virtual void write(const char *data, std::size_t n) SZN_OVERRIDE
		{
			return m_function(data, n);
		}

	private:

		WriteFunction m_function;
	};

	template <class WriteFunction>
	function_sink<typename boost::decay<WriteFunction>::type> make_function_sink(WriteFunction &&write)
	{
		return function_sink<typename boost::decay<WriteFunction>::type>(std::forward<WriteFunction>(write));
	}
}

BOOST_TYPEOF_REGISTER_TEMPLATE(::szn::iterator_sink, 1)


#endif
