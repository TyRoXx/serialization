#include <boost/test/unit_test.hpp>

#include <szn/struct3.hpp>
#include <szn/bytes.hpp>
#include <szn/big_endian.hpp>
#include <szn/vector.hpp>

namespace szn
{
	namespace
	{
		struct TestStruct
		{
			RXN_REFLECT(
				(SZN_AUTO_MEMBERS) (SZN_ITERATE),
				(a, szn::BE16) (int),
				(b, szn::BE32),
				(c, szn::bytes<szn::BE8>),
				(d, szn::bytes<szn::BE8>) (std::string),
				(v, szn::vector<szn::BE8, szn::BE16>)
			)
		};

		struct TestVisitor
		{
			template <class Format, class Value>
			void accept(Value &value)
			{
				(void)value;
			}
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v3_auto_members)
	{
		TestStruct t;
		t.a = 0;
		t.b = 3;
		t.c.resize(32);
		t.d = "hallo";
		t.v.resize(2, 12);

		TestVisitor v;
		t.iterate(v);
	}
}
