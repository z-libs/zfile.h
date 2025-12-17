
BUNDLER = z-core/zbundler.py
SRC = src/zfile.c
DIST = zfile.h

CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c11 -O2 -I.
CXXFLAGS = -Wall -Wextra -std=c++11 -O2 -I.

all: get_dependencies bundle

bundle:
	@echo "Bundling $(DIST)..."
	python3 $(BUNDLER) $(SRC) $(DIST)

get_dependencies:
	@echo "Using wget to add dependencies..."
	wget -q "https://raw.githubusercontent.com/z-libs/zstr.h/main/zstr.h" -O "zstr.h"
	
clean:
	@echo "Removing dependencies..."
	@rm zstr.h

test: get_dependencies bundle test_c test_cpp clean

test_c:
	@echo "----------------------------------------"
	@echo "Building C Tests..."
	@$(CC) $(CFLAGS) tests/test_main.c -o tests/runner_c
	@./tests/runner_c
	@rm tests/runner_c

test_cpp:
	@echo "----------------------------------------"
	@echo "Building C++ Tests..."
	@$(CXX) $(CXXFLAGS) tests/test_cpp.cpp -o tests/runner_cpp
	@./tests/runner_cpp
	@rm tests/runner_cpp

init:
	git submodule update --init --recursive

.PHONY: all get_dependencies bundle init test test_c test_cpp clean



