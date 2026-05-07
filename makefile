CXX = g++
CXX_FLAGS = -std=c++20 -Wall
SDL_FLAGS = $(shell sdl2-config --cflags --libs)
LIBS = -lSDL2_ttf 
# -lSDL2_image 

TARGET = compile
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(SDL_FLAGS) $(LIBS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJ)

# g++ -std=c++20 main.cpp $(sdl2-config --cflags --libs) -lSDL2_ttf -o compile