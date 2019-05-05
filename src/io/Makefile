FILES = channel.cpp isocketbuf.cpp osocketbuf.cpp socket.cpp
OBJECTS = $(patsubst %.cpp,$(TEMPDIR)/%.o,$(FILES))
DEPS    = $(patsubst %.cpp,$(TEMPDIR)/%.d,$(FILES))
TARGET	= $(TEMPDIR)/libio.a

INCLUDE	:= $(patsubst %,-I%,. $(GINCLUDE))

.PHONY: all

all: $(TARGET)

$(TEMPDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)