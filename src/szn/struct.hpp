#ifndef SERIALIZATION_STRUCT_HPP_86B3FCE9_D690_407C_AC98_3ED93568B47E
#define SERIALIZATION_STRUCT_HPP_86B3FCE9_D690_407C_AC98_3ED93568B47E


#include <rxn/struct.hpp>
#include <szn/util.hpp>


namespace szn
{
	//detail for internal use inside of the library
	namespace detail
	{
		/// Serializes the fields it visits to a given sink.
		template <class Sink, class Structure>
		struct serializing_field_visitor SZN_FINAL
		{
			explicit serializing_field_visitor(Sink &sink,
											   Structure const &object)
				: m_sink(sink)
				, m_object(object)
			{
			}

			template <class Value, class Format>
			void visit_field(Value Structure::*value,
			                 const Format &format,
			                 const char * /*name*/) const
			{
				serialize(m_sink, m_object.*value, format);
			}

		private:

			Sink &m_sink;
			Structure const &m_object;
		};

		/// Deserializes the fields it visits from a given source.
		template <class Source, class Structure>
		struct deserializing_field_visitor SZN_FINAL
		{
			explicit deserializing_field_visitor(Source &source,
											     Structure &object)
				: m_source(source)
				, m_object(object)
			{
			}

			template <class Value, class Format>
			void visit_field(Value Structure::*value,
			                 const Format &format,
			                 const char * /*name*/) const
			{
				deserialize(m_source,
							m_object.*value,
							format);
			}

		private:

			Source &m_source;
			Structure &m_object;
		};
	}


#define SZN_BEGIN(name_) \
struct name_ SZN_FINAL { \
	template <class Sink> \
	void serialize(Sink &sink) const { \
		const ::szn::detail::serializing_field_visitor<Sink, name_> visitor(sink, *this); \
		iterate(visitor); \
	} \
	template <class Source> \
	void deserialize(Source &source) { \
		const ::szn::detail::deserializing_field_visitor<Source, name_> visitor(source, *this); \
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
		template <class Sink, class Value>
		void serialize(Sink &sink, const Value &value) const
		{
			const detail::serializing_field_visitor<Sink, Value> visitor(sink, value);
			Value::iterate(visitor);
		}

		template <class Source, class Value>
		void deserialize(Source &source, Value &value) const
		{
			const detail::deserializing_field_visitor<Source, Value> visitor(source, value);
			Value::iterate(visitor);
		}
	};
}


#endif
