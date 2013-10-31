file(GLOB codeFiles
	"*.hpp"
	"*.cpp"
)

add_executable(example_4_convert ${codeFiles})
target_link_libraries(example_4_convert ${Boost_LIBRARIES} ${SERIALIZATION_COMPILER_LIBS})
