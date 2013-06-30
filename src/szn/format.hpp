#ifndef SERIALIZATION_FORMAT_HPP
#define SERIALIZATION_FORMAT_HPP


#include <boost/utility/declval.hpp>


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
		static yes &has_member(SZN_DECLTYPE(U::member_name) *); \
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

	SZN_DEFINE_HAS_MEMBER_TRAIT(has_min_size, min_size, std::size_t)

	template <class Format, bool HasMinSize = has_min_size<Format>::value>
	struct min_size;

	template <class Format>
	struct min_size<Format, true> : boost::integral_constant<std::size_t, Format::min_size>
	{
	};

	template <class Format>
	struct min_size<Format, false> : boost::integral_constant<std::size_t, 0>
	{
	};


	SZN_DEFINE_HAS_MEMBER_TRAIT(has_max_size, max_size, std::size_t)

	template <class Format, bool HasMaxSize = has_max_size<Format>::value>
	struct max_size;

	template <class Format>
	struct max_size<Format, true> : boost::integral_constant<std::size_t, Format::max_size>
	{
	};

	template <class Format>
	struct max_size<Format, false> : boost::integral_constant<std::size_t, 0>
	{
	};
}


#endif
