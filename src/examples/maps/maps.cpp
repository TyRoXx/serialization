#include <szn/sink.hpp>
#include <szn/source.hpp>
#include <szn/map.hpp>
#include <szn/bytes.hpp>
#include <szn/big_endian.hpp>
#include <cassert>
#include <map>

int main()
{
	std::vector<szn::byte> buffer;
	auto sink = szn::make_container_sink(buffer);
	typedef std::map<boost::uint16_t, std::string> map_type;
	map_type original;
	original[456] = "abc";

	//szn::map is a simple format for containers like std::map, boost::unordered_map etc.
	typedef szn::map<
				szn::be8, //format for the number of map entries
				szn::be16, //format for the keys
				szn::bytes<szn::be16> //format for the values
	        > map_format;

	map_format().serialize(sink, original);
	szn::byte const expected[] =
	{
	    1, //the number of map entries
	    1, 200, //456 as bytes in big endian
	    0, 3, //the string length
	    'a', 'b', 'c'
	};
	assert(buffer.size() == sizeof(expected));
	assert(std::equal(std::begin(buffer), std::end(buffer), std::begin(expected)));

	auto source = szn::make_container_source(buffer);
	map_type deserialized;
	map_format().deserialize(source, deserialized);
	assert(deserialized == original);
}
