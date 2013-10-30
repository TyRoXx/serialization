file(GLOB codeFiles
	"*.hpp"
	"*.cpp"
)

add_executable(example_2_maps ${codeFiles})
target_link_libraries(example_2_maps ${Boost_LIBRARIES} ${SERIALIZATION_COMPILER_LIBS})
