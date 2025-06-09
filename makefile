CXX = g++
CXXFLAGS = -std=c++11 -Wall `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`
SRC = src/main.cpp src/game.cpp src/board.cpp src/ai.cpp src/gui.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = warcaby

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)