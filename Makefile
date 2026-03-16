CXX=g++
CXXFLAGS=-o3 -Wall -Wextra

TARGET=run
SRC=src/example.cpp src/headers/Agent.h src/headers/SectionPool.h src/headers/System.h

all:$(TARGET)

$(TARGET):$(SRC) $(HEADERS) 
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $TARGET