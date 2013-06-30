#ifndef SERIALIZATION_STRUCT_HPP_86B3FCE9_D690_407C_AC98_3ED93568B47E
#define SERIALIZATION_STRUCT_HPP_86B3FCE9_D690_407C_AC98_3ED93568B47E


#include <szn/sink.hpp>
#include <szn/source.hpp>
#include <rxn/struct.hpp>


namespace szn
{
	//detail for internal use inside of the library
	namespace detail
	{
		/// Serializes the fields it visits to a given sink.
		template <class Structure>
		struct serializing_field_visitor SZN_FINAL
		{
			explicit serializing_field_visitor(sink &sink,
											   Structure const &object)
				: m_sink(sink)
				, m_object(object)
			{
			}

			template <class Value, class Format>
			void visitField(Value Structure::*value,
							const Format &format,
							const char * /*name*/) const
			{
				serialize(m_sink, m_object.*value, format);
			}

		private:

			sink &m_sink;
			Structure const &m_object;
		};

		/// Deserializes the fields it visits from a given source.
		template <class Structure>
		struct deserializing_field_visitor SZN_FINAL
		{
			explicit deserializing_field_visitor(source &source,
											     Structure &object)
				: m_source(source)
				, m_object(object)
			{
			}

			template <class Value, class Format>
			void visitField(Value Structure::*value,
							const Format &format,
							const char * /*name*/) const
			{
				deserialize(m_source,
							m_object.*value,
							format);
			}

		private:

			source &m_source;
			Structure &m_object;
		};
	}


#define SZN_BEGIN(name_) \
struct name_ SZN_FINAL { \
	void serialize(::szn::sink &sink) const { \
		const ::szn::detail::serializing_field_visitor<name_> visitor(sink, *this); \
		iterate(visitor); \
	} \
	void deserialize(::szn::source &source) { \
		const ::szn::detail::deserializing_field_visitor<name_> visitor(source, *this); \
		iterate(visitor); \
	} \
	RXN_BEGIN(name_)


#define SZN_BEGIN2(name_) \
struct name_ { \
	RXN_BEGIN(name_)


#define SZN_FIELD RXN_FIELD


#define SZN_END() \
	RXN_END() \
};


	struct structure SZN_FINAL
	{
		template <class Value>
		void serialize(sink &sink, const Value &value) const
		{
			const detail::serializing_field_visitor<Value> visitor(sink, value);
			Value::iterate(visitor);
		}

		template <class Value>
		void deserialize(source &source, Value &value) const
		{
			const detail::deserializing_field_visitor<Value> visitor(source, value);
			Value::iterate(visitor);
		}
	};
}


#endif
