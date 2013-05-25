file(GLOB codeFiles
	"*.h"
	"*.cpp"
)

add_library(serialization ${codeFiles})
target_link_libraries(serialization ${Boost_LIBRARIES})
