ROOT_DIR := $(abspath ..)
BUILD_DIR := $(ROOT_DIR)/build

.PHONY: all configure build run gdb valgrind clean clean-logs

all: build

configure:
	cmake -S "$(ROOT_DIR)" -B "$(BUILD_DIR)"

build: configure
	cmake --build "$(BUILD_DIR)" --parallel

run: build
	"$(BUILD_DIR)/monitoring"

gdb: build
	gdb --args "$(BUILD_DIR)/monitoring"

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes --track-origins=yes --log-file="$(ROOT_DIR)/valgrind.log" "$(BUILD_DIR)/monitoring"

clean:
	cmake --build "$(BUILD_DIR)" --target clean

clean-logs:
	find "$(ROOT_DIR)/logs" -type f -name '*.log' -delete
