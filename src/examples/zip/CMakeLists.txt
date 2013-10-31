file(GLOB codeFiles
	"*.hpp"
	"*.cpp"
)

add_executable(example_5_zip ${codeFiles})
target_link_libraries(example_5_zip ${Boost_LIBRARIES} ${SERIALIZATION_COMPILER_LIBS})
