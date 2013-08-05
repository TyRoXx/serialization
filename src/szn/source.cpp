#include "source.hpp"


namespace szn
{
	source::~source()
	{
	}


	void empty_source::load(length_type)
	{
	}

	length_type empty_source::size()
	{
		return 0;
	}

	char empty_source::get(length_type)
	{
		throw std::runtime_error("szn::EmptySource cannot be read from");
	}

	void empty_source::drop(length_type)
	{
	}

	const char *empty_source::data()
	{
		return NULL;
	}
}
