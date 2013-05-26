Things to do in no particular order
===================================

* try to make iterate() static
* separate reflexion from serialization
* adapt C++ standard naming style
* make Sink and Source parameters template-tized where possible
* improve little/big endian implementations (e.g. fix warnings, add comments)
* "lazy" ranges for deserialization
* make library header-only?
* support old compilers (MSVC9, GCC 4.2, Clang 2.9, ..)
* avoid Boost in the library?
* improve Source concept
* error handling, exceptions, constraints, backtracing
* POD format
* UTF-8/16/32 formats
* formats for std::list, deque, map, set, queue etc.
* formats for std::unique_ptr, auto_ptr, shared_ptr (and the Boost equivalents)
* formats for Boost.Container, Boost.Range, Boost.UUID, Boost.PtrContainer
* a format like boost::optional
* provide usage examples
  * garbage collector
  * custom format
* allow recursion in data structures
* std::istream/ostream in binary mode should be valid Source/Sink
* use C++11 noexcept
* allow custom field properties
* support stateful formats
* format size estimation (possible at compile time?)
* bit-wise serialization
* conditionals for versioning etc. (compile time and run time)
* default values for fields (like C++11 data member initializers)
* find shorter and more distinguishable namings for de/serialization
  * maybe read/write?
* write a manual and introduction
* default value optimization (so that a field only needs 1 bit in the stream if
  it has the default value)
* support for UTF-8 representations of data (for example JSON)
* support for C (maybe with code generation)
* shortcuts for common things
  * SZN_LE8(a) instead of SZN_FIELD(a, std::uint8_t, szn::LE8)
