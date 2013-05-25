#include "struct.h"


namespace szn
{
	namespace detail
	{
		SerializingFieldVisitor::SerializingFieldVisitor(Sink &sink)
			: m_sink(sink)
		{
		}

		DeserializingFieldVisitor::DeserializingFieldVisitor(Source &source)
			: m_source(source)
		{
		}
	}
}
