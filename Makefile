CXX = g++
CXXFLAGS = -I allegro/include -I src -Wall -Wextra \
# 	-g3 -Og

LDFLAGS = -L allegro/lib

LIBS = -lallegro -lallegro_image -lallegro_primitives -lallegro_audio -lallegro_acodec

TARGET = src/main.exe

all:
	$(CXX) src/main.cpp -o $(TARGET) $(CXXFLAGS) $(LDFLAGS) $(LIBS)

run:
	$(TARGET)

debug:
	gdb -tui $(TARGET)
