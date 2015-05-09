SHELL = /bin/sh
CC    = gcc
CFLAGS       = -fPIC -g -I . -I sapi_lib/
LDFLAGS = -pthread -lsysapi

TARGET  = CLIBackend
SOURCES = $(shell echo *.c)
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.c=.o)

PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin

all: $(TARGET)

$(TARGET): $(OBJECTS)
	    $(CC) $(CFLAGS) $(DEBUGFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
clean:
	rm -rf *.o ${TARGET}
