#include <boost/test/unit_test.hpp>
#include <boost/assign/list_of.hpp>

#include <szn/structure.hpp>
#include <szn/bytes.hpp>
#include <szn/big_endian.hpp>
#include <szn/vector.hpp>
#include <szn/source.hpp>
#include <szn/sink.hpp>

#include <sstream>

namespace szn
{
	namespace
	{
		struct simple_struct
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

		struct empty
		{
			//TODO: make empty structures compile
//			RXN_REFLECT((SZN_AUTO_MEMBERS) (SZN_ITERATE))
		};

		struct null_visitor
		{
			template <class Format, class Value>
			void accept(Value &value)
			{
				(void)value;
			}
		};

		struct printing_visitor
		{
			explicit printing_visitor(std::ostream &out)
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

		simple_struct create_simple_struct()
		{
			simple_struct t;
			t.a = 0;
			t.b = 3;
			t.c.resize(32);
			t.d = "hallo";
			t.v.resize(2, 12);
			return t;
		}

		struct tree
		{
			RXN_REFLECT((SZN_AUTO_MEMBERS) (SZN_ITERATE),
			            (value, be64),
			            (children, vector<be32, structure<tree> >)
			            )

			explicit tree(be64::default_type value)
			    : value(value)
			{
			}
		};

		struct tree_flattener
		{
			std::vector<be64::default_type> values;

			template <class Format>
			void accept(be64::default_type value)
			{
				values.push_back(value);
			}

			template <class Format>
			void accept(std::vector<tree> const &children)
			{
				BOOST_FOREACH (tree const &child, children)
				{
					visit(child);
				}
			}

			void visit(tree const &tree)
			{
				tree.iterate(*this);
			}
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v3_auto_members)
	{
		simple_struct const t = create_simple_struct();
		BOOST_CHECK_EQUAL(t.a, 0);
		BOOST_CHECK_EQUAL(t.b, 3);
		BOOST_CHECK(t.c == std::vector<szn::byte>(32));
		BOOST_CHECK_EQUAL(t.d, "hallo");
		BOOST_CHECK(t.v == std::vector<be16::default_type>(2, 12));
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v3_iterate)
	{
		simple_struct const t = create_simple_struct();

		{
			null_visitor v;
			t.iterate(v);
		}

		std::ostringstream buffer;
		printing_visitor p((buffer));
		t.iterate(p);
		BOOST_CHECK_EQUAL("0\n3\nvector\nhallo\nvector\n", buffer.str());
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v3_recursion)
	{
		tree root((8));
		root.children.push_back(tree(7));
		root.children.push_back(tree(6));
		root.children.push_back(root);

		tree_flattener v;
		v.visit(root);

		std::vector<be64::default_type> const expectedValues =
		        boost::assign::list_of(8)(7)(6)(8)(7)(6);

		BOOST_CHECK(v.values == expectedValues);
	}

	namespace
	{
		struct serialized_struct3
		{
			RXN_REFLECT((SZN_AUTO_MEMBERS) (SZN_ITERATE),
			            (i, be16),
			            (s, bytes<be8>) (std::string))
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v3_serialize)
	{
		serialized_struct3 s;
		s.i = 0xaabb;
		s.s = "abc";
		std::vector<szn::byte> generated;
		auto sink = make_container_sink(generated);
		any_structure().serialize(sink, s);
		boost::array<unsigned char, 6> const expected =
		{{
		     0xaa, 0xbb, 0x03, 'a', 'b', 'c'
		}};
		BOOST_CHECK(std::vector<unsigned char>(expected.begin(), expected.end()) == generated);
	}

	BOOST_AUTO_TEST_CASE(Serialization_Syntax_v3_deserialize)
	{
		boost::array<unsigned char, 6> const data =
		{{
		     0xaa, 0xbb, 0x03, 'a', 'b', 'c'
		}};
		serialized_struct3 s;
		s.i = 0;
		memory_source source(data);
		any_structure().deserialize(source, s);
		BOOST_CHECK_EQUAL(0xaabb, s.i);
		BOOST_CHECK_EQUAL("abc", s.s);
	}

	struct new_syntax { SZN_STRUCTURE (
		(field, be32)
	)};

	BOOST_AUTO_TEST_CASE(Serialization_structure_macro)
	{
		szn::null_sink sink;
		new_syntax n = {123};
		BOOST_CHECK_EQUAL(123, n.field);
		BOOST_STATIC_ASSERT(sizeof(n.field) >= 4);
		szn::any_structure().serialize(sink, n);
	}
}
