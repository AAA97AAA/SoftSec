FILES = loadconfig.cpp
OBJECTS = $(patsubst %.cpp,$(TEMPDIR)/%.o,$(FILES))
DEPS    = $(patsubst %.cpp,$(TEMPDIR)/%.d,$(FILES))
TARGET	= $(TEMPDIR)/libconfigs.a

INCLUDE	:= $(patsubst %,-I%,. $(GINCLUDE))

.PHONY: all

all: $(TARGET)

$(TEMPDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)