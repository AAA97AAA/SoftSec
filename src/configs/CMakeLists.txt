set(configs_HEADERS
	loadconfig.h)

set(configs_SOURCES
	loadconfig.cpp)

add_library(configs STATIC ${configs_SOURCES} ${configs_HEADERS})
target_include_directories(configs
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		${GRASS_INCLUDES}
)
