PREFIX = /usr/local
CC = g++
CCFILES = $(wildcard src/*.cpp)
EXECUTABLE = fatch
CFLAGS = -O2

ifdef CLANG
	CC = clang
endif
ifdef DEBUG
	CFLAGS += -DDEBUG -O0 -g
endif
ifndef NO_PCI
	LDFLAGS = -lpci
else
	CFLAGS += -DNO_PCI
endif

.PHONY: build build_nopci install uninstall clean run debug debug_nopci

build:
	$(CC) $(CCFILES) -o $(EXECUTABLE) $(CFLAGS) $(LDFLAGS)  

install: ./$(EXECUTABLE)
	cp ./$(EXECUTABLE) $(PREFIX)/bin/

uninstall: $(PREFIX)/bin/$(EXECUTABLE)
	rm $(PREFIX)/bin/$(EXECUTABLE)

clean:
	rm ./$(EXECUTABLE) 2> /dev/null || true
	rm ./$(EXECUTABLE)_debug 2> /dev/null || true
	rm ./vgcore* 2> /dev/null || true

run: build
ifdef DEBUGGER
	$(DEBUGGER) ./$(EXECUTABLE) $(RUNARGS)
else
	./$(EXECUTABLE) $(RUNARGS)
endif

