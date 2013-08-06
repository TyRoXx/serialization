#ifndef SERIALIZATION_ITERATOR_HPP_
#define SERIALIZATION_ITERATOR_HPP_


#include <boost/iterator/iterator_facade.hpp>


namespace szn
{
	template <class ElementFormat, class Element, class Source>
	struct deserializing_iterator : boost::iterator_facade<
			deserializing_iterator<ElementFormat, Element, Source>,
			Element,
			boost::forward_traversal_tag
			>
	{
		deserializing_iterator()
			: m_source(NULL)
		{
		}

		explicit deserializing_iterator(Source &source)
			: m_source(&source)
		{
			increment();
		}

	private:

		Source *m_source;
		mutable Element m_current;

		friend class boost::iterator_core_access;

		void increment()
		{
			assert(m_source);
			ElementFormat().deserialize(*m_source, m_current);
		}

		bool equal(deserializing_iterator const &) const
		{
			return false;
		}

		Element &dereference() const
		{
			return m_current;
		}
	};

	template <class ElementFormat, class Element, class Source>
	deserializing_iterator<ElementFormat, Element, Source>
	make_deserializing_iterator(Source &source)
	{
		return deserializing_iterator<ElementFormat, Element, Source>(source);
	}
}


#endif
