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
#include <szn/map.hpp>
#include <szn/iterator.hpp>
#include <szn/reader.hpp>
#include <szn/writer.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <sstream>
#include <list>
#include <vector>
#include <deque>
#include <map>
#include <set>
#if SZN_HAS_UNORDERED
#	include <unordered_map>
#	include <unordered_set>
#endif

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
		bool serialization_roundtrip(Value const &value,
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
		BOOST_CHECK_EQUAL(b.le16_, 0xffffu);
		BOOST_CHECK_EQUAL(b.le8_, 7);
		BOOST_CHECK_EQUAL(b.be32_, 0x01010101);
		BOOST_CHECK_EQUAL(b.be16_, 0x8000u);
		BOOST_CHECK_EQUAL(b.be8_, 0xffu);
		BOOST_CHECK_EQUAL(b.str16, "hallo");
		BOOST_CHECK_EQUAL(b.str8, "");
		BOOST_REQUIRE_EQUAL(b.vec8.size(), 2u);
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

	BOOST_AUTO_TEST_CASE(Serialization_by_adl)
	{
		szn::null_sink sink;
		for_adl_tests::Wrapper w;
		w.c.ok = true;
		szn::serialize(sink, w, szn::by_method());
	}

	BOOST_AUTO_TEST_CASE(Serialization_array_std_array)
	{
		const boost::array<boost::uint16_t, 2> test_array =
		{{
			0x1122, 0x3344
		}};
		szn::array<2, szn::be16> format;

		BOOST_CHECK(serialization_roundtrip(test_array, format));

		std::vector<unsigned char> generated;
		BOOST_AUTO(sink, szn::make_container_sink(generated));
		szn::serialize(sink, test_array, format);

		BOOST_REQUIRE(generated.size() == 4);
		BOOST_CHECK(generated[0] == 0x11);
		BOOST_CHECK(generated[1] == 0x22);
		BOOST_CHECK(generated[2] == 0x33);
		BOOST_CHECK(generated[3] == 0x44);
	}

	BOOST_AUTO_TEST_CASE(Serialization_array_c_array)
	{
		const boost::uint16_t test_array[2] =
		{
			0x1122, 0x3344
		};
		typedef szn::array<2, szn::be16> ArrayFormat;

		BOOST_CHECK(serialization_roundtrip(test_array, ArrayFormat()));

		std::vector<unsigned char> generated;
		BOOST_AUTO(sink, szn::make_container_sink(generated));
		szn::serialize(sink, test_array, ArrayFormat());

		BOOST_REQUIRE(generated.size() == 4);
		BOOST_CHECK(generated[0] == 0x11);
		BOOST_CHECK(generated[1] == 0x22);
		BOOST_CHECK(generated[2] == 0x33);
		BOOST_CHECK(generated[3] == 0x44);
	}

	BOOST_AUTO_TEST_CASE(Serialization_array_pair_serialize)
	{
		std::string generated;
		BOOST_AUTO(sink, szn::make_container_sink(generated));
		szn::array<2, szn::be8> format;
		format.serialize(sink, std::make_pair('A', 'B'));
		BOOST_CHECK_EQUAL("AB", generated);
	}

	BOOST_AUTO_TEST_CASE(Serialization_array_pair_deserialize)
	{
		std::string const data_str = "AB";
		BOOST_AUTO(source, szn::make_container_source(data_str));
		std::pair<char, char> extracted;
		szn::array<2, szn::be8> format;
		format.deserialize(source, extracted);
		BOOST_CHECK(std::make_pair('A', 'B') == extracted);
	}

	namespace
	{
		SZN_BEGIN2(no_methods)
			SZN_FIELD(i, boost::uint32_t, szn::le32)
		SZN_END()
	}

	BOOST_AUTO_TEST_CASE(Serialization_no_methods)
	{
		std::vector<unsigned char> generated;
		{
			no_methods no;
			no.i = 123;
			BOOST_AUTO(sink, szn::make_container_sink(generated));
			szn::structure().serialize(sink, no);
		}

		no_methods no;
		{
			BOOST_AUTO(source, szn::make_range_source(generated));
			szn::structure().deserialize(source, no);
		}

		BOOST_CHECK(no.i == 123);
	}

	namespace
	{
		SZN_BEGIN2(iteration_tester)
			SZN_FIELD(fieldA, int, szn::be32)
			SZN_FIELD(fieldB, long, szn::le32)
			SZN_FIELD(fieldC, char, szn::le8)
		SZN_END()

		struct iteration_test_visitor SZN_FINAL
		{
			explicit iteration_test_visitor(iteration_tester &tester)
				: m_tester(tester)
			{
			}

			void visit_field(int iteration_tester::*a, szn::be32, const char *name)
			{
				BOOST_CHECK(m_tester.*a == 2);
				BOOST_CHECK(std::string(name) == "fieldA");
			}

			void visit_field(long iteration_tester::*b, szn::le32, const char *name)
			{
				BOOST_CHECK(m_tester.*b == 7);
				BOOST_CHECK(std::string(name) == "fieldB");
			}

			void visit_field(char iteration_tester::*c, szn::le8, const char *name)
			{
				BOOST_CHECK(m_tester.*c == '!');
				BOOST_CHECK(std::string(name) == "fieldC");
			}

		private:

			iteration_tester const &m_tester;
		};
	}

	BOOST_AUTO_TEST_CASE(Serialization_struct_iterate)
	{
		iteration_tester tester;
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
		SZN_BEGIN2(bytes_tester)
			SZN_FIELD(str, std::string, szn::bytes<szn::be64>)
			SZN_FIELD(vec, std::vector<signed char>, szn::bytes<szn::be64>)
			SZN_FIELD(range, Range, szn::bytes<szn::be64>)
		SZN_END()
	}

	BOOST_AUTO_TEST_CASE(Serialization_bytes)
	{
		typedef std::pair<const char *, const char *> ConstStringRange;
		const std::string range_pointee = "range";

		std::vector<signed char> generated;
		{
			bytes_tester<ConstStringRange> tester;
			tester.str = "hallo";
			tester.vec.push_back(42);
			tester.range = std::make_pair(range_pointee.data(),
										  range_pointee.data() + range_pointee.size());
			BOOST_AUTO(sink, szn::make_container_sink(generated));
			szn::structure().serialize(sink, tester);
		}

		bytes_tester<ConstStringRange> tester;
		{
			BOOST_AUTO(source, szn::make_range_source(generated));
			szn::structure().deserialize(source, tester);
		}

		BOOST_CHECK(tester.str == "hallo");
		BOOST_CHECK(tester.vec.size() == 1);
		BOOST_CHECK(tester.vec.at(0) == 42);
		BOOST_CHECK(range_pointee == std::string(tester.range.first, tester.range.second));
	}

	BOOST_AUTO_TEST_CASE(Serialization_Bool)
	{
		BOOST_CHECK(serialization_roundtrip(true,  szn::boolean()));
		BOOST_CHECK(serialization_roundtrip(false, szn::boolean()));

		BOOST_CHECK(serialization_roundtrip(true,  szn::boolean_not<szn::boolean>()));
		BOOST_CHECK(serialization_roundtrip(false, szn::boolean_not<szn::boolean>()));
	}

#if SZN_HAS_UNIQUE_PTR
	BOOST_AUTO_TEST_CASE(Serialization_UniquePtr)
	{
		BOOST_CHECK(serialization_roundtrip(
						std::unique_ptr<long>(new long(123)),
						szn::unique_ptr<szn::le32>()));
	}
#endif

	namespace
	{
		struct test_pod
		{
			std::size_t field;

			test_pod()
			{
			}

			test_pod(std::size_t field)
				: field(field)
			{
			}
		};

		bool operator == (test_pod const &left, test_pod const &right)
		{
			return (left.field == right.field);
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_pod)
	{
#define SZNTEST_POD(type) BOOST_CHECK(serialization_roundtrip<type>(13, szn::pod<type>()));
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
		SZNTEST_POD(test_pod)
#undef SZNTEST_POD

		{
			boost::array<int, 3> array = {{1, 2, 3}};
			BOOST_CHECK(serialization_roundtrip(array, szn::pod<boost::array<int, 3> >()));
		}

		{
			int dummy;
			int * const pointer = &dummy;
			BOOST_CHECK(serialization_roundtrip(pointer, szn::pod<int *>()));
			BOOST_CHECK(serialization_roundtrip(&pointer, szn::pod<int * const *>()));
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_little_endian)
	{
		BOOST_CHECK(serialization_roundtrip<boost::int8_t>(-50, szn::le8()));
		BOOST_CHECK(serialization_roundtrip<boost::int16_t>(-50, szn::le16()));
		BOOST_CHECK(serialization_roundtrip<boost::int32_t>(-50, szn::le32()));
		BOOST_CHECK(serialization_roundtrip<boost::int64_t>(-50, szn::le64()));

		BOOST_CHECK(serialization_roundtrip(-50, szn::le8()));
		BOOST_CHECK(serialization_roundtrip(-50, szn::le16()));
		BOOST_CHECK(serialization_roundtrip(-50, szn::le32()));
		BOOST_CHECK(serialization_roundtrip(-50, szn::le64()));
	}

	BOOST_AUTO_TEST_CASE(Serialization_big_endian)
	{
		BOOST_CHECK(serialization_roundtrip<boost::int8_t>(-50, szn::be8()));
		BOOST_CHECK(serialization_roundtrip<boost::int16_t>(-50, szn::be16()));
		BOOST_CHECK(serialization_roundtrip<boost::int32_t>(-50, szn::be32()));
		BOOST_CHECK(serialization_roundtrip<boost::int64_t>(-50, szn::be64()));

		BOOST_CHECK(serialization_roundtrip(-50, szn::be8()));
		BOOST_CHECK(serialization_roundtrip(-50, szn::be16()));
		BOOST_CHECK(serialization_roundtrip(-50, szn::be32()));
		BOOST_CHECK(serialization_roundtrip(-50, szn::be64()));
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
		BOOST_CHECK(serialization_roundtrip(Zero, szn:: BOOST_PP_CAT(endianness, bitsize) ())); \
		BOOST_CHECK(serialization_roundtrip(NonZero, szn:: BOOST_PP_CAT(endianness, bitsize) ())); \
		BOOST_CHECK(serialization_roundtrip(Ones, szn:: BOOST_PP_CAT(endianness, bitsize) ()));
		
#define SZNTEST_ENUM_VALUES2(endianness) \
		SZNTEST_ENUM_VALUES(endianness, 8) \
		SZNTEST_ENUM_VALUES(endianness, 16) \
		SZNTEST_ENUM_VALUES(endianness, 32) \
		SZNTEST_ENUM_VALUES(endianness, 64)

		SZNTEST_ENUM_VALUES2(le)
		SZNTEST_ENUM_VALUES2(be)

#undef SZNTEST_ENUM_VALUES2
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
		BOOST_CHECK(serialization_roundtrip(TestEnumClass::Zero, szn:: BOOST_PP_CAT(endianness, bitsize) ())); \
		BOOST_CHECK(serialization_roundtrip(TestEnumClass::NonZero, szn:: BOOST_PP_CAT(endianness, bitsize) ())); \
		BOOST_CHECK(serialization_roundtrip(TestEnumClass::Ones, szn:: BOOST_PP_CAT(endianness, bitsize) ()));

#define SZNTEST_ENUM_VALUES2(endianness) \
		SZNTEST_ENUM_VALUES(endianness, 8) \
		SZNTEST_ENUM_VALUES(endianness, 16) \
		SZNTEST_ENUM_VALUES(endianness, 32) \
		SZNTEST_ENUM_VALUES(endianness, 64)

		SZNTEST_ENUM_VALUES2(le)
		SZNTEST_ENUM_VALUES2(be)

#undef SZNTEST_ENUM_VALUES2
#undef SZNTEST_ENUM_VALUES
	}
#endif

	namespace
	{
		template <class Format>
		bool test_min_max_size(Format const &, length_type expected_min, length_type expected_max)
		{
			length_type const min = szn::min_size<Format>::value;
			length_type const max = szn::max_size<Format>::value;
			return (min == expected_min) && (max == expected_max);
		}

		template <class Format>
		bool test_exact_size(Format const &format, length_type expected_size)
		{
			return test_min_max_size(format, expected_size, expected_size);
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_min_size)
	{
		BOOST_CHECK(test_exact_size(szn::be8(), 1));
		BOOST_CHECK(test_exact_size(szn::be16(), 2));
		BOOST_CHECK(test_exact_size(szn::be32(), 4));
		BOOST_CHECK(test_exact_size(szn::be64(), 8));

		BOOST_CHECK(test_exact_size(szn::le8(), 1));
		BOOST_CHECK(test_exact_size(szn::le16(), 2));
		BOOST_CHECK(test_exact_size(szn::le32(), 4));
		BOOST_CHECK(test_exact_size(szn::le64(), 8));

		BOOST_CHECK(test_exact_size(szn::boolean(), 1));

#if SZN_HAS_UNIQUE_PTR
		BOOST_CHECK(test_exact_size(szn::unique_ptr<szn::le64>(), 8));
#endif

		BOOST_CHECK(test_min_max_size(szn::bytes<szn::le32>(), 4,
		                           std::numeric_limits<length_type>::max()));

		BOOST_CHECK(test_min_max_size(szn::vector<szn::le32, szn::le8>(), 4,
								   std::numeric_limits<length_type>::max()));
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

	BOOST_AUTO_TEST_CASE(Serialization_pair_serialize)
	{
		std::string generated;
		BOOST_AUTO(sink, make_container_sink(generated));
		typedef szn::pair<szn::le8, szn::le8> in_order;
		typedef szn::pair<szn::le8, szn::le8, szn::pair_order::second_first> reversed;
		in_order().serialize(sink, std::make_pair('a', 'b'));
		reversed().serialize(sink, std::make_pair('c', 'd'));
		BOOST_CHECK_EQUAL("abdc", generated);
	}

	BOOST_AUTO_TEST_CASE(Serialization_pair_deserialize)
	{
		std::string const data = "VWXY";
		BOOST_AUTO(source, szn::make_range_source(data));
		typedef szn::pair<szn::le8, szn::le8, szn::pair_order::second_first> reversed;
		typedef szn::pair<szn::le8, szn::le8> in_order;
		std::pair<char, char> wv, xy;
		reversed().deserialize(source, wv);
		in_order().deserialize(source, xy);
		BOOST_CHECK(wv == std::make_pair('W', 'V'));
		BOOST_CHECK(xy == std::make_pair('X', 'Y'));
	}

	BOOST_AUTO_TEST_CASE(Serialization_std_map_serialize)
	{
		szn::map<szn::le8, szn::bytes<szn::le8>, szn::le16> format;
		std::string generated;
		BOOST_AUTO(sink, make_container_sink(generated));
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
		szn::map<szn::le8, szn::bytes<szn::le8>, szn::le16> format;
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
			BOOST_CHECK_EQUAL(m.size(), 3u);
			BOOST_CHECK_EQUAL(m["A"], 0x3344);
			BOOST_CHECK_EQUAL(m["B"], 0x1122);
			BOOST_CHECK_EQUAL(m["C"], 0x5566);
		}
	}

	template <class Ptr>
	static void serialize_ptr()
	{
		std::string generated;
		BOOST_AUTO(sink, make_container_sink(generated));
		typedef szn::optional<szn::boolean, szn::be16> format;
		Ptr const value(new int(0x1234));
		format().serialize(sink, value);
		char const expected[] = {1, 0x12, 0x34};
		BOOST_CHECK_EQUAL(generated, std::string(expected, 3));
	}

	BOOST_AUTO_TEST_CASE(Serialization_optional_serialize)
	{
		{
			std::string generated;
			BOOST_AUTO(sink, make_container_sink(generated));
			typedef szn::optional<szn::boolean, szn::be16> format;

			{
				format().serialize(sink, boost::optional<boost::uint16_t>(0x1122u));
				char const expected[] = {1, 0x11, 0x22};
				BOOST_CHECK_EQUAL(generated, std::string(expected, 3));
			}

			{
				generated.clear();
				format().serialize(sink, boost::optional<boost::uint16_t>());
				char const expected[] = {0};
				BOOST_CHECK_EQUAL(generated, std::string(expected, 1));
			}
		}

		serialize_ptr<boost::scoped_ptr<int> >();
		serialize_ptr<boost::shared_ptr<int> >();
		serialize_ptr<std::auto_ptr<int> >();

#if SZN_HAS_UNIQUE_PTR
		serialize_ptr<std::unique_ptr<int>>();
#endif

#if SZN_HAS_SHARED_PTR
		serialize_ptr<std::shared_ptr<int>>();
#endif
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
		test_optional_deserialize<boost::shared_ptr<boost::uint16_t>, ptr_like_is_set>();
		test_optional_deserialize<std::auto_ptr<boost::uint16_t>, auto_ptr_is_set>();
#if SZN_HAS_UNIQUE_PTR
		test_optional_deserialize<std::unique_ptr<boost::uint16_t>, ptr_like_is_set>();
#endif
#if SZN_HAS_SHARED_PTR
		test_optional_deserialize<std::shared_ptr<boost::uint16_t>, ptr_like_is_set>();
#endif
	}

	template <class Container>
	static void serialize_vector()
	{
		szn::vector<szn::be16, szn::be16> format;
		std::string generated;
		BOOST_AUTO(sink, make_container_sink(generated));
		Container const values = boost::assign::list_of(2)(3)(5)(7);
		format.serialize(sink, values);
		char const expected[] = {0, 4, 0, 2, 0, 3, 0, 5, 0, 7};
		BOOST_CHECK_EQUAL(std::string(expected, 10), generated);
	}

	template <class Container>
	static void serialize_vector_roundtrip()
	{
		szn::vector<szn::be16, szn::be16> format;
		Container const values = boost::assign::list_of(2)(3)(5)(7);
		std::string generated;
		{
			BOOST_AUTO(sink, make_container_sink(generated));
			format.serialize(sink, values);
		}
		BOOST_AUTO(source, make_container_source(generated));
		Container extracted;
		format.deserialize(source, extracted);
		BOOST_CHECK(values == extracted);
	}

	BOOST_AUTO_TEST_CASE(Serialization_vector_serialize)
	{
		serialize_vector<std::vector<boost::uint16_t> >();
		serialize_vector<std::list<boost::uint16_t> >();
		serialize_vector<std::deque<boost::uint16_t> >();
		serialize_vector<boost::array<boost::uint16_t, 4> >();
		serialize_vector<std::set<boost::uint16_t> >();
		serialize_vector_roundtrip<boost::unordered_set<boost::uint16_t> >();
#if SZN_HAS_UNORDERED
		serialize_vector_roundtrip<std::unordered_set<boost::uint16_t> >();
#endif
		//TODO: std::array, boost containers
	}

	template <class Container>
	static void deserialize_vector()
	{
		char const data[] = {3, 1, 2, 3};
		std::string const data_str(data, 4);
		BOOST_AUTO(source, make_container_source(data_str));
		szn::vector<szn::be8, szn::be8> format;
		Container extracted;
		format.deserialize(source, extracted);
		Container const expected = boost::assign::list_of(1)(2)(3);
		BOOST_CHECK(expected == extracted);
	}

	BOOST_AUTO_TEST_CASE(Serialization_vector_deserialize)
	{
		deserialize_vector<std::vector<boost::uint16_t> >();
		deserialize_vector<std::list<boost::uint16_t> >();
		deserialize_vector<std::deque<boost::uint16_t> >();
		deserialize_vector<boost::array<boost::uint16_t, 3> >();
		deserialize_vector<std::set<boost::uint16_t> >();
		deserialize_vector<boost::unordered_set<boost::uint16_t> >();
#if SZN_HAS_UNORDERED
		deserialize_vector<std::unordered_set<boost::uint16_t> >();
#endif
		//TODO: std::array, boost containers
		//TODO: test error handling on wrong array size
	}

	BOOST_AUTO_TEST_CASE(Serialization_vector_pair_serialize)
	{
		szn::vector<szn::be16, szn::be16> format;
		std::string generated;
		BOOST_AUTO(sink, make_container_sink(generated));
		format.serialize(sink, std::make_pair(0x4455, 0x6677));
		char const expected[] = {0, 2, 0x44, 0x55, 0x66, 0x77};
		BOOST_CHECK_EQUAL(std::string(expected, 6), generated);
	}

	BOOST_AUTO_TEST_CASE(Serialization_deserializing_iterator)
	{
		//TODO make the iterator work without the extra 'E' at the end
		std::string const data_str = "ABCDE";
		BOOST_AUTO(source, make_container_source(data_str));
		BOOST_AUTO(it, (make_deserializing_iterator<szn::be8, char>(source)));
		std::string extracted;
		std::copy_n(it, 4, std::back_inserter(extracted));
		BOOST_CHECK_EQUAL("ABCD", extracted);
	}

	template <class CountingSink>
	static void test_counting_sink(CountingSink &sink)
	{
		BOOST_CHECK_EQUAL(0, sink.count());
		sink.write(0, 0);
		BOOST_CHECK_EQUAL(0, sink.count());
		sink.write("data", 4);
		BOOST_CHECK_EQUAL(4, sink.count());
		sink.write("", 1);
		BOOST_CHECK_EQUAL(5, sink.count());
	}

	BOOST_AUTO_TEST_CASE(Serialization_counting_sink)
	{
		{
			szn::null_sink null;
			szn::counting_sink_adaptor<szn::null_sink> sink(null);
			test_counting_sink(sink);
		}

		{
			szn::counting_sink sink;
			test_counting_sink(sink);
		}
	}

	BOOST_AUTO_TEST_CASE(Serialization_zero_source)
	{
		zero_source source;
		boost::uint8_t a = 1;
		boost::uint16_t b = 2;
		boost::uint32_t c = 3;
		boost::uint64_t d = 0;
		reader(source).be8(a).be16(b).be32(c).be64(d);
		BOOST_CHECK_EQUAL(0, a);
		BOOST_CHECK_EQUAL(0, b);
		BOOST_CHECK_EQUAL(0, c);
		BOOST_CHECK_EQUAL(0, d);
	}

	BOOST_AUTO_TEST_CASE(Serialization_stream_source)
	{
		std::istringstream stream;
		stream.str("Hallo");
		szn::stream_source source(stream);
		source.load(10000);
		BOOST_CHECK_EQUAL(5, source.size());
		BOOST_CHECK_EQUAL("Hallo", std::string(source.data(), static_cast<size_t>(source.size())));
		BOOST_CHECK_EQUAL(false, source.is_stable());
		source.drop(2);
		BOOST_CHECK_EQUAL('l', source.get(0));
		BOOST_CHECK_EQUAL('l', source.get(1));
		BOOST_CHECK_EQUAL('o', source.get(2));
		source.drop(3);
		BOOST_CHECK_EQUAL(0, source.size());
		source.load(10000);
		BOOST_CHECK_EQUAL(0, source.size());
	}

	BOOST_AUTO_TEST_CASE(Serialization_stream_sink)
	{
		std::ostringstream stream;
		szn::stream_sink sink(stream);
		sink.write("Hallo", 5);
		BOOST_CHECK_EQUAL("Hallo", stream.str());
	}
}
