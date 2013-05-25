#ifndef SERIALIZATION_STRUCT_H
#define SERIALIZATION_STRUCT_H


#include "sink.h"
#include "source.h"


namespace szn
{
	//detail for internal use inside of the library
	namespace detail
	{
		/// Serializes the fields it visits to a given sink.
		struct SerializingFieldVisitor SZN_FINAL
		{
			explicit SerializingFieldVisitor(Sink &sink);

			template <class Value, class Format>
			void visitField(const Value &value,
							const Format &format,
							const char * /*name*/) const
			{
				serialize(m_sink, value, format);
			}

		private:

			Sink &m_sink;
		};

		/// Deserializes the fields it visits from a given source.
		struct DeserializingFieldVisitor SZN_FINAL
		{
			explicit DeserializingFieldVisitor(Source &source);

			template <class Value, class Format>
			void visitField(Value &value,
							const Format &format,
							const char * /*name*/) const
			{
				//Because iterate() gives us only const-references to
				//the structure's elements we have to cast the const away.
				typedef typename std::remove_const<Value>::type MutableValue;
				deserialize(m_source,
							const_cast<MutableValue &>(value),
							format);
			}

		private:

			Source &m_source;
		};
	}

#define SZN_BEGIN(name_) \
struct name_ SZN_FINAL { \
	void serialize(::szn::Sink &sink) const { \
		const ::szn::detail::SerializingFieldVisitor visitor(sink); \
		this->iterate(visitor); \
	} \
	void deserialize(::szn::Source &source) { \
		const ::szn::detail::DeserializingFieldVisitor visitor(source); \
		this->iterate(visitor); \
	} \
	template <class Visitor> \
	void iterate(Visitor &visitor) const { \
		(void)visitor; //avoid warning when the structure has no fields

#define SZN_BEGIN2(name_) \
struct name_ { \
	template <class Visitor> \
	void iterate(Visitor &visitor) const {

#define SZN_FIELD(name_, type_, format_) \
		this->_internal_iterate_ ## name_ (visitor); \
	} \
public: \
	type_ name_; \
private: \
	template <class Visitor> \
	void _internal_iterate_ ## name_(Visitor &visitor) const { \
		visitor.visitField(this->name_, format_(), #name_ );

#define SZN_END() \
	} \
};

	struct Struct SZN_FINAL
	{
		template <class Value>
		void serialize(Sink &sink, const Value &value) const
		{
			const detail::SerializingFieldVisitor visitor(sink);
			value.iterate(visitor);
		}

		template <class Value>
		void deserialize(Source &source, Value &value) const
		{
			const detail::DeserializingFieldVisitor visitor(source);
			value.iterate(visitor);
		}
	};
}


#endif
