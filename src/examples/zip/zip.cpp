#include <szn/source.hpp>
#include <szn/endianness.hpp>
#include <szn/structure.hpp>
#include <szn/util.hpp>
#include <iostream>
#include <string>
#include <fstream>

struct local_file_header_begin
{
	SZN_STRUCTURE
	(
		(min_version, szn::le16),
		(flags, szn::le16),
		(compression, szn::le16),
		(modification_time, szn::le16),
		(modification_date, szn::le16),
		(crc32, szn::le32),
		(compressed_size, szn::le32),
		(uncompressed_size, szn::le32)
	)
};

struct local_file_header_lengths
{
	SZN_STRUCTURE
	(
		(name_length, szn::le16),
		(extra_length, szn::le16)
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
		szn::any_structure().deserialize(source, begin);
		local_file_header_lengths lengths;
		szn::any_structure().deserialize(source, lengths);
		szn::read(source, name, lengths.name_length);
		szn::read(source, extra, lengths.extra_length);
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
		header.deserialize(source);
		std::cout << "first file name: " << header.name << '\n'
		          << "compressed size: " << header.begin.compressed_size << '\n'
		          << "uncompressed size: " << header.begin.uncompressed_size << '\n';
	}
	else
	{
		std::cerr << "Unrecognized signature 0x" << std::hex << signature << '\n';
	}
}
