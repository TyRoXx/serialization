#ifndef SERIALIZATION_UNIQUE_PTR_HPP_15AF5B10_28F9_492D_ACFC_9C5EDD15A843
#define SERIALIZATION_UNIQUE_PTR_HPP_15AF5B10_28F9_492D_ACFC_9C5EDD15A843


#include <szn/util.hpp>
#include <szn/format.hpp>
#include <memory>


namespace szn
{
#if SZN_HAS_UNIQUE_PTR
	template <class PointeeFormat>
	struct unique_ptr
	{
		static std::size_t const min_size = min_size<PointeeFormat>::value;
		static std::size_t const max_size = max_size<PointeeFormat>::value;

		typedef std::unique_ptr<typename PointeeFormat::default_type> default_type;

		template <class Sink, class Pointee, class Deleter>
		void serialize(Sink &sink, const std::unique_ptr<Pointee, Deleter> &p) const
		{
			using szn::serialize;
			serialize(sink, *p, PointeeFormat());
		}

		template <class Source, class Pointee, class Deleter>
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
#endif
}


#endif
