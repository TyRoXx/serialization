#include "sink.hpp"


namespace szn
{
	Sink::~Sink()
	{
	}

	void NullSink::write(const char *data, std::size_t n)
	{
		(void)data;
		(void)n;
	}
}
