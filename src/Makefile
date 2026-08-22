CXX      = g++
CXXFLAGS  = -g -O0 -Wall -fPIC -std=c++20
GTK_CFLAGS = $(shell pkg-config --cflags gtkmm-4.0)
GTK_LIBS   = $(shell pkg-config --libs gtkmm-4.0)
LDFLAGS = $(shell pkg-config --libs gtkmm-4.0)
AGENT_PATH = ../agents
DELETE = $(shell find . -maxdepth 3 -type f -regex ".*\.o\|.*\.so$$" 2>/dev/null)

BUILD = Core/core.cpp Gui/gui.cpp Controller/controller.cpp main.cpp 
BUILD_O = $(BUILD:.cpp=.o)
LIB = $(shell find include -type f -name "*.cpp" 2>/dev/null)
LIB_O = $(LIB:.cpp=.o)

all: run

# Цель может быть любой библиотекой вида lib%.so
lib%.so: $(AGENT_PATH)/%.o
	$(CXX) $(LDFLAGS) -shared -o $(AGENT_PATH)/$@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(GTK_CFLAGS) -c $< -o $@

libinclude.a: $(LIB_O)
	ar rcs $@ $^

build: $(BUILD_O) libinclude.a
	$(CXX) $(LDFLAGS) $(BUILD_O) -o main -L. -linclude $(GTK_LIBS)
	chmod +x main

gdb: build
	gdb main
run: build
	./main
valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --track-origins=yes --log-file=valgrind.log ./main

clean:
	rm -f $(DELETE) main libinclude.a

clean-logs:
	rm -f $(shell find ../logs -type f -name "*.log")
clean-agents:
	rm -f $(shell find ../agents -type f -name "*.so")
