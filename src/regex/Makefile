FILES = AlternationExpression.cpp AnyCharacterExpression.cpp LazyRepeatRangeExpression.cpp LiteralExpression.cpp RegularExpression.cpp RepeatNExpression.cpp RepeatRangeExpression.cpp SearchExpression.cpp parse.cpp
OBJECTS = $(patsubst %.cpp,$(TEMPDIR)/%.o,$(FILES))
DEPS    = $(patsubst %.cpp,$(TEMPDIR)/%.d,$(FILES))
TARGET	= $(TEMPDIR)/libregex.a

INCLUDE	:= $(patsubst %,-I%,. $(GINCLUDE))

.PHONY: all

all: $(TARGET)

$(TEMPDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)