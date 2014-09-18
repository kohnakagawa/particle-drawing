#for OpenGL graphics makefile
CC = g++

#CFLAGS = -g -O0 -Wall
CFLAGS = -O3

OBJECTS = sysdraw.o mousehandle.o jpegout.o main.o
TARGET = cDraw

LOADLIBES = -lglut -lGLU -L/usr/lib/nvidia-304 -lGL -ljpeg -lGLEW

.SUFFIXES:
.SUFFIXES: .cpp .o
.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) core.* *~