#ifndef REFLEXION_STRUCT2_HPP
#define REFLEXION_STRUCT2_HPP


#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>


#define RXN_FIELD(rxn_name, rxn_type, rxn_annotations) rxn_type rxn_name;

#define RXN_GET_0(a) BOOST_PP_SEQ_ELEM(0, a)
#define RXN_GET_1(a) BOOST_PP_SEQ_ELEM(1, a)
#define RXN_GET_2(a) BOOST_PP_SEQ_ELEM(2, a)

#define RXN_PARSE_FIELD(r, data, i, e) \
	RXN_FIELD(RXN_GET_0(e), RXN_GET_1(e), RXN_GET_2(e))

#define RXN_FIELDS(...) \
	BOOST_PP_SEQ_FOR_EACH_I( \
		RXN_PARSE_FIELD, _, \
		BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define RXN_REFLECT(rxn_struct_name) RXN_FIELDS


#endif
