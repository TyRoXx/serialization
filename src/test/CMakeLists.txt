file(GLOB codeFiles
	"*.h"
	"*.cpp"
)

add_executable(test ${codeFiles})
target_link_libraries(test serialization)
