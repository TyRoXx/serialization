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

	bool empty_source::is_stable() const
	{
		return true;
	}


	void zero_source::load(length_type n)
	{
		(void)n;
	}

	length_type zero_source::size()
	{
		return 1;
	}

	char zero_source::get(length_type index)
	{
		(void)index;
		return 0;
	}

	void zero_source::drop(length_type n)
	{
		(void)n;
	}

	const char *zero_source::data()
	{
		return "";
	}

	bool zero_source::is_stable() const
	{
		return true;
	}
}
