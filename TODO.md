
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
* provide usage examples
  * garbage collector
  * custom format
* allow recursion in data structures
* std::istream/ostream in binary mode should be valid Source/Sink
* use C++11 noexcept
* do internal includes like <szn/util.h> instead of "util.h"
