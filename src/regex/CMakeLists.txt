set(regex_HEADERS
	AlternationExpression.h
	AnyCharacterExpression.h
	LazyRepeatRangeExpression.h
	LiteralExpression.h
	RegularExpression.h
	RepeatNExpression.h
	RepeatRangeExpression.h
	SearchExpression.h
	re.h)

set(regex_SOURCES
	AlternationExpression.cpp
	AnyCharacterExpression.cpp
	LazyRepeatRangeExpression.cpp
	LiteralExpression.cpp
	RegularExpression.cpp
	RepeatNExpression.cpp
	RepeatRangeExpression.cpp
	SearchExpression.cpp
	parse.cpp)

add_library(regex STATIC ${regex_SOURCES} ${regex_HEADERS})
target_include_directories(regex
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
		${GRASS_INCLUDES}
)