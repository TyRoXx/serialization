#include <szn/source.hpp>
#include <szn/endianness.hpp>
#include <szn/structure.hpp>
#include <iostream>
#include <string>
#include <fstream>

using namespace szn;

#define PRINT_ELEMENT(name, format, annotations) \
	out << BOOST_STRINGIZE(name) << ": " << name << '\n';

#define PRINT(fields) \
	void print(std::ostream &out) const { \
		BOOST_PP_SEQ_FOR_EACH_I(RXN_GENERATE_FIELD, PRINT_ELEMENT, fields) \
	}

struct local_file_header
{
	RXN_REFLECT((SZN_ITERATE) (SZN_AUTO_MEMBERS) (PRINT),
	        (min_version, le16),
	        (flags, le16),
	        (compression, le16),
	        (modification_time, le16),
	        (modification_date, le16),
	        (crc32, le32),
	        (compressed_size, le32),
	        (uncompressed_size, le32),
	        (name_length, le16),
	        (extra_length, le16)
	)
};

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		return 1;
	}

	std::ifstream file(argv[1], std::ios::binary);
	if (!file)
	{
		std::cerr << "Cannot open the file\n";
		return 1;
	}

	szn::stream_source source(file);
	boost::uint32_t signature;
	szn::le32().deserialize(source, signature);

	if (0x04034b50 == signature)
	{
		local_file_header header;
		szn::structure().deserialize(source, header);
		header.print(std::cout);
	}
	else
	{
		std::cerr << "Unrecognized signature 0x" << std::hex << signature << '\n';
	}
}
