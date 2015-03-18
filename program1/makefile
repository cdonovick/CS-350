CC=clang
CFLAGS=-c -ggdb -std=c99 -Wall -Wextra -pedantic
LDFLAGS=
LIBFLAGS=-lpthread
SOURCES=main.c util.c
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
EXECUTABLE=program1
	
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBFLAGS)
	
$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) $^

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) *.*~

