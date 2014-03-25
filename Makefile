OBJECTS = darkness.o
HEADERS =
CC = gcc
OUTPUT = 
DEBUG = -Wall
LINKING_OPTIONS = 
CFLAGS = $(shell allegro-config --cflags)
LDFLAGS = $(shell allegro-config --libs)
WIN_CC = wine c:\\MinGW\\bin\\gcc.exe
WIN_OUTPUT = darkness.exe

darkness: $(OBJECTS)
	 $(CC) $(DEBUG) $(LINKING_OPTIONS) -o darkness $(OBJECTS) $(LDFLAGS)

darkness.o: darkness.c $(HEADERS)
	 $(CC) $(DEBUG) -c darkness.c $(CFLAGS)

clean:
	 rm -f $(OBJECTS)

win:
	 $(WIN_CC) $(DEBUG) *.c -o $(WIN_OUTPUT) -lalleg

