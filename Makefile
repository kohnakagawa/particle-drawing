#for OpenGL graphics makefile
CC = g++

#CFLAGS = -g -O0 -Wall
CFLAGS = -O3

OBJECTS = sysdraw.o mousehandle.o jpegout.o main.o
TARGET = Draw

LOADLIBES = -lglut -lGLU -lGL -ljpeg

.SUFFIXES:
.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) -c $< $(LOADLIBES) -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) core.* *~