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
			typedef std::vector<int, std::allocator<int> > VectorInt;
			typedef szn::Vector<szn::BE8, szn::BE16> Vector8_16;

			RXN_REFLECT(
				(SZN_AUTO_MEMBERS),
				(a, szn::BE16) (int),
				(b, szn::BE32),
				(c, szn::Bytes<szn::BE8>),
			    (d, szn::Bytes<szn::BE8>) (std::string),
				(v, szn::Vector<szn::BE8, szn::BE16>)
			)
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v3)
	{
		TestStruct t;
		t.a = 0;
		t.b = 3;
		t.c.resize(32);
		t.d = "hallo";
		t.v.resize(2, 12);
	}
}
