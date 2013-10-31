#include <szn/sink.hpp>
#include <szn/source.hpp>
#include <szn/endianness.hpp>
#include <iostream>
#include <string>

template <std::size_t Size>
void convert_all()
{
	std::istream &in = std::cin;
	szn::stream_source source(in);
	szn::stream_sink sink(std::cout);
	for (;;)
	{
		typedef typename szn::integer<Size, szn::big_endian_byte_order> in_format;
		typedef typename szn::integer<Size, szn::little_endian_byte_order> out_format;
		typename in_format::default_type element;
		try
		{
			in_format().deserialize(source, element);
		}
		catch (std::runtime_error const &)
		{
			//currently, this is the only way to detect eof or error
			break;
		}

		out_format().serialize(sink, element);
	}
}

int main(int argc, char **argv)
{
	void (*cv)() = convert_all<2>;
	if (argc >= 2)
	{
		std::string option = argv[1];
		if (option == "8")
		{
			cv = convert_all<1>;
		}
		else if (option == "16")
		{
			cv = convert_all<2>;
		}
		else if (option == "32")
		{
			cv = convert_all<4>;
		}
		else if (option == "64")
		{
			cv = convert_all<8>;
		}
		else
		{
			std::cerr << "Unrecognized option. Possible options are: 8, 16, 32 or 64\n";
			return 1;
		}
	}
	cv();
}
