#include "source.hpp"


namespace szn
{
	Source::~Source()
	{
	}


	void EmptySource::load(std::size_t)
	{
	}

	std::size_t EmptySource::size()
	{
		return 0;
	}

	char EmptySource::get(std::size_t)
	{
		throw std::runtime_error("szn::EmptySource cannot be read from");
	}

	void EmptySource::drop(std::size_t)
	{
	}

	const char *EmptySource::data()
	{
		return nullptr;
	}
}
