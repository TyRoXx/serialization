#include "sink.hpp"


namespace szn
{
	sink::~sink()
	{
	}

	void null_sink::write(const char *data, std::size_t n)
	{
		(void)data;
		(void)n;
	}
}
