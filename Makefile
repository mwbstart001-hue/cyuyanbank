CXX = g++
CXXFLAGS = -Wall -Iinclude -std=c++17
LDFLAGS = -lsqlite3

TARGET = bank_system
SRCS = src/main.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf data/*.db

.PHONY: all clean
