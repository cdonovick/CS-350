CC=gcc
CFLAGS=-c -std=c99 -Wall -Wextra -pedantic -ggdb 
LDFLAGS=
LIBFLAGS=-lpthread
SOURCES=main.c
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
EXECUTABLE=lab3

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBFLAGS)
	
$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) $^ 
clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) *.*~
