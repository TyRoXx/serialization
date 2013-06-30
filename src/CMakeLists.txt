cmake_minimum_required(VERSION 2.8)

project(serialization)

if(MSVC_VERSION VERSION_LESS 1600)
	set(SZN_NO_CPP11 ON)
else()
	option(SZN_NO_CPP11 "Disable use of C++11 if available for the compiler" OFF)
endif()
if(SZN_NO_CPP11)
	add_definitions("-DSZN_NO_CPP11")
endif()

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
		if(SZN_NO_CPP11)
			set(flags "${flags} -std=gnu++03")
		else()
			set(flags "${flags} -std=gnu++11")
		endif()

		#enable security features (DEP, ASLR)
		#they seem to be disabled by default on MinGW
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--nxcompat -Wl,--dynamicbase")
	else()
		if(NOT SZN_NO_CPP11)
			execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
							OUTPUT_VARIABLE GCC_VERSION)
			if(GCC_VERSION VERSION_LESS 4.3)
				set(SZN_NO_CPP11 ON)
			endif()
		endif()

		if(SZN_NO_CPP11)
			set(flags "${flags} -std=c++98")
		else()
			set(flags "${flags} -std=c++0x")
			message(STATUS "C++0x enabled")
		endif()
	endif()

	list(APPEND CMAKE_CXX_FLAGS ${flags})
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	message(STATUS "Using Clang")
	set(flags "-Wall -pedantic -fmacro-backtrace-limit=0")
	if(NOT SZN_NO_CPP11)
		set(flags "${flags} -std=c++11")
	endif()
	set(CMAKE_CXX_FLAGS ${flags})
endif()

if(MSVC)
	#disable warnings about deprecation
	set(flags "/wd4996")

	#enable concurrency
	set(flags "${flags} /MP")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flags}")

	set(Boost_USE_STATIC_LIBS        ON)
	set(Boost_USE_MULTITHREADED      ON)
	set(Boost_USE_STATIC_RUNTIME     OFF)
	set(BOOST_ALL_DYN_LINK           OFF)
endif()

option(SZN_BOOST_DYNAMIC "change if linker complains about Boost" ON)
if(SZN_BOOST_DYNAMIC AND NOT MSVC)
	add_definitions("-DBOOST_TEST_DYN_LINK")
endif()

find_package(Boost COMPONENTS system unit_test_framework REQUIRED)
if(NOT Boost_FOUND)
	message(FATAL_ERROR "Did not find Boost (required)")
endif()
include_directories(${Boost_INCLUDE_DIRS})

include_directories(".")

add_subdirectory(szn)
add_subdirectory(test)
add_subdirectory(examples)
