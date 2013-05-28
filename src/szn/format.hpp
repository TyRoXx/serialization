#ifndef SERIALIZATION_FORMAT_HPP
#define SERIALIZATION_FORMAT_HPP


#include <type_traits>


namespace szn
{
	template <class Format, class SFINAE = void>
	struct MinSize : std::integral_constant<std::size_t, 0>
	{
	};

	template <class Format>
	struct MinSize<Format, typename std::enable_if<std::is_scalar<decltype(Format::minSize)>::value, void>::type>
		: std::integral_constant<std::size_t, Format::minSize>
	{
	};


	template <class Format, class SFINAE = void>
	struct MaxSize : std::integral_constant<std::size_t,
			~static_cast<std::size_t>(0)>
	//      not using numeric_limits here because max() is not
	//      a compile-time constant for every compiler
	{
	};

	template <class Format>
	struct MaxSize<Format, typename std::enable_if<std::is_scalar<decltype(Format::maxSize)>::value, void>::type>
		: std::integral_constant<std::size_t, Format::maxSize>
	{
	};
}


#endif
