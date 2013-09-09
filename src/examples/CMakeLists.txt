file(GLOB codeFiles
	"*.hpp"
	"*.cpp"
)

add_executable(examples ${codeFiles})
target_link_libraries(examples ${Boost_LIBRARIES})
