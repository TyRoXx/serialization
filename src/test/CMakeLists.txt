file(GLOB codeFiles
	"../szn/*.hpp"
	"../rxn/*.hpp"
	"*.cpp"
)

add_executable(test ${codeFiles})
target_link_libraries(test ${Boost_LIBRARIES} ${SERIALIZATION_COMPILER_LIBS})
