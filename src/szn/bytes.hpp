#ifndef SERIALIZATION_BYTES_HPP_8BE0A18D_7A18_41A3_B154_C6C47903EDE7
#define SERIALIZATION_BYTES_HPP_8BE0A18D_7A18_41A3_B154_C6C47903EDE7


#include <szn/util.hpp>
#include <szn/format.hpp>
#include <szn/to_bytes.hpp>
#include <szn/vector.hpp>
#include <szn/big_endian.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/utility.hpp> //next
#include <boost/cstdint.hpp>
#include <string>
#include <vector>
#include <utility>
#include <stdexcept>
#include <cassert>


namespace szn
{
	/// a format for a variable-length sequence of bytes preceded by
	/// its length.
	/// Can be used with std::string
	template <class LengthFormat>
	struct bytes : vector<LengthFormat, be8>
	{
	};
}


#endif
