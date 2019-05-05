FILES = barrier.cpp messagebox.cpp process.cpp procman.cpp program.cpp resman.cpp runenv.cpp runsys.cpp fileman.cpp path.cpp authman.cpp
OBJECTS = $(patsubst %.cpp,$(TEMPDIR)/%.o,$(FILES))
DEPS    = $(patsubst %.cpp,$(TEMPDIR)/%.d,$(FILES))
TARGET	= $(TEMPDIR)/libcore.a

INCLUDE	:= $(patsubst %,-I%,. $(GINCLUDE))

.PHONY: all

all: $(TARGET)

$(TEMPDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)