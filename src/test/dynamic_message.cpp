#include <boost/test/unit_test.hpp>

#include <szn/bytes.hpp>
#include <szn/big_endian.hpp>
#include <szn/bool.hpp>

namespace szn
{
	namespace
	{
	}

	BOOST_AUTO_TEST_CASE(Serialization_dynamic_message)
	{
		std::string generated;
		auto sink = makeContainerSink(generated);

		//this should be possible with less typing:
		szn::serialize(sink, 12, szn::BE32());
		szn::serialize(sink, "hello", szn::Bytes<szn::BE16>());
		szn::serialize(sink, true, szn::Bool());

		/* //ideas:
		szn::writer(sink)
			.be32(12)
		    .be<32>(12)
			.bytes16("hello")
		    .utf8<16>("hello")
			.boolean(true)
		    .bool_(true)
			;
		*/

		char const expectedBytes[] =
		{
		    0, 0, 0, 12,
		    0, 5, 'h', 'e', 'l', 'l', 'o',
		    1
		};
		BOOST_CHECK_EQUAL(generated,
		                  std::string(expectedBytes, sizeof(expectedBytes)));
	}
}
