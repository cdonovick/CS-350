CC=gcc
CFLAGS=-c -std=c99 -Wall -Wextra -pedantic
LDFLAGS=-s
SOURCES=main.c mytimer.c
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))
EXECUTABLE=lab1
	
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $^ -o $@
	
$(OBJECTS): $(SOURCES)
	$(CC) $(CFLAGS) $^

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) *.*~

