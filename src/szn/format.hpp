#ifndef SERIALIZATION_FORMAT_HPP_051B02A8_C07D_41BC_8E29_F57D4067385E
#define SERIALIZATION_FORMAT_HPP_051B02A8_C07D_41BC_8E29_F57D4067385E


#include <boost/utility/declval.hpp>
#include <boost/integer/static_min_max.hpp>
#include <boost/static_assert.hpp>


namespace szn
{
#define SZN_DEFINE_HAS_MEMBER_TRAIT(trait_name, member_name, member_type) \
	template <class T> \
	struct trait_name \
	{ \
	private: \
	 \
		typedef char yes[1]; \
		typedef char no[2]; \
		 \
		template <class U> \
		static yes &has_member(member_type *, size_t SFINAE = sizeof(U::member_name)); \
		 \
		template <class U> \
		static no &has_member(void const *); \
		 \
	public: \
		enum \
		{ \
			value = (sizeof(has_member<T>(boost::declval<member_type *>())) == sizeof(yes)) \
		}; \
	};

	SZN_DEFINE_HAS_MEMBER_TRAIT(has_min_size, min_size, length_type)

	template <class Format, bool HasMinSize = has_min_size<Format>::value>
	struct min_size;

	template <class Format>
	struct min_size<Format, true> : boost::integral_constant<length_type, Format::min_size>
	{
	};

	template <class Format>
	struct min_size<Format, false> : boost::integral_constant<length_type, 0>
	{
	};


	SZN_DEFINE_HAS_MEMBER_TRAIT(has_max_size, max_size, length_type)

	template <class Format, bool HasMaxSize = has_max_size<Format>::value>
	struct max_size;

	template <class Format>
	struct max_size<Format, true> : boost::integral_constant<length_type, Format::max_size>
	{
	};

	template <class Format>
	struct max_size<Format, false> : boost::integral_constant<length_type, 0>
	{
	};


	template <length_type First, length_type Second>
	struct add_lengths : boost::integral_constant<
		length_type,
		First + boost::static_unsigned_min<Second, static_cast<length_type>(-1) - First>::value>
	{
	};

	BOOST_STATIC_ASSERT((add_lengths<0, 0>::value == 0));
	BOOST_STATIC_ASSERT((add_lengths<1, 0>::value == 1));
	BOOST_STATIC_ASSERT((add_lengths<0, 1>::value == 1));
	BOOST_STATIC_ASSERT((add_lengths<static_cast<length_type>(-1), 0>::value == static_cast<length_type>(-1)));
	BOOST_STATIC_ASSERT((add_lengths<static_cast<length_type>(-1), 1>::value == static_cast<length_type>(-1)));
	BOOST_STATIC_ASSERT((add_lengths<static_cast<length_type>(-1) - 10, 8>::value == (static_cast<length_type>(-1) - 2)));
	BOOST_STATIC_ASSERT((add_lengths<static_cast<length_type>(-1) - 10, 10>::value == static_cast<length_type>(-1)));
	BOOST_STATIC_ASSERT((add_lengths<static_cast<length_type>(-1) - 10, 12>::value == static_cast<length_type>(-1)));
}


#endif
