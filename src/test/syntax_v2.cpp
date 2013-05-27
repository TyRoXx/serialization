#include <boost/test/unit_test.hpp>

#include <szn/struct2.hpp>
#include <szn/bytes.hpp>
#include <szn/big_endian.hpp>

namespace szn
{
	namespace
	{
		struct TestStruct
		{
			RXN_REFLECT(
				(RXN_MEMBERS) (RXN_ITERATE),
				(a, int) (szn::BE16),
				(b, long) (szn::BE32),
				(c, std::string) (szn::Bytes<szn::BE8>)
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
		TestVisitor v;
		t.iterate(v);

		TestStruct2 t2;
		t2.name = "name";
		t2.id = 123;
	}
}
