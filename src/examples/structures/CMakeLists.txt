file(GLOB codeFiles
	"*.hpp"
	"*.cpp"
)

add_executable(example_3_structures ${codeFiles})
target_link_libraries(example_3_structures ${Boost_LIBRARIES} ${SERIALIZATION_COMPILER_LIBS})
