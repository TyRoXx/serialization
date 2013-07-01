#include <boost/test/unit_test.hpp>
#include <boost/assign/list_of.hpp>

#include <szn/struct3.hpp>
#include <szn/bytes.hpp>
#include <szn/big_endian.hpp>
#include <szn/vector.hpp>
#include <szn/struct.hpp>

#include <sstream>

namespace szn
{
	namespace
	{
		struct SimpleStruct
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

		struct Empty
		{
			//TODO: make empty structures compile
//			RXN_REFLECT((SZN_AUTO_MEMBERS) (SZN_ITERATE))
		};

		struct NullVisitor
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

		SimpleStruct createSimpleStruct()
		{
			SimpleStruct t;
			t.a = 0;
			t.b = 3;
			t.c.resize(32);
			t.d = "hallo";
			t.v.resize(2, 12);
			return t;
		}

		struct Tree
		{
			RXN_REFLECT((SZN_AUTO_MEMBERS) (SZN_ITERATE),
			            (value, be64),
			            (children, vector<be32, structure>) (std::vector<Tree>)
			            )

			explicit Tree(be64::default_type value)
			    : value(value)
			{
			}
		};

		struct TreeFlattener
		{
			std::vector<be64::default_type> values;

			template <class Format>
			void accept(be64::default_type value)
			{
				values.push_back(value);
			}

			template <class Format>
			void accept(std::vector<Tree> const &children)
			{
				BOOST_FOREACH (Tree const &child, children)
				{
					visit(child);
				}
			}

			void visit(Tree const &tree)
			{
				tree.iterate(*this);
			}
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v3_auto_members)
	{
		SimpleStruct const t = createSimpleStruct();
		BOOST_CHECK_EQUAL(t.a, 0);
		BOOST_CHECK_EQUAL(t.b, 3);
		BOOST_CHECK(t.c == std::vector<char>(32));
		BOOST_CHECK_EQUAL(t.d, "hallo");
		BOOST_CHECK(t.v == std::vector<be16::default_type>(2, 12));
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v3_iterate)
	{
		SimpleStruct const t = createSimpleStruct();

		{
			NullVisitor v;
			t.iterate(v);
		}

		std::ostringstream buffer;
		PrintingVisitor p((buffer));
		t.iterate(p);
		BOOST_CHECK_EQUAL("0\n3\nvector\nhallo\nvector\n", buffer.str());
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v3_recursion)
	{
		Tree root((8));
		root.children.push_back(Tree(7));
		root.children.push_back(Tree(6));
		root.children.push_back(root);

		TreeFlattener v;
		v.visit(root);

		std::vector<be64::default_type> const expectedValues =
		        boost::assign::list_of(8)(7)(6)(8)(7)(6);

		BOOST_CHECK(v.values == expectedValues);
	}
}
