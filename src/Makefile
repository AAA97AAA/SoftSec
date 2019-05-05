TARGET = server client
DEPS = configs core io programs regex

INCLUDE	:= $(patsubst %,-I%,. $(GINCLUDE))

.PHONY: all $(DEPS)

all: $(TARGET) 

server: server.cpp programs regex core io configs
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -o $(OUTDIR)/$@ -L$(TEMPDIR) $(addprefix -l,$(wordlist 2, $(words $^), $^)) $(LIBS)

client: client.cpp programs regex core io configs
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -o $(OUTDIR)/$@ -L$(TEMPDIR) $(addprefix -l,$(wordlist 2, $(words $^), $^)) $(LIBS)

$(DEPS):
	$(MAKE) --directory=$@ all