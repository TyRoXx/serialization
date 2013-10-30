#include <szn/sink.hpp>
#include <szn/source.hpp>
#include <szn/big_endian.hpp>
#include <szn/little_endian.hpp>
#include <cassert>

int main()
{
	std::vector<unsigned char> buffer;
	auto sink = szn::make_container_sink(buffer);
	szn::be64().serialize(sink, 0x12345678);
	szn::le32().serialize(sink, 0xaabbccdd);
	unsigned char const expected[] =
	{
	    0, 0, 0, 0, 0x12, 0x34, 0x56, 0x78,
	    0xdd, 0xcc, 0xbb, 0xaa
	};
	assert(buffer.size() == sizeof(expected));
	assert(std::equal(std::begin(buffer), std::end(buffer), std::begin(expected)));

	auto source = szn::make_container_source(buffer);

	boost::uint64_t first = 0;
	szn::be64().deserialize(source, first);
	assert(first == 0x12345678);

	boost::uint32_t second = 0;
	szn::le32().deserialize(source, second);
	assert(second == 0xaabbccdd);
}
