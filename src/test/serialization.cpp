#include <boost/test/unit_test.hpp>

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
#include <szn/sink.hpp>
#include <szn/source.hpp>
#include <szn/pair.hpp>
#include <szn/optional.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>

#include <sstream>
#include <map>

namespace szn
{
	namespace
	{
		SZN_BEGIN(TestStruct)
			SZN_FIELD(le32_,          int,                   le32)
			SZN_FIELD(le16_,     unsigned,                   le16)
			SZN_FIELD( le8_,         char,                   le8 )

			SZN_FIELD(be32_,          int,                   be32)
			SZN_FIELD(be16_,     unsigned,                   be16)
			SZN_FIELD( be8_, boost::uint8_t,                 be8 )

			SZN_FIELD(str16,  std::string,            bytes<le16>)
			SZN_FIELD( str8,  std::string,            bytes<le8 >)

			SZN_FIELD( vec8, std::vector<int>, vector<le8 BOOST_PP_COMMA() le32>)

			SZN_FIELD( f32,         float,     binary_float<le32>)
			SZN_FIELD( f64,        double,     binary_float<le64>)
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
				BOOST_AUTO(sink, szn::make_container_sink(generated));
				szn::serialize(sink, value, format);
			}

			BOOST_AUTO(source, szn::make_range_source(generated));
			mutable_value readValue;
			szn::deserialize(source, readValue, format);

			return Equal()(value, readValue);
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_Struct_serialize)
	{
		std::vector<unsigned char> generated;
		BOOST_AUTO(sink, szn::make_container_sink(generated));

		TestStruct b;
		b.le32_ = 32;
		b.le16_ = 0xffff;
		b.le8_ = 7;
		b.be32_ = 0x01010101;
		b.be16_ = 0x8000;
		b.be8_ = 0xff;
		b.str16 = "hallo";
		b.str8 = "";
		b.vec8.push_back(0xdeadbeef);
		b.vec8.push_back(0);
		b.f32 = 0.0f;
		b.f64 = 0.0;

		serialize(sink, b, szn::by_method());

		BOOST_REQUIRE_EQUAL(generated.size(), sizeof(structureData));
		BOOST_CHECK(std::equal(boost::begin(structureData),
							   boost::end(structureData),
							   generated.begin()));
	}

	BOOST_AUTO_TEST_CASE(Serialization_Struct_deserialize)
	{
		szn::memory_source source(structureData);

		TestStruct b;
		deserialize(source, b, szn::by_method());

		BOOST_CHECK_EQUAL(b.le32_, 32);
		BOOST_CHECK_EQUAL(b.le16_, 0xffff);
		BOOST_CHECK_EQUAL(b.le8_, 7);
		BOOST_CHECK_EQUAL(b.be32_, 0x01010101);
		BOOST_CHECK_EQUAL(b.be16_, 0x8000);
		BOOST_CHECK_EQUAL(b.be8_, 0xff);
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
		BOOST_AUTO(sink, szn::make_container_sink(generated));

		EmptyStruct().serialize(sink);

		BOOST_CHECK(generated.empty());
	}

	BOOST_AUTO_TEST_CASE(Serialization_EmptyStruct_deserialize)
	{
		szn::memory_source source((szn::memory_source::range_type()));

		EmptyStruct s;
		s.deserialize(source);
	}

	namespace
	{
		template <class Container>
		bool testMakeContainerSink()
		{
			Container c;
			BOOST_AUTO(sink, szn::make_container_sink(c));
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

			void serialize(sink &, const Custom &custom, szn::by_adl)
			{
				BOOST_REQUIRE(custom.ok);
			}

			void deserialize(source &, Custom &custom, szn::by_adl)
			{
				BOOST_REQUIRE(!custom.ok);
				custom.ok = true;
			}

			SZN_BEGIN(Wrapper)
				SZN_FIELD(c, Custom, szn::by_adl)
			SZN_END()
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_ASCIIFloat_serialize)
	{
		typedef szn::ascii_float<4> Format;

		std::vector<char> generated;
		BOOST_AUTO(sink, szn::make_container_sink(generated));

		Format().serialize(sink, 1.234f);

		BOOST_CHECK(equalBytes(generated, std::string("1.234\0", 6)));
	}

	BOOST_AUTO_TEST_CASE(Serialization_ByADL)
	{
		szn::null_sink sink;
		for_adl_tests::Wrapper w;
		w.c.ok = true;
		szn::serialize(sink, w, szn::by_method());
	}

	BOOST_AUTO_TEST_CASE(Serialization_Array_std_array)
	{
		const boost::array<boost::uint16_t, 2> testArray =
		{{
			0x1122, 0x3344
		}};
		typedef szn::array<2, szn::be16> ArrayFormat;

		BOOST_CHECK(serializationRoundtrip(testArray, ArrayFormat()));

		std::vector<unsigned char> generated;
		BOOST_AUTO(sink, szn::make_container_sink(generated));
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
		typedef szn::array<2, szn::be16> ArrayFormat;

		BOOST_CHECK(serializationRoundtrip(testArray, ArrayFormat()));

		std::vector<unsigned char> generated;
		BOOST_AUTO(sink, szn::make_container_sink(generated));
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
			SZN_FIELD(i, boost::uint32_t, szn::le32)
		SZN_END()
	}

	BOOST_AUTO_TEST_CASE(Serialization_noMethods)
	{
		std::vector<unsigned char> generated;
		{
			NoMethods no;
			no.i = 123;
			BOOST_AUTO(sink, szn::make_container_sink(generated));
			szn::structure().serialize(sink, no);
		}

		NoMethods no;
		{
			BOOST_AUTO(source, szn::make_range_source(generated));
			szn::structure().deserialize(source, no);
		}

		BOOST_CHECK(no.i == 123);
	}

	namespace
	{
		SZN_BEGIN2(IterationTester)
			SZN_FIELD(fieldA, int, szn::be32)
			SZN_FIELD(fieldB, long, szn::le32)
			SZN_FIELD(fieldC, char, szn::le8)
		SZN_END()

		struct iteration_test_visitor SZN_FINAL
		{
			explicit iteration_test_visitor(IterationTester &tester)
				: m_tester(tester)
			{
			}

			void visit_field(int IterationTester::*a, szn::be32, const char *name)
			{
				BOOST_CHECK(m_tester.*a == 2);
				BOOST_CHECK(std::string(name) == "fieldA");
			}

			void visit_field(long IterationTester::*b, szn::le32, const char *name)
			{
				BOOST_CHECK(m_tester.*b == 7);
				BOOST_CHECK(std::string(name) == "fieldB");
			}

			void visit_field(char IterationTester::*c, szn::le8, const char *name)
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
		iteration_test_visitor visitor((tester));
		tester.iterate(visitor);
	}

	namespace
	{
		//SZN structures can in fact be templates.
		//For example C++ field types could be different for the sender and receiver.
		template <class Range>
		SZN_BEGIN2(BytesTester)
			SZN_FIELD(str, std::string, szn::bytes<szn::be64>)
			SZN_FIELD(vec, std::vector<signed char>, szn::bytes<szn::be64>)
			SZN_FIELD(range, Range, szn::bytes<szn::be64>)
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
			BOOST_AUTO(sink, szn::make_container_sink(generated));
			szn::structure().serialize(sink, tester);
		}

		BytesTester<ConstStringRange> tester;
		{
			BOOST_AUTO(source, szn::make_range_source(generated));
			szn::structure().deserialize(source, tester);
		}

		BOOST_CHECK(tester.str == "hallo");
		BOOST_CHECK(tester.vec.size() == 1);
		BOOST_CHECK(tester.vec.at(0) == 42);
		BOOST_CHECK(rangePointee == std::string(tester.range.first, tester.range.second));
	}

	BOOST_AUTO_TEST_CASE(Serialization_Bool)
	{
		BOOST_CHECK(serializationRoundtrip(true,  szn::boolean()));
		BOOST_CHECK(serializationRoundtrip(false, szn::boolean()));

		BOOST_CHECK(serializationRoundtrip(true,  szn::not<szn::boolean>()));
		BOOST_CHECK(serializationRoundtrip(false, szn::not<szn::boolean>()));
	}

#if SZN_HAS_UNIQUE_PTR
	BOOST_AUTO_TEST_CASE(Serialization_UniquePtr)
	{
		BOOST_CHECK(serializationRoundtrip(
						std::unique_ptr<long>(new long(123)),
						szn::unique_ptr<szn::le32>()));
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
#define SZNTEST_POD(type) BOOST_CHECK(serializationRoundtrip<type>(13, szn::pod<type>()));
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
			BOOST_CHECK(serializationRoundtrip(array, szn::pod<boost::array<int, 3> >()));
		}

		{
			int dummy;
			int * const pointer = &dummy;
			BOOST_CHECK(serializationRoundtrip(pointer, szn::pod<int *>()));
			BOOST_CHECK(serializationRoundtrip(&pointer, szn::pod<int * const *>()));
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_LittleEndian)
	{
		BOOST_CHECK(serializationRoundtrip<boost::int8_t>(-50, szn::le8()));
		BOOST_CHECK(serializationRoundtrip<boost::int16_t>(-50, szn::le16()));
		BOOST_CHECK(serializationRoundtrip<boost::int32_t>(-50, szn::le32()));
		BOOST_CHECK(serializationRoundtrip<boost::int64_t>(-50, szn::le64()));

		BOOST_CHECK(serializationRoundtrip(-50, szn::le8()));
		BOOST_CHECK(serializationRoundtrip(-50, szn::le16()));
		BOOST_CHECK(serializationRoundtrip(-50, szn::le32()));
		BOOST_CHECK(serializationRoundtrip(-50, szn::le64()));
	}

	BOOST_AUTO_TEST_CASE(Serialization_BigEndian)
	{
		BOOST_CHECK(serializationRoundtrip<boost::int8_t>(-50, szn::be8()));
		BOOST_CHECK(serializationRoundtrip<boost::int16_t>(-50, szn::be16()));
		BOOST_CHECK(serializationRoundtrip<boost::int32_t>(-50, szn::be32()));
		BOOST_CHECK(serializationRoundtrip<boost::int64_t>(-50, szn::be64()));

		BOOST_CHECK(serializationRoundtrip(-50, szn::be8()));
		BOOST_CHECK(serializationRoundtrip(-50, szn::be16()));
		BOOST_CHECK(serializationRoundtrip(-50, szn::be32()));
		BOOST_CHECK(serializationRoundtrip(-50, szn::be64()));
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

		SZNTEST_ENUM_VALUES(le, 8)
		SZNTEST_ENUM_VALUES(le, 16)
		SZNTEST_ENUM_VALUES(le, 32)
		SZNTEST_ENUM_VALUES(le, 64)

		SZNTEST_ENUM_VALUES(be, 8)
		SZNTEST_ENUM_VALUES(be, 16)
		SZNTEST_ENUM_VALUES(be, 32)
		SZNTEST_ENUM_VALUES(be, 64)

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

		SZNTEST_ENUM_VALUES(le, 8)
		SZNTEST_ENUM_VALUES(le, 16)
		SZNTEST_ENUM_VALUES(le, 32)
		SZNTEST_ENUM_VALUES(le, 64)

		SZNTEST_ENUM_VALUES(be, 8)
		SZNTEST_ENUM_VALUES(be, 16)
		SZNTEST_ENUM_VALUES(be, 32)
		SZNTEST_ENUM_VALUES(be, 64)

#undef SZNTEST_ENUM_VALUES
	}
#endif

	namespace
	{
		template <class Format>
		bool testMinMaxSize(Format const &, length_type expectedMin, length_type expectedMax)
		{
			length_type const min = szn::min_size<Format>::value;
			length_type const max = szn::max_size<Format>::value;
			return (min == expectedMin) && (max == expectedMax);
		}

		template <class Format>
		bool testExactSize(Format const &format, length_type expectedSize)
		{
			return testMinMaxSize(format, expectedSize, expectedSize);
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_min_size)
	{
		BOOST_CHECK(testExactSize(szn::be8(), 1));
		BOOST_CHECK(testExactSize(szn::be16(), 2));
		BOOST_CHECK(testExactSize(szn::be32(), 4));
		BOOST_CHECK(testExactSize(szn::be64(), 8));

		BOOST_CHECK(testExactSize(szn::le8(), 1));
		BOOST_CHECK(testExactSize(szn::le16(), 2));
		BOOST_CHECK(testExactSize(szn::le32(), 4));
		BOOST_CHECK(testExactSize(szn::le64(), 8));

		BOOST_CHECK(testExactSize(szn::boolean(), 1));

		BOOST_CHECK(testMinMaxSize(szn::bytes<szn::le32>(), 4,
		                           std::numeric_limits<length_type>::max()));

		BOOST_CHECK(testMinMaxSize(szn::vector<szn::le32, szn::le8>(), 4,
		                           std::numeric_limits<length_type>::max()));

#if SZN_HAS_UNIQUE_PTR
		BOOST_CHECK(testExactSize(szn::unique_ptr<szn::le64>(), 8));
#endif
	}

	BOOST_AUTO_TEST_CASE(Serialization_bytes_non_pointer)
	{
		std::string generated;
		{
			BOOST_AUTO(sink, make_container_sink(generated));
			szn::bytes<szn::be8> format;
			std::string const value = "value";
			format.serialize(sink, boost::make_iterator_range(value.begin(),
															  value.end()));
		}
		char const expected[] =
		{
		    5, 'v', 'a', 'l', 'u', 'e'
		};
		BOOST_CHECK_EQUAL(generated, std::string(expected, 6));
	}

	BOOST_AUTO_TEST_CASE(Serialization_bytes_input_iterator_success)
	{
		std::string generated;
		{
			BOOST_AUTO(sink, make_container_sink(generated));
			szn::bytes<szn::be8> format;
			std::stringstream value;
			value << "value REST";
			format.serialize(sink,
			                 boost::make_iterator_range(
			                     std::istreambuf_iterator<char>(value),
			                     std::istreambuf_iterator<char>()),
			                 6);
		}
		char const expected[] =
		{
		    6, 'v', 'a', 'l', 'u', 'e', ' '
		};
		BOOST_CHECK_EQUAL(generated, std::string(expected, 7));
	}

	static void run_bytes_input_fail()
	{
		std::string generated;
		BOOST_AUTO(sink, make_container_sink(generated));
		szn::bytes<szn::be8> format;
		std::stringstream value;
		value << "value";
		format.serialize(sink,
		                 boost::make_iterator_range(
		                     std::istreambuf_iterator<char>(value),
		                     std::istreambuf_iterator<char>()),
		                 6);
	}

	static bool is_bytes_input_fail_exception(std::runtime_error const &)
	{
		return true;
	}

	BOOST_AUTO_TEST_CASE(Serialization_bytes_input_iterator_fail)
	{
		BOOST_CHECK_EXCEPTION(run_bytes_input_fail(),
		                      std::runtime_error,
		                      is_bytes_input_fail_exception);

	}

	BOOST_AUTO_TEST_CASE(Serialization_std_map_serialize)
	{
		std::string generated;
		BOOST_AUTO(sink, make_container_sink(generated));
		szn::vector<szn::le8, szn::pair<szn::bytes<szn::le8>, szn::le16>> format;
		std::map<std::string, boost::uint16_t> value;

		{
			format.serialize(sink, value);
			std::string const expected(1, 0);
			BOOST_CHECK_EQUAL(generated, expected);
		}

		{
			value["abc"] = 0x1234;
			generated.clear();
			format.serialize(sink, value);
			char const expected[] = {1, 3, 'a', 'b', 'c', 0x34, 0x12};
			BOOST_CHECK_EQUAL(generated, std::string(expected, 7));
		}

		{
			value["def"] = 0x5678;
			generated.clear();
			format.serialize(sink, value);
			char const expected[] = {2, 3, 'a', 'b', 'c', 0x34, 0x12, 3, 'd', 'e', 'f', 0x78, 0x56};
			BOOST_CHECK_EQUAL(generated, std::string(expected, 13));
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_std_map_deserialize)
	{
		szn::vector<szn::le8, szn::pair<szn::bytes<szn::le8>, szn::le16>> format;
		typedef std::map<std::string, boost::uint16_t> map;

		{
			std::string const data(1, 0);
			BOOST_AUTO(source, szn::make_range_source(data));
			map m;
			BOOST_CHECK(m.empty());
			format.deserialize(source, m);
			BOOST_CHECK(m.empty());
		}

		{
			char const data[] = {3, 1, 'B', 0x22, 0x11, 1, 'A', 0x44, 0x33, 1, 'C', 0x66, 0x55};
			std::string const data_str(data, 13);
			BOOST_AUTO(source, szn::make_range_source(data_str));
			map m;
			BOOST_CHECK(m.empty());
			format.deserialize(source, m);
			BOOST_CHECK_EQUAL(m.size(), 3);
			BOOST_CHECK_EQUAL(m["A"], 0x3344);
			BOOST_CHECK_EQUAL(m["B"], 0x1122);
			BOOST_CHECK_EQUAL(m["C"], 0x5566);
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_optional_serialize)
	{
		std::string generated;
		BOOST_AUTO(sink, make_container_sink(generated));
		typedef szn::optional<szn::boolean, szn::be16> format;

		{
			format().serialize(sink, boost::optional<boost::uint16_t>(0x1122));
			char const expected[] = {1, 0x11, 0x22};
			BOOST_CHECK_EQUAL(generated, std::string(expected, 3));
		}

		{
			generated.clear();
			format().serialize(sink, boost::optional<boost::uint16_t>());
			char const expected[] = {0};
			BOOST_CHECK_EQUAL(generated, std::string(expected, 1));
		}

		{
			generated.clear();
			boost::scoped_ptr<int> const value(new int(0x1234));
			format().serialize(sink, value);
			char const expected[] = {1, 0x12, 0x34};
			BOOST_CHECK_EQUAL(generated, std::string(expected, 3));
		}
	}

	template <class Nullable, class IsSet>
	static void test_optional_deserialize(IsSet const &is_set = IsSet())
	{
		typedef szn::optional<szn::boolean, szn::be16> format;

		{
			char const data[] = {1, 0x11, 0x22};
			std::string const data_str(data, 3);
			BOOST_AUTO(source, make_container_source(data_str));
			Nullable extracted;
			format().deserialize(source, extracted);
			BOOST_REQUIRE(is_set(extracted));
			BOOST_CHECK_EQUAL(*extracted, 0x1122);
		}

		{
			char const data[] = {0};
			std::string const data_str(data, 1);
			BOOST_AUTO(source, make_container_source(data_str));
			Nullable extracted;
			format().deserialize(source, extracted);
			BOOST_CHECK(!is_set(extracted));
		}
	}

	struct ptr_like_is_set
	{
		template <class Nullable>
		bool operator ()(Nullable const &n) const
		{
			return !!n;
		}
	};

	struct auto_ptr_is_set
	{
		template <class Value>
		bool operator ()(std::auto_ptr<Value> const &n) const
		{
			return n.get() != 0;
		}
	};

	BOOST_AUTO_TEST_CASE(Serialization_optional_deserialize)
	{
		test_optional_deserialize<boost::optional<boost::uint16_t>, ptr_like_is_set>();
		test_optional_deserialize<boost::scoped_ptr<boost::uint16_t>, ptr_like_is_set>();
		test_optional_deserialize<std::auto_ptr<boost::uint16_t>, auto_ptr_is_set>();
#if SZN_HAS_UNIQUE_PTR
		test_optional_deserialize<std::unique_ptr<boost::uint16_t>, ptr_like_is_set>();
#endif
	}
}
