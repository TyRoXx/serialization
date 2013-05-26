#ifndef REFLEXION_STRUCT2_HPP
#define REFLEXION_STRUCT2_HPP


#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>


#define RXN_FIELD(rxn_type, rxn_name) rxn_type rxn_name;

#define RXN_IGNORE(...)

#define RXN_GET_FIRST_2(...) __VA_ARGS__ RXN_IGNORE(
#define RXN_GET_FIRST(a) RXN_GET_FIRST_2 a)

#define RXN_GET_SECOND(a) RXN_IGNORE a

#define RXN_PARSE_FIELD(r, data, i, e) \
	RXN_FIELD(RXN_GET_FIRST(e), RXN_GET_SECOND(e))

#define RXN_FIELDS(...) \
	BOOST_PP_SEQ_FOR_EACH_I( \
		RXN_PARSE_FIELD, _, \
		BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define RXN_REFLECT(rxn_struct_name) RXN_FIELDS


#endif
