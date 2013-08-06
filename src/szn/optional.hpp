#ifndef SERIALIZATION_OPTIONAL_HPP_
#define SERIALIZATION_OPTIONAL_HPP_


#include <szn/bool.hpp>
#include <szn/format.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <memory>


namespace szn
{
	template <class ConditionFormat, class ValueFormat>
	struct optional
	{
		typedef boost::optional<typename ValueFormat::default_type> default_type;

		static length_type const min_size = add_lengths<min_size<ConditionFormat>::value, min_size<ValueFormat>::value>::value;
		static length_type const max_size = add_lengths<max_size<ConditionFormat>::value, max_size<ValueFormat>::value>::value;

		template <class Sink, class Nullable>
		void serialize(Sink &sink, Nullable const &value) const
		{
			bool const is_set = !!value;
			serialize_indirect(sink, is_set, value);
		}

		template <class Sink, class Value>
		void serialize(Sink &sink, std::auto_ptr<Value> const &ptr) const
		{
			bool const is_set = ptr.get() != 0;
			serialize_indirect(sink, is_set, ptr);
		}

		template <class Source, class Value>
		void deserialize(Source &source, boost::optional<Value> &value) const
		{
			assert(!value);
			if (parse_condition(source))
			{
				Value temporary;
				ValueFormat().deserialize(source, temporary);
				value = boost::move(temporary);
			}
		}

		template <class Source, class Value>
		void deserialize(Source &source, boost::scoped_ptr<Value> &ptr) const
		{
			return deserialize_auto_ptr_like<Value>(source, ptr);
		}

		template <class Source, class Value>
		void deserialize(Source &source, boost::shared_ptr<Value> &ptr) const
		{
			assert(!ptr.get());
			if (parse_condition(source))
			{
				ptr = boost::make_shared<Value>();
				ValueFormat().deserialize(source, *ptr);
			}
		}

		template <class Source, class Value>
		void deserialize(Source &source, std::auto_ptr<Value> &ptr) const
		{
			return deserialize_auto_ptr_like<Value>(source, ptr);
		}

#if SZN_HAS_UNIQUE_PTR
		template <class Source, class Value>
		void deserialize(Source &source, std::unique_ptr<Value> &ptr) const
		{
			return deserialize_auto_ptr_like<Value>(source, ptr);
		}
#endif

#if SZN_HAS_SHARED_PTR
		template <class Source, class Value>
		void deserialize(Source &source, std::shared_ptr<Value> &ptr) const
		{
			assert(!ptr.get());
			if (parse_condition(source))
			{
				ptr = std::make_shared<Value>();
				ValueFormat().deserialize(source, *ptr);
			}
		}
#endif

	private:

		template <class Sink, class Dereferenceable>
		void serialize_indirect(Sink &sink, bool is_set, Dereferenceable const &deref) const
		{
			ConditionFormat().serialize(sink, is_set);
			if (is_set)
			{
				ValueFormat().serialize(sink, *deref);
			}
		}

		template <class Source>
		bool parse_condition(Source &source) const
		{
			bool is_set = false;
			ConditionFormat().deserialize(source, is_set);
			return is_set;
		}

		template <class Value, class Source, class Ptr>
		void deserialize_auto_ptr_like(Source &source, Ptr &ptr) const
		{
			assert(!ptr.get());
			if (parse_condition(source))
			{
				ptr.reset(new Value);
				ValueFormat().deserialize(source, *ptr);
			}
		}
	};
}


#endif
