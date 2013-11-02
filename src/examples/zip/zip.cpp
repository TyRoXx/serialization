#include <szn/source.hpp>
#include <szn/endianness.hpp>
#include <szn/structure.hpp>
#include <szn/util.hpp>
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

struct local_file_header_begin
{
	RXN_REFLECT((SZN_ITERATE) (SZN_AUTO_MEMBERS) (PRINT),
	        (min_version, le16),
	        (flags, le16),
	        (compression, le16),
	        (modification_time, le16),
	        (modification_date, le16),
	        (crc32, le32),
	        (compressed_size, le32),
	        (uncompressed_size, le32)
	)
};

struct local_file_header_lengths
{
	SZN_STRUCTURE(
	        (name_length, le16),
	        (extra_length, le16)
	)
};

struct local_file_header
{
	local_file_header_begin begin;
	std::string name;
	std::string extra;

	template <class Source>
	void deserialize(Source &source)
	{
		szn::structure().deserialize(source, begin);
		local_file_header_lengths lengths;
		szn::structure().deserialize(source, lengths);
		szn::read(source, name, lengths.name_length);
		szn::read(source, extra, lengths.extra_length);
	}

	void print(std::ostream &out) const
	{
		begin.print(out);
		out << "name: " << name << '\n';
		out << "extra: " << extra << '\n';
	}
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

	boost::uint32_t const local_file_signature = 0x04034b50;
	if (local_file_signature == signature)
	{
		local_file_header header;
		szn::by_method().deserialize(source, header);
		header.print(std::cout);
	}
	else
	{
		std::cerr << "Unrecognized signature 0x" << std::hex << signature << '\n';
	}
}