CC=gcc
CFLAGS=-c -std=c99 -Wall -Wextra -pedantic -ggdb
LDFLAGS=
SOURCES=main.c
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
EXECUTABLE=lab2
	
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $^ -o $@
	
$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) $^

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) *.*~

