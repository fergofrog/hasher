CFLAGS = -Wall
CFLAGS += -O2
CFLAGS += -g -pg
LDFLAGS = 
LDFLAGS += -g -pg
SOURCES = main.c global.c file.c md5/md5.c sha1/sha1.c sha2/sha2.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = hasher

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

clean:
	-rm -f $(OBJECTS) $(EXECUTABLE) core

$(OBJECTS): global.h file.h
main.o: md5/md5.h sha1/sha1.h sha2/sha2.h
