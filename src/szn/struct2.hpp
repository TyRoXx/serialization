#ifndef SERIALIZATION_STRUCT2_HPP
#define SERIALIZATION_STRUCT2_HPP


#include <szn/sink.hpp>
#include <szn/source.hpp>
#include <rxn/struct2.hpp>


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
	struct MaxSize : std::integral_constant<std::size_t, ~static_cast<std::size_t>(0)>
	{
	};

	template <class Format>
	struct MaxSize<Format, typename std::enable_if<std::is_scalar<decltype(Format::maxSize)>::value, void>::type>
		: std::integral_constant<std::size_t, Format::maxSize>
	{
	};
}


#endif
