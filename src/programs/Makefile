FILES = date.cpp get.cpp grep.cpp ls.cpp mkrm.cpp ping.cpp put.cpp shell.cpp who.cpp
OBJECTS = $(patsubst %.cpp,$(TEMPDIR)/%.o,$(FILES))
DEPS    = $(patsubst %.cpp,$(TEMPDIR)/%.d,$(FILES))
TARGET	= $(TEMPDIR)/libprograms.a

INCLUDE	:= $(patsubst %,-I%,. $(GINCLUDE))

.PHONY: all

all: $(TARGET)

$(TEMPDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)