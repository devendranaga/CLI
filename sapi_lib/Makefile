SHELL = /bin/sh
CC    = gcc
CFLAGS       = -Wall -fPIC -g
LDFLAGS = -shared

TARGET  = libsysapi.so
SOURCES = $(shell echo *.c)
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.c=.o)

PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin

all: $(TARGET)

$(TARGET): $(OBJECTS)
	    $(CC) $(CFLAGS) $(LDFLAGS) $(DEBUGFLAGS) -o $(TARGET) $(OBJECTS)
clean:
	rm -rf *.o ${TARGET}
