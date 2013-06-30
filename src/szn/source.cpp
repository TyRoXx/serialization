#include "source.hpp"


namespace szn
{
	source::~source()
	{
	}


	void empty_source::load(std::size_t)
	{
	}

	std::size_t empty_source::size()
	{
		return 0;
	}

	char empty_source::get(std::size_t)
	{
		throw std::runtime_error("szn::EmptySource cannot be read from");
	}

	void empty_source::drop(std::size_t)
	{
	}

	const char *empty_source::data()
	{
		return NULL;
	}
}
