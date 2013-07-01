#ifndef SERIALIZATION_STRUCT3_HPP_EBB9B9F7_02FB_4C22_958B_86E566E6B48F
#define SERIALIZATION_STRUCT3_HPP_EBB9B9F7_02FB_4C22_958B_86E566E6B48F


#include <rxn/struct2.hpp>


namespace szn
{
	namespace detail
	{
		template <class Format, class AnnotatedType>
		struct auto_member_type
		{
			typedef typename Format::default_type type;
		};

		template <class Format, class Ignored, class AnnotatedType>
		struct auto_member_type<Format, Ignored (AnnotatedType)>
		{
			typedef AnnotatedType type;
		};
	}
}
#define SZN_AUTO_MEMBER(name, format, annotations) \
	::szn::detail::auto_member_type<RXN_IDENTITY format, void annotations>::type name;

#define SZN_AUTO_MEMBERS(fields) BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, SZN_AUTO_MEMBER, fields)


#define SZN_ITERATE_ACCEPT_MEMBER(name, format, annotations) \
	visitor.template accept<RXN_REMOVE_PAREN(format)> \
						   (this->name);

#define SZN_ITERATE(fields) \
	template <class Visitor> \
	void iterate(Visitor &visitor) { \
		BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, SZN_ITERATE_ACCEPT_MEMBER, fields) \
	} \
	template <class Visitor> \
	void iterate(Visitor &visitor) const { \
		BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, SZN_ITERATE_ACCEPT_MEMBER, fields) \
	}


#endif
