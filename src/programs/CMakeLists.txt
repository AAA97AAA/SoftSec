set(programs_HEADERS
	programs.h)

set(programs_SOURCES
	shell.cpp
	ping.cpp
	who.cpp
	mkrm.cpp
	get.cpp
	put.cpp
	grep.cpp
	date.cpp
	ls.cpp)

add_library(programs STATIC ${programs_SOURCES} ${programs_HEADERS})
target_include_directories(programs
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		${GRASS_INCLUDES}
)
