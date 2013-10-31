Serialization library for C++
=============================

How to use
----------

You can find samples of usage in src/examples/ (more to do).


Supported compilers
-------------------

* GCC >= 4.3
* Visual C++ 2008, 2010

GCC 4.2 and Clang have problems with Boost.Preprocessor.  I will try to fix that.


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
