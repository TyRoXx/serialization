#ifndef REFLEXION_STRUCT_HPP_DFB24FD7_8792_4FF8_811F_C5870DC80A48
#define REFLEXION_STRUCT_HPP_DFB24FD7_8792_4FF8_811F_C5870DC80A48


#define RXN_BEGIN(struct_name_) \
private: typedef struct_name_ rxn_this_type; \
public: \
	template <class Visitor> \
	static void iterate(Visitor &visitor) { \
		(void)visitor; //avoid warning when the structure has no fields


#define RXN_FIELD(name_, type_, format_) \
		_internal_iterate_ ## name_ (visitor); \
	} \
public: \
	type_ name_; \
private: \
	template <class Visitor> \
	static void _internal_iterate_ ## name_(Visitor &visitor) { \
		visitor.visitField(&rxn_this_type::name_, format_(), #name_ );


#define RXN_END() \
	}


#endif