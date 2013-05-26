#ifndef REFLEXION_STRUCT_HPP_DFB24FD7_8792_4FF8_811F_C5870DC80A48
#define REFLEXION_STRUCT_HPP_DFB24FD7_8792_4FF8_811F_C5870DC80A48


#define RXN_BEGIN() \
	template <class Visitor> \
	void iterate(Visitor &visitor) const { \
		(void)visitor; //avoid warning when the structure has no fields


#define RXN_FIELD(name_, type_, format_) \
		this->_internal_iterate_ ## name_ (visitor); \
	} \
public: \
	type_ name_; \
private: \
	template <class Visitor> \
	void _internal_iterate_ ## name_(Visitor &visitor) const { \
		visitor.visitField(this->name_, format_(), #name_ );


#define RXN_END() \
	}


#endif
