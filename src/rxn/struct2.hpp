#ifndef REFLEXION_STRUCT2_HPP
#define REFLEXION_STRUCT2_HPP


#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>


#define RXN_GENERATE_FIELDS(r, fields, generator) generator(fields)

#define RXN_REFLECT(generators, ...) \
	BOOST_PP_SEQ_FOR_EACH(RXN_GENERATE_FIELDS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__), generators)

#define RXN_GENERATE_FIELD(r, generator, i, field) generator(field)


#define RXN_MEMBER(field) BOOST_PP_SEQ_ELEM(1, field) BOOST_PP_SEQ_ELEM(0, field);
#define RXN_MEMBERS(fields) BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, RXN_MEMBER, fields)

#define RXN_VISIT(field) visitor.template accept<BOOST_PP_SEQ_ELEM(2, field)> \
												(this->BOOST_PP_SEQ_ELEM(0, field));

#define RXN_ITERATE(fields) \
	template <class Visitor> \
	void iterate(Visitor &visitor) { \
		BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, RXN_VISIT, fields) \
	} \
	template <class Visitor> \
	void iterate(Visitor &visitor) const { \
		BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, RXN_VISIT, fields) \
	}


#endif
