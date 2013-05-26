cmake_minimum_required(VERSION 2.8)

project(serialization)

if(CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "Using GCC")

	#enable many useful warnings
	set(flags "-Wall")

	#check usage of printf-like functions
	set(flags "${flags} -Wformat-security")

	#warn about implicit conversions
	set(flags "${flags} -Wconversion")

	#some more warnings (which are not always useful)
	set(flags "${flags} -Wextra")

	#extra strictness
	set(flags "${flags} -pedantic")

	if(UNIX)
		set(flags "${flags} -pthread")
	endif()

	if(MINGW)
		#otherwise MinGW complains about undeclared "swprintf"
		set(flags "${flags} -std=gnu++11")

		#enable security features (DEP, ASLR)
		#they seem to be disabled by default on MinGW
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--nxcompat -Wl,--dynamicbase")
	else()
		set(flags "${flags} -std=c++0x")
	endif()

	list(APPEND CMAKE_CXX_FLAGS ${flags})
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	message(STATUS "Using Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")
endif()

option(SZN_BOOST_DYNAMIC "change if linker complains about Boost" ON)
if(SZN_BOOST_DYNAMIC)
	add_definitions("-DBOOST_TEST_DYN_LINK")
endif()

find_package(Boost REQUIRED system unit_test_framework)
if(NOT Boost_FOUND)
	message(FATAL_ERROR "Did not find Boost (required)")
endif()
include_directories(${Boost_INCLUDE_DIRS})

include_directories(".")

add_subdirectory(szn)
add_subdirectory(test)
add_subdirectory(examples)
