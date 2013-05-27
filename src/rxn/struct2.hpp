#ifndef REFLEXION_STRUCT2_HPP
#define REFLEXION_STRUCT2_HPP


#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>


#define RXN_GENERATE_FIELDS(r, fields, generator) generator(fields)

#define RXN_REFLECT(generators, ...) \
	BOOST_PP_SEQ_FOR_EACH(RXN_GENERATE_FIELDS, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__), generators)

#define RXN_IDENTITY(...) __VA_ARGS__

#define RXN_HEAD(head, ...) head
#define RXN_TAIL(head, ...) __VA_ARGS__

#define RXN_IGNORE(...)

#define RXN_ADD_PARENS(...) ((__VA_ARGS__))
#define RXN_NORMALIZE_FIELD(field) RXN_ADD_PARENS field

#define RXN_GENERATE_FIELD_3(generator, name_type_tuple, annotations) \
	generator( \
		BOOST_PP_TUPLE_ELEM(2, 0, name_type_tuple), \
		(RXN_TAIL name_type_tuple), \
		annotations \
		)

#define RXN_GENERATE_FIELD_2(generator, normalized_field) \
	RXN_GENERATE_FIELD_3( \
		generator, \
		BOOST_PP_SEQ_HEAD(normalized_field), \
		BOOST_PP_SEQ_TAIL(normalized_field))

#define RXN_GENERATE_FIELD(r, generator, i, field) \
	RXN_GENERATE_FIELD_2(generator, RXN_NORMALIZE_FIELD(field))

#define RXN_MEMBER(name, type, annotations) RXN_IDENTITY type name;
#define RXN_MEMBERS(fields) BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, RXN_MEMBER, fields)

#define RXN_VISIT(name, type, annotations) \
	visitor.template accept<BOOST_PP_SEQ_ELEM(0, annotations)> \
						   (this->name);

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
