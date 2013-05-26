#ifndef REFLEXION_STRUCT2_HPP
#define REFLEXION_STRUCT2_HPP


#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>


#define RXN_FIELD(rxn_name, rxn_type, rxn_annotations) rxn_type rxn_name;

#define RXN_PARSE_FIELD(r, data, i, e) \
	RXN_FIELD(BOOST_PP_SEQ_ELEM(0, e), \
			  BOOST_PP_SEQ_ELEM(1, e), \
			  BOOST_PP_SEQ_ELEM(2, e))

#define RXN_FIELDS(...) \
	BOOST_PP_SEQ_FOR_EACH_I( \
		RXN_PARSE_FIELD, _, \
		BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define RXN_REFLECT(rxn_struct_name) RXN_FIELDS


#endif
