CXX = icpc
#CXX = g++

#CFLAGS = -g -O0 -Wall -std=c++11
CFLAGS = -std=c++11 -O3 -xHOST -no-prec-div

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