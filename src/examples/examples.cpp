#include <szn/bytes.hpp>
#include <szn/sink.hpp>
#include <szn/big_endian.hpp>

int main()
{
	szn::null_sink sink;
	szn::bytes<szn::be8>().serialize(sink, "Hello, world!");
}
