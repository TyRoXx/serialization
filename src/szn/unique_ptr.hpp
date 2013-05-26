#ifndef SERIALIZATION_UNIQUE_PTR_HPP
#define SERIALIZATION_UNIQUE_PTR_HPP


#include <szn/util.hpp>
#include <memory>


namespace szn
{
	template <class PointeeFormat>
	struct UniquePtr
	{
		template <class Pointee, class Deleter>
		void serialize(Sink &sink, const std::unique_ptr<Pointee, Deleter> &p) const
		{
			using szn::serialize;
			serialize(sink, *p, PointeeFormat());
		}

		template <class Pointee, class Deleter>
		void deserialize(Source &source, std::unique_ptr<Pointee, Deleter> &p) const
		{
			using szn::deserialize;
			if (!p)
			{
				p.reset(new Pointee); //TODO avoid default constructor
			}
			deserialize(source, *p, PointeeFormat());
		}
	};
}


#endif
