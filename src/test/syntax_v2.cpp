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
			RXN_FIELDS (
				(szn::BE16) (int) a,
				(szn::BE32) (long) b,
				(szn::Bytes<szn::BE8>) (std::string) c
			)
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v2)
	{
		TestStruct t;
		t.a = 0;
		t.b = 3;
		t.c = "hallo";
	}
}
