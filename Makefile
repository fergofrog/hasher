CFLAGS = -Wall
#CFLAGS += -O2
CFLAGS += -g -pg
CFLAGS += -pthread
LDFLAGS = 
LDFLAGS += -g -pg
LDFLAGS += -pthread
SOURCES = main.c global.c file.c hash.c md5/md5.c sha1/sha1.c sha2/sha2.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = hasher

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

clean:
	-rm -f $(OBJECTS) $(EXECUTABLE) core gmon.out

$(OBJECTS): global.h file.h
main.o: hash.h md5/md5.h sha1/sha1.h sha2/sha2.h
