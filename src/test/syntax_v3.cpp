#include <boost/test/unit_test.hpp>

#include <szn/struct3.hpp>
#include <szn/bytes.hpp>
#include <szn/big_endian.hpp>
#include <szn/vector.hpp>

#include <sstream>

namespace szn
{
	namespace
	{
		struct TestStruct
		{
			RXN_REFLECT(
				(SZN_AUTO_MEMBERS) (SZN_ITERATE),
				(a, be16) (int),
				(b, be32),
				(c, bytes<be8>),
				(d, bytes<be8>) (std::string),
				(v, vector<be8, be16>)
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

		struct PrintingVisitor
		{
			explicit PrintingVisitor(std::ostream &out)
			    : out(out)
			{
			}

			template <class Format, class Value>
			void accept(Value const &value)
			{
				out << value << '\n';
			}

			template <class Format, class T>
			void accept(std::vector<T> const &)
			{
				out << "vector\n";
			}

		private:

			std::ostream &out;
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

		std::ostringstream buffer;
		PrintingVisitor p((buffer));
		t.iterate(p);
		BOOST_CHECK_EQUAL("0\n3\nvector\nhallo\nvector\n", buffer.str());
	}
}
