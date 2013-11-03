#ifndef SERIALIZATION_STRUCT3_HPP_EBB9B9F7_02FB_4C22_958B_86E566E6B48F
#define SERIALIZATION_STRUCT3_HPP_EBB9B9F7_02FB_4C22_958B_86E566E6B48F


#include <rxn/reflect.hpp>


namespace szn
{
	namespace detail
	{
		template <class Format, class AnnotatedType>
		struct auto_member_type
		{
			typedef typename Format::default_type type;
		};

		template <class Format, class Ignored, class AnnotatedType>
		struct auto_member_type<Format, Ignored (AnnotatedType)>
		{
			typedef AnnotatedType type;
		};
	}
}
#define SZN_AUTO_MEMBER(name, format, annotations) \
	::szn::detail::auto_member_type<RXN_IDENTITY format, void annotations>::type name;

#define SZN_AUTO_MEMBERS(fields) BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, SZN_AUTO_MEMBER, fields)


#define SZN_ITERATE_ACCEPT_MEMBER(name, format, annotations) \
	visitor.template accept<RXN_REMOVE_PAREN(format)> \
						   (this->name);

#define SZN_ITERATE(fields) \
	template <class Visitor> \
	void iterate(Visitor &visitor) { \
		BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, SZN_ITERATE_ACCEPT_MEMBER, fields) \
	} \
	template <class Visitor> \
	void iterate(Visitor &visitor) const { \
		BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, SZN_ITERATE_ACCEPT_MEMBER, fields) \
	}

#define SZN_STRUCTURE(...) RXN_REFLECT((SZN_AUTO_MEMBERS) (SZN_ITERATE), __VA_ARGS__)


namespace szn
{
	namespace detail
	{
		template <class Sink>
		struct serializing_structure3_visitor
		{
			explicit serializing_structure3_visitor(Sink &sink)
			    : m_sink(sink)
			{
			}

			template <class Format, class Element>
			void accept(Element const &element) const
			{
				Format().serialize(m_sink, element);
			}

		private:

			Sink &m_sink;
		};

		template <class Source>
		struct deserializing_structure3_visitor
		{
			explicit deserializing_structure3_visitor(Source &source)
			    : m_source(source)
			{
			}

			template <class Format, class Element>
			void accept(Element &element) const
			{
				Format().deserialize(m_source, element);
			}

		private:

			Source &m_source;
		};
	}

	struct any_structure
	{
		template <class Sink, class Structure>
		void serialize(Sink &sink, Structure const &object) const
		{
			detail::serializing_structure3_visitor<Sink> const visitor(sink);
			object.iterate(visitor);
		}

		template <class Source, class Structure>
		void deserialize(Source &source, Structure &object) const
		{
			detail::deserializing_structure3_visitor<Source> const visitor(source);
			object.iterate(visitor);
		}
	};

	template <class Structure>
	struct structure : any_structure
	{
		typedef Structure default_type;

		template <class Sink>
		void serialize(Sink &sink, Structure const &object) const
		{
			detail::serializing_structure3_visitor<Sink> const visitor(sink);
			object.iterate(visitor);
		}

		template <class Source>
		void deserialize(Source &source, Structure &object) const
		{
			detail::deserializing_structure3_visitor<Source> const visitor(source);
			object.iterate(visitor);
		}
	};
}


#endif
