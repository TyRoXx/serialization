#include <szn/bytes.hpp>
#include <szn/sink.hpp>
#include <szn/big_endian.hpp>
#include <iostream>

int main()
{
	szn::null_sink sink;
	szn::bytes<szn::be8>().serialize(sink, "Hello, world!");

	boost::uint32_t s = std::rand();
	szn::stream_sink sink2(std::cout);
	szn::be32().serialize(sink2, s);
}
