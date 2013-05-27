file(GLOB codeFiles
	"*.hpp"
	"*.cpp"
)

add_library(serialization ${codeFiles})
target_link_libraries(serialization ${Boost_LIBRARIES})

