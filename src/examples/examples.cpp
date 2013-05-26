#include <szn/bytes.hpp>
#include <szn/sink.hpp>
#include <szn/big_endian.hpp>

int main()
{
	szn::NullSink sink;
	szn::Bytes<szn::BE8>().serialize(sink, "Hello, world!");
}
