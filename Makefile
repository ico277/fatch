PREFIX = /usr/local
CC = gcc
CFILES = src/fatch.c
EXECUTABLE = fatch
CFLAGS = -O2
LDFLAGS = -lpci

build:
	$(CC) $(CFILES) -o $(EXECUTABLE) $(LDFLAGS) $(CFLAGS)

build_nopci:
	$(CC) -DNO_PCI $(CFILES) -o $(EXECUTABLE) $(CFLAGS)

install: ./$(EXECUTABLE)
	cp ./$(EXECUTABLE) $(PREFIX)/bin/

uninstall: $(PREFIX)/bin/$(EXECUTABLE)
	rm $(PREFIX)/bin/$(EXECUTABLE)

clean:
	rm ./$(EXECUTABLE) 2> /dev/null || true
	rm ./$(EXECUTABLE)_debug 2> /dev/null || true
	rm ./vgcore* 2> /dev/null || true

run: build
	./$(EXECUTABLE)

debug: clean
	$(CC) $(CFILES) -o $(EXECUTABLE)_debug $(LDFLAGS) $(CFLAGS)
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXECUTABLE)_debug

debug_nopci: clean
	$(CC) -DNO_PCI $(CFILES) -o $(EXECUTABLE)_debug $(CFLAGS)
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXECUTABLE)_debug
	