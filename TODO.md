Things to do in no particular order
===================================

* adapt C++ standard naming style
* make Sink and Source parameters template-tized where possible
* "lazy" ranges for deserialization
* make library header-only?
* support old compilers (MSVC9, GCC 4.2, Clang 2.9, ..)
* avoid Boost in the library?
* improve Source concept
* error handling, exceptions, constraints, backtracing
* UTF-8/16/32 formats
* formats for std::list, deque, map, set, queue, vector<bool> etc.
* formats for std::unique_ptr, auto_ptr, shared_ptr, bitset
* formats for Boost.Container, Boost.Range, Boost.UUID, Boost.PtrContainer,
  Boost.DynamicBitset, Boost.SmartPtr
* a format like boost::optional
* provide usage examples
  * garbage collector
  * custom format
* support pointers and recursion in data structures
* std::istream/ostream in binary mode should be valid Source/Sink
* use C++11 noexcept
* allow custom field properties
* support stateful formats
* bit-wise serialization
* conditionals for versioning etc. (compile time and run time)
* default values for fields (like C++11 data member initializers)
* find shorter and more distinguishable namings for de/serialization
  * maybe read/write or load/save
* write a manual and introduction
* default value optimization (so that a field only needs 1 bit in the stream if
  it has the default value)
* support for UTF-8 representations of data (for example JSON)
* investigate object-relational mapping (e.g. SQL)
* support for C (maybe with code generation)
* shortcuts for common things
  * SZN_LE8(a) instead of SZN_FIELD(a, std::uint8_t, szn::LE8)
* syntax v2 transformations (lexically, semantically with templates)
* create syntax v3
  * maybe FIELD(name, (type), (annotations)...)
  * METHOD(name, result, (parameters...), (annotations)...)
* syntax idea
  * (name, format) [(type)]
  * C++ type is infered from the format in most cases
    * (name, szn::UTF8<szn::BE32>) (std::string)
    * (name, szn::BE16)
    * (name, MyStructure)
    * (name, szn::POD<int>)
* ignore trailing comma in v2 field sequence
* polymorphism
* check possible bit field support
* investigate sign extension problems to find safe solutions
* use 64-bit integer for sizes in bits or bytes instead of std::size_t
  * boost::uintmax_t is an option
