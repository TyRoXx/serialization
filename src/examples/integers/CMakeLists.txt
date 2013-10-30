file(GLOB codeFiles
	"*.hpp"
	"*.cpp"
)

add_executable(example_0_integers ${codeFiles})
target_link_libraries(example_0_integers ${Boost_LIBRARIES} ${SERIALIZATION_COMPILER_LIBS})
