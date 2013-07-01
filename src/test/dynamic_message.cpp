#include <boost/test/unit_test.hpp>

#include <szn/writer.hpp>
#include <szn/big_endian.hpp>

namespace szn
{
	BOOST_AUTO_TEST_CASE(Serialization_dynamic_message)
	{
		std::string generated;
		auto sink = make_container_sink(generated);

		szn::writer(sink)
			.be32(12)
			.bytes<be16>("hello")
			.boolean(true)
			;

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
