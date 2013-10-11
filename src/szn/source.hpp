#ifndef SERIALIZATION_SOURCE_HPP_F36EFB7C_FC73_4CA5_AE0D_43EBB4E699F7
#define SERIALIZATION_SOURCE_HPP_F36EFB7C_FC73_4CA5_AE0D_43EBB4E699F7


#include <szn/util.hpp>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <boost/range/iterator_range.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/typeof/typeof.hpp>


namespace szn
{
	struct source
	{
		virtual ~source();
		virtual void load(length_type n) = 0;
		virtual length_type size() = 0;
		virtual char get(length_type index) = 0;
		virtual void drop(length_type n) = 0;
		virtual const char *data() = 0;

		inline void read(char *destination, std::size_t length)
		{
			load(length);
			if (size() < length)
			{
				throw std::runtime_error("Insufficient input from source to read");
			}
			std::copy(data(), data() + length, destination);
			drop(length);
		}

		/**
		 * @brief is_stable
		 * @return whether the memory pointed to by data() remains the same
		 *         during the source's lifetime
		 */
		virtual bool is_stable() const = 0;
	};

	inline source::~source()
	{
	}

	namespace detail
	{
		template <class To, class From>
		To cast_iterator(From const &from)
		{
			return from;
		}

		template <class To, class From>
		To cast_iterator(From const *from)
		{
			return reinterpret_cast<To>(from);
		}
	}

	template <class LValueRandomAccessByteRange,
			  class PointsToLValue = typename boost::enable_if<
				  boost::is_lvalue_reference<typename std::iterator_traits<typename LValueRandomAccessByteRange::const_iterator>::reference>, void>::type
			  >
	struct range_source : source
	{
		typedef LValueRandomAccessByteRange range_type;
		typedef typename range_type::const_iterator const_iterator;
		typedef typename std::iterator_traits<const_iterator>::difference_type difference_type;

		template <class Range>
		explicit range_source(Range const &range)
			: m_position(0)
		{
			m_range = range_type(detail::cast_iterator<const_iterator>(boost::begin(range)),
								 detail::cast_iterator<const_iterator>(boost::end(range)));
		}

		virtual void load(length_type n) SZN_OVERRIDE
		{
			(void)n;
		}

		virtual length_type size() SZN_OVERRIDE
		{
			return static_cast<std::size_t>(
						std::distance(boost::begin(m_range), boost::end(m_range)) - m_position);
		}

		virtual char get(length_type index) SZN_OVERRIDE
		{
			assert(index < size());
			return boost::next(boost::begin(m_range), m_position)[static_cast<size_t>(index)];
		}

		virtual void drop(length_type n) SZN_OVERRIDE
		{
			assert(n <= size());
			m_position += static_cast<difference_type>(n);
		}

		virtual const char *data() SZN_OVERRIDE
		{
			return reinterpret_cast<const char *>(
						&*(boost::begin(m_range)) + m_position);
		}

		virtual bool is_stable() const SZN_OVERRIDE
		{
			return true;
		}

	private:

		range_type m_range;
		difference_type m_position;
	};

	template <class Range>
	range_source<Range> make_range_source(Range const &range)
	{
		return range_source<Range>(range);
	}

	template <class Container>
	range_source<Container> make_container_source(Container const &container)
	{
		return make_range_source(container);
	}

	typedef range_source<boost::iterator_range<const char *> > memory_source;


	struct empty_source : source
	{
		virtual void load(length_type n) SZN_OVERRIDE;
		virtual length_type size() SZN_OVERRIDE;
		virtual char get(length_type index) SZN_OVERRIDE;
		virtual void drop(length_type n) SZN_OVERRIDE;
		virtual const char *data() SZN_OVERRIDE;
		virtual bool is_stable() const SZN_OVERRIDE;
	};

	inline void empty_source::load(length_type)
	{
	}

	inline length_type empty_source::size()
	{
		return 0;
	}

	inline char empty_source::get(length_type)
	{
		throw std::runtime_error("szn::EmptySource cannot be read from");
	}

	inline void empty_source::drop(length_type)
	{
	}

	inline const char *empty_source::data()
	{
		return NULL;
	}

	inline bool empty_source::is_stable() const
	{
		return true;
	}

	struct zero_source : source
	{
		virtual void load(length_type n) SZN_OVERRIDE;
		virtual length_type size() SZN_OVERRIDE;
		virtual char get(length_type index) SZN_OVERRIDE;
		virtual void drop(length_type n) SZN_OVERRIDE;
		virtual const char *data() SZN_OVERRIDE;
		virtual bool is_stable() const SZN_OVERRIDE;
	};

	inline void zero_source::load(length_type n)
	{
		(void)n;
	}

	inline length_type zero_source::size()
	{
		return 1;
	}

	inline char zero_source::get(length_type index)
	{
		(void)index;
		return 0;
	}

	inline void zero_source::drop(length_type n)
	{
		(void)n;
	}

	inline const char *zero_source::data()
	{
		return "";
	}

	inline bool zero_source::is_stable() const
	{
		return true;
	}

	struct stream_source : source
	{
		explicit stream_source(std::istream &in);
		virtual void load(length_type n) SZN_OVERRIDE;
		virtual length_type size() SZN_OVERRIDE;
		virtual char get(length_type index) SZN_OVERRIDE;
		virtual void drop(length_type n) SZN_OVERRIDE;
		virtual const char *data() SZN_OVERRIDE;
		virtual bool is_stable() const SZN_OVERRIDE;

	private:

		std::istream &m_in;
		std::vector<char> m_buffer;
		length_type m_size;
	};

	inline stream_source::stream_source(std::istream &in)
		: m_in(in)
		, m_size(0)
	{
	}

	inline void stream_source::load(length_type n)
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
		m_size = (std::min)(clamped_n, m_buffer.size());
	}

	inline length_type stream_source::size()
	{
		return m_size;
	}

	inline char stream_source::get(length_type index)
	{
		assert(index < m_size);
		assert(index < m_buffer.size());
		return m_buffer[static_cast<size_t>(index)];
	}

	inline void stream_source::drop(length_type n)
	{
		assert(n <= m_size);
		assert(n <= m_buffer.size());
		m_buffer.erase(m_buffer.begin(), m_buffer.begin() + static_cast<ptrdiff_t>(n));
		m_size -= n;
	}

	inline const char *stream_source::data()
	{
		return m_buffer.empty() ? NULL : &m_buffer.front();
	}

	inline bool stream_source::is_stable() const
	{
		return false;
	}
}

BOOST_TYPEOF_REGISTER_TEMPLATE(::szn::range_source, 2)


#endif
