set(io_HEADERS
	channel.h
	socket.h
	socketstream.h)

set(io_SOURCES
	channel.cpp
	isocketbuf.cpp
	osocketbuf.cpp
	socket.cpp)

add_library(io STATIC ${io_SOURCES} ${io_HEADERS})
target_include_directories(io
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		${GRASS_INCLUDES}
)