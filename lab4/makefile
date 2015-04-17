CC=gcc
CFLAGS=-c -O3 -std=gnu99 -Wall -Wextra -pedantic
LDFLAGS=-s
LIBFLAGS=-lpthread
SOURCES=main.c mytimer.c
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
EXECUTABLE=lab4
	
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBFLAGS)
	
$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) $^

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) *.*~

