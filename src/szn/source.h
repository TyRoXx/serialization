#ifndef SERIALIZATION_SOURCE_H
#define SERIALIZATION_SOURCE_H


#include "util.h"
#include <algorithm>


namespace szn
{
	struct Source
	{
		virtual ~Source();
		virtual void load(std::size_t n) = 0;
		virtual std::size_t size() = 0;
		virtual char get(std::size_t index) = 0;
		virtual void drop(std::size_t n) = 0;
		virtual const char *data() = 0;

		/**
		 * @brief isStable
		 * @return whether the memory pointed to by data() remains the same
		 *         during the source's lifetime
		 */
		virtual bool isStable() const = 0;
	};

	struct MemorySource : Source
	{
		explicit MemorySource(const char *begin, const char *end);
		explicit MemorySource(const unsigned char *begin, const unsigned char *end);
		explicit MemorySource(const signed char *begin, const signed char *end);
		virtual void load(std::size_t n) SZN_OVERRIDE;
		virtual std::size_t size() SZN_OVERRIDE;
		virtual char get(std::size_t index) SZN_OVERRIDE;
		virtual void drop(std::size_t n) SZN_OVERRIDE;
		virtual const char *data() SZN_OVERRIDE;
		virtual bool isStable() const SZN_OVERRIDE;

	private:

		const char *m_begin, *m_end;
	};
}


#endif
