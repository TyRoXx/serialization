Serialization library for C++
=============================

How to use
----------

The library is currently header-only.
You can find samples of usage in src/examples/ (more to do).

From the first example:
```C++
std::vector<szn::byte> buffer;
auto sink = szn::make_container_sink(buffer);
szn::be64().serialize(sink, 0x12345678);
szn::le32().serialize(sink, 0xaabbccdd);
szn::byte const expected[] =
{
	0, 0, 0, 0, 0x12, 0x34, 0x56, 0x78,
	0xdd, 0xcc, 0xbb, 0xaa
};
assert(buffer.size() == sizeof(expected));
assert(std::equal(std::begin(buffer), std::end(buffer), std::begin(expected)));
```


Supported compilers
-------------------

* GCC >= 4.3
* Visual C++ 2008, 2010

GCC 4.2 and Clang have problems with Boost.Preprocessor.  I will try to fix that.
C++11 is recommended, but not required. C++11 is used in the headers by default,
but can be disabled by defining SZN_NO_CPP11.


Dependencies
------------

Some header-only libraries from Boost are required.
In order to run the tests you need the compiled Boost unit test framework.
Any Boost release since 1.48 should work.
We recommend more recent releases though.


Testing
-------

There is a unit test project in src/test. It is built on Boost test.


Things to do
------------

There is a list of ideas in TODO.md


License
-------

You can find the license text in the file LICENSE. It contains the MIT license.
