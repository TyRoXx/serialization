#include <boost/test/unit_test.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/cstdint.hpp>

#include <szn/struct.hpp>
#include <szn/struct2.hpp>
#include <szn/little_endian.hpp>
#include <szn/big_endian.hpp>
#include <szn/bytes.hpp>
#include <szn/vector.hpp>
#include <szn/float.hpp>
#include <szn/array.hpp>
#include <szn/bool.hpp>
#include <szn/pod.hpp>
#include <szn/unique_ptr.hpp>
#include <szn/endianness.hpp>
#include <szn/format.hpp>

namespace szn
{
	namespace
	{
		SZN_BEGIN(TestStruct)
			SZN_FIELD( le32,          int,                   LE32)
			SZN_FIELD( le16,     unsigned,                   LE16)
			SZN_FIELD(  le8,         char,                   LE8 )

			SZN_FIELD( be32,          int,                   BE32)
			SZN_FIELD( be16,     unsigned,                   BE16)
			SZN_FIELD(  be8, boost::uint8_t,                 BE8 )

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

		struct Equal
		{
			template <class T>
			bool operator ()(T const &left, T const &right) const
			{
				return left == right;
			}

			template <class T, std::size_t N>
			bool operator ()(T const (&left)[N], T const (&right)[N]) const
			{
				return std::equal(&left[0], &left[N], &right[0]);
			}

#if SZN_HAS_UNIQUE_PTR
			template <class T>
			bool operator ()(std::unique_ptr<T> const &left,
							 std::unique_ptr<T> const &right) const
			{
				return *left == *right;
			}
#endif
		};

		template <class Value, class Format>
		bool serializationRoundtrip(Value const &value,
									Format const &format)
		{
			typedef typename boost::remove_const<Value>::type mutable_value;

			std::vector<char> generated;
			{
				BOOST_AUTO(sink, szn::makeContainerSink(generated));
				szn::serialize(sink, value, format);
			}

			BOOST_AUTO(source, szn::makeRangeSource(generated));
			mutable_value readValue;
			szn::deserialize(source, readValue, format);

			return Equal()(value, readValue);
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_Struct_serialize)
	{
		std::vector<unsigned char> generated;
		BOOST_AUTO(sink, szn::makeContainerSink(generated));

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
		BOOST_CHECK(std::equal(boost::begin(structureData),
							   boost::end(structureData),
							   generated.begin()));
	}

	BOOST_AUTO_TEST_CASE(Serialization_Struct_deserialize)
	{
		szn::MemorySource source(structureData);

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
		BOOST_AUTO(sink, szn::makeContainerSink(generated));

		EmptyStruct().serialize(sink);

		BOOST_CHECK(generated.empty());
	}

	BOOST_AUTO_TEST_CASE(Serialization_EmptyStruct_deserialize)
	{
		szn::MemorySource source((szn::MemorySource::range_type()));

		EmptyStruct s;
		s.deserialize(source);
	}

	namespace
	{
		template <class Container>
		bool testMakeContainerSink()
		{
			Container c;
			BOOST_AUTO(sink, szn::makeContainerSink(c));
			sink.write("hello", 5);

			return c.size() == 5 &&
					std::equal(boost::begin(c), boost::end(c), "hello");
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
		BOOST_CHECK(testMakeContainerSink<std::vector<char> >());
		BOOST_CHECK(testMakeContainerSink<std::vector<unsigned char> >());
		BOOST_CHECK(testMakeContainerSink<std::vector<signed char> >());
		BOOST_CHECK(testMakeContainerSink<std::string>());
		BOOST_CHECK((testMakeContainerSink<std::basic_string<char, TestCharTraits, TestAllocator> >()));
	}

	namespace
	{
		template <class Container>
		bool equalBytes(const Container &left, const std::string &right)
		{
			return (left.size() == right.size()) &&
					std::equal(boost::begin(left), boost::end(left), boost::begin(right));
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
		BOOST_AUTO(sink, szn::makeContainerSink(generated));

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
		const boost::array<boost::uint16_t, 2> testArray =
		{{
			0x1122, 0x3344
		}};
		typedef szn::Array<2, szn::BE16> ArrayFormat;

		BOOST_CHECK(serializationRoundtrip(testArray, ArrayFormat()));

		std::vector<unsigned char> generated;
		BOOST_AUTO(sink, szn::makeContainerSink(generated));
		szn::serialize(sink, testArray, ArrayFormat());

		BOOST_REQUIRE(generated.size() == 4);
		BOOST_CHECK(generated[0] == 0x11);
		BOOST_CHECK(generated[1] == 0x22);
		BOOST_CHECK(generated[2] == 0x33);
		BOOST_CHECK(generated[3] == 0x44);
	}

	BOOST_AUTO_TEST_CASE(Serialization_Array_C_array)
	{
		const boost::uint16_t testArray[2] =
		{
			0x1122, 0x3344
		};
		typedef szn::Array<2, szn::BE16> ArrayFormat;

		BOOST_CHECK(serializationRoundtrip(testArray, ArrayFormat()));

		std::vector<unsigned char> generated;
		BOOST_AUTO(sink, szn::makeContainerSink(generated));
		szn::serialize(sink, testArray, ArrayFormat());

		BOOST_REQUIRE(generated.size() == 4);
		BOOST_CHECK(generated[0] == 0x11);
		BOOST_CHECK(generated[1] == 0x22);
		BOOST_CHECK(generated[2] == 0x33);
		BOOST_CHECK(generated[3] == 0x44);
	}

	namespace
	{
		SZN_BEGIN2(NoMethods)
			SZN_FIELD(i, boost::uint32_t, szn::LE32)
		SZN_END()
	}

	BOOST_AUTO_TEST_CASE(Serialization_noMethods)
	{
		std::vector<unsigned char> generated;
		{
			NoMethods no;
			no.i = 123;
			BOOST_AUTO(sink, szn::makeContainerSink(generated));
			szn::Struct().serialize(sink, no);
		}

		NoMethods no;
		{
			BOOST_AUTO(source, szn::makeRangeSource(generated));
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
			explicit IterationTestVisitor(IterationTester &tester)
				: m_tester(tester)
			{
			}

			void visitField(int IterationTester::*a, szn::BE32, const char *name)
			{
				BOOST_CHECK(m_tester.*a == 2);
				BOOST_CHECK(std::string(name) == "fieldA");
			}

			void visitField(long IterationTester::*b, szn::LE32, const char *name)
			{
				BOOST_CHECK(m_tester.*b == 7);
				BOOST_CHECK(std::string(name) == "fieldB");
			}

			void visitField(char IterationTester::*c, szn::LE8, const char *name)
			{
				BOOST_CHECK(m_tester.*c == '!');
				BOOST_CHECK(std::string(name) == "fieldC");
			}

		private:

			IterationTester const &m_tester;
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_Struct_iterate)
	{
		IterationTester tester;
		tester.fieldA = 2;
		tester.fieldB = 7;
		tester.fieldC = '!';
		IterationTestVisitor visitor((tester));
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
			BOOST_AUTO(sink, szn::makeContainerSink(generated));
			szn::Struct().serialize(sink, tester);
		}

		BytesTester<ConstStringRange> tester;
		{
			BOOST_AUTO(source, szn::makeRangeSource(generated));
			szn::Struct().deserialize(source, tester);
		}

		BOOST_CHECK(tester.str == "hallo");
		BOOST_CHECK(tester.vec.size() == 1);
		BOOST_CHECK(tester.vec.at(0) == 42);
		BOOST_CHECK(rangePointee == std::string(tester.range.first, tester.range.second));
	}

	BOOST_AUTO_TEST_CASE(Serialization_Bool)
	{
		BOOST_CHECK(serializationRoundtrip(true, szn::Bool()));
		BOOST_CHECK(serializationRoundtrip(false, szn::Bool()));
	}

#if SZN_HAS_UNIQUE_PTR
	BOOST_AUTO_TEST_CASE(Serialization_UniquePtr)
	{
		BOOST_CHECK(serializationRoundtrip(
						std::unique_ptr<long>(new long(123)),
						szn::UniquePtr<szn::LE32>()));
	}
#endif

	namespace
	{
		struct TestPOD
		{
			std::size_t field;

			TestPOD()
			{
			}

			TestPOD(std::size_t field)
				: field(field)
			{
			}
		};

		bool operator == (TestPOD const &left, TestPOD const &right)
		{
			return (left.field == right.field);
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_POD)
	{
#define SZNTEST_POD(type) BOOST_CHECK(serializationRoundtrip<type>(13, szn::POD<type>()));
		SZNTEST_POD(char)
		SZNTEST_POD(signed char)
		SZNTEST_POD(unsigned char)
		SZNTEST_POD(signed short)
		SZNTEST_POD(unsigned short)
		SZNTEST_POD(signed int)
		SZNTEST_POD(unsigned int)
		SZNTEST_POD(signed long)
		SZNTEST_POD(unsigned long)
#ifndef SZN_NO_CPP11
		SZNTEST_POD(signed long long)
		SZNTEST_POD(unsigned long long)
#endif
		SZNTEST_POD(wchar_t)
		SZNTEST_POD(std::size_t)
		SZNTEST_POD(std::ptrdiff_t)
		SZNTEST_POD(TestPOD)
#undef SZNTEST_POD

		{
			boost::array<int, 3> array = {{1, 2, 3}};
			BOOST_CHECK(serializationRoundtrip(array, szn::POD<boost::array<int, 3> >()));
		}

		{
			int dummy;
			int * const pointer = &dummy;
			BOOST_CHECK(serializationRoundtrip(pointer, szn::POD<int *>()));
			BOOST_CHECK(serializationRoundtrip(&pointer, szn::POD<int * const *>()));
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_LittleEndian)
	{
		BOOST_CHECK(serializationRoundtrip<boost::int8_t>(-50, szn::LE8()));
		BOOST_CHECK(serializationRoundtrip<boost::int16_t>(-50, szn::LE16()));
		BOOST_CHECK(serializationRoundtrip<boost::int32_t>(-50, szn::LE32()));
		BOOST_CHECK(serializationRoundtrip<boost::int64_t>(-50, szn::LE64()));

		BOOST_CHECK(serializationRoundtrip(-50, szn::LE8()));
		BOOST_CHECK(serializationRoundtrip(-50, szn::LE16()));
		BOOST_CHECK(serializationRoundtrip(-50, szn::LE32()));
		BOOST_CHECK(serializationRoundtrip(-50, szn::LE64()));
	}

	BOOST_AUTO_TEST_CASE(Serialization_BigEndian)
	{
		BOOST_CHECK(serializationRoundtrip<boost::int8_t>(-50, szn::BE8()));
		BOOST_CHECK(serializationRoundtrip<boost::int16_t>(-50, szn::BE16()));
		BOOST_CHECK(serializationRoundtrip<boost::int32_t>(-50, szn::BE32()));
		BOOST_CHECK(serializationRoundtrip<boost::int64_t>(-50, szn::BE64()));

		BOOST_CHECK(serializationRoundtrip(-50, szn::BE8()));
		BOOST_CHECK(serializationRoundtrip(-50, szn::BE16()));
		BOOST_CHECK(serializationRoundtrip(-50, szn::BE32()));
		BOOST_CHECK(serializationRoundtrip(-50, szn::BE64()));
	}

	namespace
	{
		enum TestEnum
		{
			Zero = 0,
			NonZero = 12,
			Ones = 0xff
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_enum)
	{

#define SZNTEST_ENUM_VALUES(endianness, bitsize) \
		BOOST_CHECK(serializationRoundtrip(Zero, szn:: BOOST_PP_CAT(endianness, bitsize) ())); \
		BOOST_CHECK(serializationRoundtrip(NonZero, szn:: BOOST_PP_CAT(endianness, bitsize) ())); \
		BOOST_CHECK(serializationRoundtrip(Ones, szn:: BOOST_PP_CAT(endianness, bitsize) ()));

		SZNTEST_ENUM_VALUES(LE, 8)
		SZNTEST_ENUM_VALUES(LE, 16)
		SZNTEST_ENUM_VALUES(LE, 32)
		SZNTEST_ENUM_VALUES(LE, 64)

		SZNTEST_ENUM_VALUES(BE, 8)
		SZNTEST_ENUM_VALUES(BE, 16)
		SZNTEST_ENUM_VALUES(BE, 32)
		SZNTEST_ENUM_VALUES(BE, 64)

#undef SZNTEST_ENUM_VALUES
	}

#if SZN_HAS_ENUM_CLASS
	namespace
	{
		enum class TestEnumClass
		{
			Zero = 0,
			NonZero = 12,
			Ones = 0xff
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_enum_class)
	{
#define SZNTEST_ENUM_VALUES(endianness, bitsize) \
		BOOST_CHECK(serializationRoundtrip(TestEnumClass::Zero, szn:: BOOST_PP_CAT(endianness, bitsize) ())); \
		BOOST_CHECK(serializationRoundtrip(TestEnumClass::NonZero, szn:: BOOST_PP_CAT(endianness, bitsize) ())); \
		BOOST_CHECK(serializationRoundtrip(TestEnumClass::Ones, szn:: BOOST_PP_CAT(endianness, bitsize) ()));

		SZNTEST_ENUM_VALUES(LE, 8)
		SZNTEST_ENUM_VALUES(LE, 16)
		SZNTEST_ENUM_VALUES(LE, 32)
		SZNTEST_ENUM_VALUES(LE, 64)

		SZNTEST_ENUM_VALUES(BE, 8)
		SZNTEST_ENUM_VALUES(BE, 16)
		SZNTEST_ENUM_VALUES(BE, 32)
		SZNTEST_ENUM_VALUES(BE, 64)

#undef SZNTEST_ENUM_VALUES
	}
#endif

	namespace
	{
		template <class Format>
		bool testMinMaxSize(Format const &, std::size_t expectedMin, std::size_t expectedMax)
		{
			std::size_t const min = szn::MinSize<Format>::value;
			std::size_t const max = szn::MaxSize<Format>::value;
			return (min == expectedMin) && (max == expectedMax);
		}

		template <class Format>
		bool testExactSize(Format const &format, std::size_t expectedSize)
		{
			return testMinMaxSize(format, expectedSize, expectedSize);
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_min_size)
	{
		BOOST_CHECK(testExactSize(szn::BE8(), 1));
		BOOST_CHECK(testExactSize(szn::BE16(), 2));
		BOOST_CHECK(testExactSize(szn::BE32(), 4));
		BOOST_CHECK(testExactSize(szn::BE64(), 8));

		BOOST_CHECK(testExactSize(szn::LE8(), 1));
		BOOST_CHECK(testExactSize(szn::LE16(), 2));
		BOOST_CHECK(testExactSize(szn::LE32(), 4));
		BOOST_CHECK(testExactSize(szn::LE64(), 8));

		BOOST_CHECK(testExactSize(szn::Bool(), 1));

		BOOST_CHECK(testMinMaxSize(szn::Bytes<szn::LE32>(), 4, std::numeric_limits<std::size_t>::max()));

#if SZN_HAS_UNIQUE_PTR
		BOOST_CHECK(testExactSize(szn::UniquePtr<szn::LE64>(), 8));
#endif
	}
}
