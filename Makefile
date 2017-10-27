CXX = g++

CFLAGS = -g -O3 -Wall -Wextra -std=c++11 -ffast-math -funroll-loops -pthread

OBJECTS = sysdraw.o mousehandle.o jpegout.o main.o
TARGET = cDraw
LOADLIBES = -lpthread -lglut -lGLU -lGL -ljpeg -lGLEW -L/usr/lib/i386-linux-gnu -lboost_filesystem -lboost_system

.SUFFIXES:
.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) core.* *~
