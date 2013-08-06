Things to do in no particular order
===================================

* make Sink and Source parameters template-tized where possible
* "lazy" ranges for deserialization
* make library header-only?
* support old compilers (GCC 4.2, Clang 2.9, ..)
* improve Source concept
* error handling, exceptions, constraints, backtracing
* UTF-8/16/32 formats
* formats for std::list, deque, set, queue, vector<bool> etc.
* formats for bitset
* formats for Boost.Container, Boost.UUID, Boost.PtrContainer,
  Boost.DynamicBitset
* provide usage examples
  * improve garbage collection example
  * custom format
* support pointers and recursion in data structures
* std::istream/ostream in binary mode should be valid Source/Sink
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
* ignore trailing comma in v2 field sequence
* polymorphism
* check possible bit field support
* investigate sign extension problems to find safe solutions
* RXN_REFLECT should support an empty field list
