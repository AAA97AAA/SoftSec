SRCDIR   = src
BINDIR   = bin
INCLUDES = include

CC=g++
CFLAGS=-Wall -Wextra -g -fno-stack-protector -z execstack -lpthread -std=gnu11 -I $(INCLUDES)/
DEPS = authman.o barrier.o fileman.o messagebox.o path.o process.o procman.o  resman.o runenv.o runsys.o channel.o isocketbuf.o osocketbuf.o socket.o loadconfig.o  grass.o parse.o AlternationExpression.o AnyCharacterExpression.o LazyRepeatRangeExpression.o LiteralExpression.o RegularExpression.o RepeatNExpression.o RepeatRangeExpression.o SearchExpression.o date.o get.o grep.o ls.o mkrm.o ping.o put.o shell.o who.o parse.o grass.o shell.o

DEPSINBIN = $(BINDIR)/authman.o $(BINDIR)/barrier.o $(BINDIR)/fileman.o $(BINDIR)/messagebox.o $(BINDIR)/path.o $(BINDIR)/process.o $(BINDIR)/procman.o  $(BINDIR)/resman.o $(BINDIR)/runenv.o $(BINDIR)/runsys.o $(BINDIR)/channel.o $(BINDIR)/isocketbuf.o $(BINDIR)/osocketbuf.o $(BINDIR)/socket.o $(BINDIR)/loadconfig.o   $(BINDIR)/AlternationExpression.o $(BINDIR)/AnyCharacterExpression.o $(BINDIR)/LazyRepeatRangeExpression.o $(BINDIR)/LiteralExpression.o $(BINDIR)/RegularExpression.o $(BINDIR)/RepeatNExpression.o $(BINDIR)/RepeatRangeExpression.o $(BINDIR)/SearchExpression.o $(BINDIR)/date.o $(BINDIR)/get.o $(BINDIR)/grep.o $(BINDIR)/ls.o $(BINDIR)/mkrm.o $(BINDIR)/ping.o $(BINDIR)/put.o $(BINDIR)/shell.o $(BINDIR)/who.o $(BINDIR)/parse.o $(BINDIR)/grass.o

all: $(BINDIR)/client $(BINDIR)/server cleanObj

$(BINDIR)/client: $(SRCDIR)/client.cpp  $(DEPS)
	$(CC) -I src  $(CFLAGS) $< -o $@ $(DEPSINBIN)

$(BINDIR)/server: $(SRCDIR)/server.cpp  $(DEPS)
	$(CC) -I src  $(CFLAGS) $< -o $@ $(DEPSINBIN)

%.o: $(SRCDIR)/%.cpp
	$(CC)  -I src $(CFLAGS) -o $(BINDIR)/$@ -c $<

%.o: $(SRCDIR)/*/%.cpp $(SRCDIR)/core/%.h
	$(CC)  -I src $(CFLAGS) -o $(BINDIR)/$@ -c $<

%.o: $(SRCDIR)/*/%.cpp
	$(CC)  -I src $(CFLAGS) -o $(BINDIR)/$@ -c $<

cleanObj:
	rm -f $(BINDIR)/*.o


.PHONY: clean
clean:
	rm -f $(BINDIR)/client $(BINDIR)/server
