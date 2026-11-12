CXX = g++
CXXFLAGS = -I allegro/include -I src -Wall -Wextra
# 	-g3 -Og

LDFLAGS = -L allegro/lib

LIBS = -lallegro -lallegro_image -lallegro_primitives -lallegro_audio -lallegro_acodec -lallegro_font -lallegro_ttf

TARGET = src/main.exe

all:
	$(CXX) src/main.cpp -o $(TARGET) $(CXXFLAGS) $(LDFLAGS) $(LIBS)

debug:
	$(CXX) src/main.cpp -o $(TARGET) $(CXXFLAGS) $(LDFLAGS) $(LIBS) -g3 -Og

run:
	$(TARGET)

run-debug:
	gdb -tui $(TARGET)
