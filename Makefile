CXX = g++

#CFLAGS = -g -O0 -Wall -std=c++11
CFLAGS = -O3 -std=c++11

OBJECTS = sysdraw.o mousehandle.o jpegout.o main.o
TARGET = cDraw
LOADLIBES = -lglut -lGLU -L/usr/lib/nvidia-331-updates -lGL -ljpeg -lGLEW

.SUFFIXES:
.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) $(LOADLIBES) -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) core.* *~