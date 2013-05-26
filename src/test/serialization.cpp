#include <boost/test/unit_test.hpp>

#include <szn/struct.hpp>
#include <szn/little_endian.hpp>
#include <szn/big_endian.hpp>
#include <szn/bytes.hpp>
#include <szn/vector.hpp>
#include <szn/float.hpp>
#include <szn/array.hpp>


namespace szn
{
	namespace
	{
		SZN_BEGIN(TestStruct)
			SZN_FIELD( le32,          int,                   LE32)
			SZN_FIELD( le16,          int,                   LE16)
			SZN_FIELD(  le8,         char,                   LE8 )

			SZN_FIELD( be32,          int,                   BE32)
			SZN_FIELD( be16,          int,                   BE16)
			SZN_FIELD(  be8, std::uint8_t,                   BE8 )

			SZN_FIELD(str16,  std::string,            Bytes<LE16>)
			SZN_FIELD( str8,  std::string,            Bytes<LE8 >)

			SZN_FIELD( vec8, std::vector<int>, Vector<LE8 BOOST_PP_COMMA() LE32>)

			SZN_FIELD( f32,         float,      BinaryFloat<LE32>)
			SZN_FIELD( f64,        double,      BinaryFloat<LE64>)
		SZN_END()

		const unsigned char structureData[] =
		{
			/*le32*/   0x20, 0x00, 0x00, 0x00,
			/*le16*/   0xff, 0xff,
			/*le8*/    0x07,

			/*be32*/   0x01, 0x01, 0x01, 0x01,
			/*be16*/   0x80, 0,
			/*be8*/    0xff,

			/*str16*/  0x05, 0x00, 'h', 'a', 'l', 'l', 'o',
			/*str8*/   0x00,

			/*vec8*/   0x02,
						   0xef, 0xbe, 0xad, 0xde,
						   0x00, 0x00, 0x00, 0x00,

			/*f32*/    0x00, 0x00, 0x00, 0x00,
			/*f64*/    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_Struct_serialize)
	{
		std::vector<unsigned char> generated;
		auto sink = szn::makeContainerSink(generated);

		TestStruct b;
		b.le32 = 32;
		b.le16 = 0xffff;
		b.le8 = 7;
		b.be32 = 0x01010101;
		b.be16 = 0x8000;
		b.be8 = 0xff;
		b.str16 = "hallo";
		b.str8 = "";
		b.vec8.push_back(0xdeadbeef);
		b.vec8.push_back(0);
		b.f32 = 0.0f;
		b.f64 = 0.0;

		serialize(sink, b, szn::ByMethod());

		BOOST_REQUIRE_EQUAL(generated.size(), sizeof(structureData));
		BOOST_CHECK(std::equal(std::begin(structureData),
							   std::end(structureData),
							   generated.begin()));
	}

	BOOST_AUTO_TEST_CASE(Serialization_Struct_deserialize)
	{
		szn::MemorySource source(structureData,
								 structureData + sizeof(structureData));

		TestStruct b;
		deserialize(source, b, szn::ByMethod());

		BOOST_CHECK_EQUAL(b.le32, 32);
		BOOST_CHECK_EQUAL(b.le16, 0xffff);
		BOOST_CHECK_EQUAL(b.le8, 7);
		BOOST_CHECK_EQUAL(b.be32, 0x01010101);
		BOOST_CHECK_EQUAL(b.be16, 0x8000);
		BOOST_CHECK_EQUAL(b.be8, 0xff);
		BOOST_CHECK_EQUAL(b.str16, "hallo");
		BOOST_CHECK_EQUAL(b.str8, "");
		BOOST_REQUIRE_EQUAL(b.vec8.size(), 2);
		BOOST_CHECK_EQUAL(b.vec8[0], 0xdeadbeef);
		BOOST_CHECK_EQUAL(b.vec8[1], 0);
		BOOST_CHECK_EQUAL(b.f32, 0.0f);
		BOOST_CHECK_EQUAL(b.f64, 0.0);
	}

	namespace
	{
		SZN_BEGIN(EmptyStruct)
		SZN_END()
	}

	BOOST_AUTO_TEST_CASE(Serialization_EmptyStruct_serialize)
	{
		std::string generated;
		auto sink = szn::makeContainerSink(generated);

		EmptyStruct().serialize(sink);

		BOOST_CHECK(generated.empty());
	}

	BOOST_AUTO_TEST_CASE(Serialization_EmptyStruct_deserialize)
	{
		const char * const nothing = nullptr;
		szn::MemorySource source(nothing, nothing);

		EmptyStruct s;
		s.deserialize(source);
	}

	namespace
	{
		template <class Container>
		bool testMakeContainerSink()
		{
			Container c;
			auto sink = szn::makeContainerSink(c);
			sink.write("hello", 5);

			return c.size() == 5 &&
					std::equal(begin(c), end(c), "hello");
		}

		struct TestAllocator : std::allocator<char>
		{
		};

		struct TestCharTraits : std::char_traits<char>
		{
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_makeContainerSink)
	{
		BOOST_CHECK(testMakeContainerSink<std::vector<char>>());
		BOOST_CHECK(testMakeContainerSink<std::vector<unsigned char>>());
		BOOST_CHECK(testMakeContainerSink<std::vector<signed char>>());
		BOOST_CHECK(testMakeContainerSink<std::string>());
		BOOST_CHECK((testMakeContainerSink<std::basic_string<char, TestCharTraits, TestAllocator>>()));
	}

	namespace
	{
		template <class Container>
		bool equalBytes(const Container &left, const std::string &right)
		{
			return (left.size() == right.size()) &&
					std::equal(begin(left), end(left), begin(right));
		}

		namespace for_adl_tests
		{
			struct Custom
			{
				bool ok;
			};

			void serialize(Sink &, const Custom &custom, szn::ByADL)
			{
				BOOST_REQUIRE(custom.ok);
			}

			void deserialize(Source &, Custom &custom, szn::ByADL)
			{
				BOOST_REQUIRE(!custom.ok);
				custom.ok = true;
			}

			SZN_BEGIN(Wrapper)
				SZN_FIELD(c, Custom, szn::ByADL)
			SZN_END()
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_ASCIIFloat_serialize)
	{
		typedef szn::ASCIIFloat<4> Format;

		std::vector<char> generated;
		auto sink = szn::makeContainerSink(generated);

		Format().serialize(sink, 1.234f);

		BOOST_CHECK(equalBytes(generated, std::string("1.234\0", 6)));
	}

	BOOST_AUTO_TEST_CASE(Serialization_ByADL)
	{
		szn::NullSink sink;
		for_adl_tests::Wrapper w;
		w.c.ok = true;
		szn::serialize(sink, w, szn::ByMethod());
	}

	BOOST_AUTO_TEST_CASE(Serialization_Array_std_array)
	{
		std::vector<unsigned char> generated;
		auto sink = szn::makeContainerSink(generated);

		const std::array<std::uint16_t, 2> testArray =
		{{
			0x1122, 0x3344
		}};

		typedef szn::Array<2, szn::BE16> ArrayFormat;

		szn::serialize(sink, testArray, ArrayFormat());

		BOOST_REQUIRE(generated.size() == 4);
		BOOST_CHECK(generated[0] == 0x11);
		BOOST_CHECK(generated[1] == 0x22);
		BOOST_CHECK(generated[2] == 0x33);
		BOOST_CHECK(generated[3] == 0x44);

		std::array<std::uint16_t, 2> deserializedArray;
		{
			szn::MemorySource source(generated.data(),
									 generated.data() + generated.size());
			szn::deserialize(source, deserializedArray, ArrayFormat());
		}

		BOOST_CHECK(testArray == deserializedArray);
	}

	BOOST_AUTO_TEST_CASE(Serialization_Array_C_array)
	{
		std::vector<unsigned char> generated;
		auto sink = szn::makeContainerSink(generated);

		const std::uint16_t testArray[2] =
		{
			0x1122, 0x3344
		};

		typedef szn::Array<2, szn::BE16> ArrayFormat;

		szn::serialize(sink, testArray, ArrayFormat());

		BOOST_REQUIRE(generated.size() == 4);
		BOOST_CHECK(generated[0] == 0x11);
		BOOST_CHECK(generated[1] == 0x22);
		BOOST_CHECK(generated[2] == 0x33);
		BOOST_CHECK(generated[3] == 0x44);

		std::uint16_t deserializedArray[2];
		{
			szn::MemorySource source(generated.data(),
									 generated.data() + generated.size());
			szn::deserialize(source, deserializedArray, ArrayFormat());
		}

		using std::begin;
		using std::end;
		BOOST_CHECK(std::equal(begin(testArray), end(testArray),
							   begin(deserializedArray)));
	}

	namespace
	{
		SZN_BEGIN2(NoMethods)
			SZN_FIELD(i, std::uint32_t, szn::LE32)
		SZN_END()
	}

	BOOST_AUTO_TEST_CASE(Serialization_noMethods)
	{
		std::vector<unsigned char> generated;
		{
			NoMethods no;
			no.i = 123;
			auto sink = szn::makeContainerSink(generated);
			szn::Struct().serialize(sink, no);
		}

		NoMethods no;
		{
			szn::MemorySource source(generated.data(),
									 generated.data() + generated.size());
			szn::Struct().deserialize(source, no);
		}

		BOOST_CHECK(no.i == 123);
	}

	namespace
	{
		SZN_BEGIN2(IterationTester)
			SZN_FIELD(fieldA, int, szn::BE32)
			SZN_FIELD(fieldB, long, szn::LE32)
			SZN_FIELD(fieldC, char, szn::LE8)
		SZN_END()

		struct IterationTestVisitor SZN_FINAL
		{
			void visitField(int a, szn::BE32, const char *name)
			{
				BOOST_CHECK(a == 2);
				BOOST_CHECK(std::string(name) == "fieldA");
			}

			void visitField(long b, szn::LE32, const char *name)
			{
				BOOST_CHECK(b == 7);
				BOOST_CHECK(std::string(name) == "fieldB");
			}

			void visitField(char c, szn::LE8, const char *name)
			{
				BOOST_CHECK(c == '!');
				BOOST_CHECK(std::string(name) == "fieldC");
			}
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_Struct_iterate)
	{
		IterationTester tester;
		tester.fieldA = 2;
		tester.fieldB = 7;
		tester.fieldC = '!';
		IterationTestVisitor visitor;
		tester.iterate(visitor);
	}

	namespace
	{
		//SZN structures can in fact be templates.
		//For example C++ field types could be different for the sender and receiver.
		template <class Range>
		SZN_BEGIN2(BytesTester)
			SZN_FIELD(str, std::string, szn::Bytes<szn::BE64>)
			SZN_FIELD(vec, std::vector<signed char>, szn::Bytes<szn::BE64>)
			SZN_FIELD(range, Range, szn::Bytes<szn::BE64>)
		SZN_END()
	}

	BOOST_AUTO_TEST_CASE(Serialization_Bytes)
	{
		typedef std::pair<const char *, const char *> ConstStringRange;
		const std::string rangePointee = "range";

		std::vector<signed char> generated;
		{
			BytesTester<ConstStringRange> tester;
			tester.str = "hallo";
			tester.vec.push_back(42);
			tester.range = std::make_pair(rangePointee.data(),
										  rangePointee.data() + rangePointee.size());
			auto sink = szn::makeContainerSink(generated);
			szn::Struct().serialize(sink, tester);
		}

		BytesTester<ConstStringRange> tester;
		{
			szn::MemorySource source(generated.data(),
									 generated.data() + generated.size());
			szn::Struct().deserialize(source, tester);
		}

		BOOST_CHECK(tester.str == "hallo");
		BOOST_CHECK(tester.vec.size() == 1);
		BOOST_CHECK(tester.vec.at(0) == 42);
		BOOST_CHECK(rangePointee == std::string(tester.range.first, tester.range.second));
	}
}
