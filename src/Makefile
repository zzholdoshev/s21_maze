CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c11 -D _DEFAULT_SOURCE -g -fprofile-arcs -ftest-coverage
nc=-lncurses
check=-lcheck

SRCDIR = .
BUILDDIR=../build
TARGET_NAME = maze
TARGET = $(BUILDDIR)/$(TARGET_NAME)
SRCS := main.c
OBJS := $(patsubst %.c, ../build/%.o, $(SRCS))
LIB_SRCS := a1_maze.c graph_funcs.c q_learning.c
LIB_OBJS := $(patsubst %.c, ../build/%.o, $(LIB_SRCS))
TESTS_SRCS := $(wildcard tests/*.c)
TESTS_OBJS := $(patsubst %.c, ../build/%.o, $(TESTS_SRCS))
TESTS_BIN := ../build/tests/a1_maze_tests

GCOV_TOOL = gcov
GCOV_REPORT_DIR = html_report
GCOV_REPORT_CMD = gcovr --gcov-executable "$(GCOV_TOOL)" \
									--decisions \
									--exclude tests/ \
									--exclude-noncode-lines \
									--exclude-throw-branches \
									--exclude-unreachable-branches \
									--html-theme github.dark-blue \
									--html-details $(GCOV_REPORT_DIR)/index.html \
									../build/

ifeq ($(OS),Windows_NT)
	TESTS_BIN := $(TESTS_BIN).exe
else
	UNAME_S = $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
	
	else
		check +=-lm -lsubunit
	endif
endif

.PHONY: all clean

all: clean setup $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS) $(LIB_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(nc)

install: clean setup $(TARGET)
	mv $(TARGET) ~/
	rm -rf $(BUILDDIR) $(TESTS_BIN) $(GCOV_REPORT_DIR)

uninstall:
	rm ~/$(TARGET_NAME)

../build/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

setup:
	mkdir -p ../build
	mkdir -p ../build/tests

$(TESTS_BIN): setup $(TESTS_OBJS) $(LIB_OBJS)
	$(CC) $(CFLAGS) $(TESTS_OBJS) $(LIB_OBJS) -o $(TESTS_BIN) $(check) $(nc)

.PHONY: tests
tests: clean $(TESTS_BIN)
	./$(TESTS_BIN)

.PHONY: gcov_report
gcov_report: clean $(TESTS_BIN)
			@mkdir -p $(GCOV_REPORT_DIR)
			-./$(TESTS_BIN)
			$(GCOV_REPORT_CMD)
			open ./$(GCOV_REPORT_DIR)/index.html

clean:
	rm -rf $(BUILDDIR) $(TESTS_BIN) $(GCOV_REPORT_DIR)
style:
	clang-format -style=google -i *.c *.h
