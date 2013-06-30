#ifndef SERIALIZATION_STRUCT3_HPP
#define SERIALIZATION_STRUCT3_HPP


#include <rxn/struct2.hpp>


namespace szn
{
	namespace detail
	{
		template <class Format, class AnnotatedType>
		struct AutoMemberType
		{
			typedef typename Format::default_type type;
		};

		template <class Format, class Ignored, class AnnotatedType>
		struct AutoMemberType<Format, Ignored (AnnotatedType)>
		{
			typedef AnnotatedType type;
		};
	}
}
#define SZN_AUTO_MEMBER(name, format, annotations) \
	::szn::detail::AutoMemberType<RXN_IDENTITY format, void annotations>::type name;

#define SZN_AUTO_MEMBERS(fields) BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, SZN_AUTO_MEMBER, fields)


#endif
