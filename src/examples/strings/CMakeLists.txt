file(GLOB codeFiles
	"*.hpp"
	"*.cpp"
)

add_executable(example_1_strings ${codeFiles})
target_link_libraries(example_1_strings ${Boost_LIBRARIES} ${SERIALIZATION_COMPILER_LIBS})
