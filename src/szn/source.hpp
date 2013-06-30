#ifndef SERIALIZATION_SOURCE_HPP_F36EFB7C_FC73_4CA5_AE0D_43EBB4E699F7
#define SERIALIZATION_SOURCE_HPP_F36EFB7C_FC73_4CA5_AE0D_43EBB4E699F7


#include <szn/util.hpp>
#include <algorithm>
#include <stdexcept>
#include <boost/range/iterator_range.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/typeof/typeof.hpp>


namespace szn
{
	struct source
	{
		virtual ~source();
		virtual void load(std::size_t n) = 0;
		virtual std::size_t size() = 0;
		virtual char get(std::size_t index) = 0;
		virtual void drop(std::size_t n) = 0;
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
		 * @brief isStable
		 * @return whether the memory pointed to by data() remains the same
		 *         during the source's lifetime
		 */
		virtual bool isStable() const = 0;
	};

	namespace detail
	{
		template <class To, class From>
		To castIterator(From const &from)
		{
			return from;
		}

		template <class To, class From>
		To castIterator(From const *from)
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
			m_range = range_type(detail::castIterator<const_iterator>(boost::begin(range)),
								 detail::castIterator<const_iterator>(boost::end(range)));
		}

		virtual void load(std::size_t n) SZN_OVERRIDE
		{
			(void)n;
		}

		virtual std::size_t size() SZN_OVERRIDE
		{
			return static_cast<std::size_t>(
						std::distance(boost::begin(m_range), boost::end(m_range)) - m_position);
		}

		virtual char get(std::size_t index) SZN_OVERRIDE
		{
			assert(index < size());
			return boost::next(boost::begin(m_range), m_position)[index];
		}

		virtual void drop(std::size_t n) SZN_OVERRIDE
		{
			assert(n <= size());
			m_position += static_cast<difference_type>(n);
		}

		virtual const char *data() SZN_OVERRIDE
		{
			return reinterpret_cast<const char *>(
						&*(boost::begin(m_range)) + m_position);
		}

		virtual bool isStable() const SZN_OVERRIDE
		{
			return true;
		}

	private:

		range_type m_range;
		difference_type m_position;
	};

	template <class Range>
	range_source<Range> makeRangeSource(Range const &range)
	{
		return range_source<Range>(range);
	}

	typedef range_source<boost::iterator_range<const char *> > MemorySource;


	struct empty_source : source
	{
		virtual void load(std::size_t n) SZN_OVERRIDE;
		virtual std::size_t size() SZN_OVERRIDE;
		virtual char get(std::size_t index) SZN_OVERRIDE;
		virtual void drop(std::size_t n) SZN_OVERRIDE;
		virtual const char *data() SZN_OVERRIDE;
	};
}

BOOST_TYPEOF_REGISTER_TEMPLATE(::szn::range_source, 2)


#endif
