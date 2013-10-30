#include <szn/sink.hpp>
#include <szn/source.hpp>
#include <szn/structure.hpp>
#include <szn/bytes.hpp>
#include <szn/big_endian.hpp>
#include <cassert>

struct person_entry
{
	RXN_REFLECT((SZN_AUTO_MEMBERS) (SZN_ITERATE),
            (id, szn::be32),
            (name, szn::bytes<szn::be8>) (std::string)
            )
};

int main()
{
	std::vector<unsigned char> buffer;
	auto sink = szn::make_container_sink(buffer);

	person_entry alice;
	alice.id = 12;
	alice.name = "Alice";
	szn::structure3().serialize(sink, alice);

	unsigned char const expected[] =
	{
	    0, 0, 0, 12,
	    5, 'A', 'l', 'i', 'c', 'e'
	};
	assert(buffer.size() == sizeof(expected));
	assert(std::equal(std::begin(buffer), std::end(buffer), std::begin(expected)));

	auto source = szn::make_container_source(buffer);
	person_entry who;
	szn::structure3().deserialize(source, who);
	assert(alice.id == who.id);
	assert(alice.name == who.name);
}
