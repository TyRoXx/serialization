#include <szn/sink.hpp>
#include <szn/source.hpp>
#include <szn/bytes.hpp>
#include <szn/big_endian.hpp>
#include <cassert>

int main()
{
	std::vector<szn::byte> buffer;
	auto sink = szn::make_container_sink(buffer);
	szn::bytes<szn::be16>().serialize(sink, "Hello, world!");
	szn::byte const expected[] =
	{
		0, 13, //<- the length of the string in big endian
		'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!'
	};
	assert(buffer.size() == sizeof(expected));
	assert(std::equal(std::begin(buffer), std::end(buffer), std::begin(expected)));

	auto source = szn::make_container_source(buffer);
	std::string message;
	szn::bytes<szn::be16>().deserialize(source, message);
	assert(message == "Hello, world!");
}
