set(core_HEADERS
	barrier.h
	messagebox.h
	process.h
	procman.h
	program.h
	resman.h
	runenv.h
	runsys.h
	fileman.h
	path.h
	authman.h)

set(core_SOURCES
	barrier.cpp
	messagebox.cpp
	process.cpp
	procman.cpp
	program.cpp
	resman.cpp
	runenv.cpp
	runsys.cpp
	fileman.cpp
	path.cpp
	authman.cpp)

add_library(core STATIC ${core_SOURCES} ${core_HEADERS})
target_include_directories(core
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		${GRASS_INCLUDES}
)