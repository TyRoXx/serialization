#include <boost/test/unit_test.hpp>

#include <szn/struct2.hpp>
#include <szn/bytes.hpp>
#include <szn/big_endian.hpp>
#include <szn/vector.hpp>

namespace szn
{
	namespace
	{
		struct TestStruct
		{
			typedef std::vector<int, std::allocator<int>> VectorInt;
			typedef szn::Vector<szn::BE8, szn::BE16> Vector8_16;

			RXN_REFLECT(
				(RXN_MEMBERS) (RXN_ITERATE),
				(a, int) (szn::BE16),
				(b, long) (szn::BE32),
				(c, std::string) (szn::Bytes<szn::BE8>),
				(v, std::vector<int, std::allocator<int>>) (Vector8_16)
			)
		};

#define TEST_STRUCT_2_MEMBERS \
		(name, std::string) (szn::Bytes<szn::BE32>), \
		(id, int) (szn::BE16)

		struct TestStruct2
		{
			RXN_REFLECT((RXN_MEMBERS) (RXN_ITERATE), TEST_STRUCT_2_MEMBERS)
		};

		struct TestVisitor
		{
			template <class Format, class Value>
			void accept(Value &member)
			{
				(void)member;
			}
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v2)
	{
		TestStruct t;
		t.a = 0;
		t.b = 3;
		t.c = "hallo";
		t.v.resize(2, 12);
		TestVisitor v;
		t.iterate(v);

		TestStruct2 t2;
		t2.name = "name";
		t2.id = 123;
	}
}
