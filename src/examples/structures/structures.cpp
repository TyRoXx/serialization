#include <szn/sink.hpp>
#include <szn/source.hpp>
#include <szn/structure.hpp>
#include <szn/bytes.hpp>
#include <szn/big_endian.hpp>
#include <cassert>

struct person_entry
{
	SZN_STRUCTURE
	(
		(id, szn::be32),
		(name, szn::bytes<szn::be8>) (std::string)
	)
};

struct group
{
	SZN_STRUCTURE
	(
		(members, szn::vector<szn::be16, szn::structure<person_entry> >)
	)
};

int main()
{
	std::vector<szn::byte> buffer;
	auto sink = szn::make_container_sink(buffer);

	{
		group users;
		{
			person_entry alice;
			alice.id = 12;
			alice.name = "Alice";
			users.members.push_back(alice);
		}
		szn::any_structure().serialize(sink, users);

		szn::byte const expected[] =
		{
		    0, 1,
		    0, 0, 0, 12,
		    5, 'A', 'l', 'i', 'c', 'e'
		};
		assert(buffer.size() == sizeof(expected));
		assert(std::equal(std::begin(buffer), std::end(buffer), std::begin(expected)));
	}

	{
		auto source = szn::make_container_source(buffer);
		group users;
		szn::any_structure().deserialize(source, users);
		assert(users.members.size() == 1);
		assert(users.members[0].id == 12);
		assert(users.members[0].name == "Alice");
	}
}
