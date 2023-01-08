CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g $(LIBS)
INCLUDE = -Iinclude

SRC = $(wildcard src/*.c) $(wildcard src/**/*.c)
OBJ = $(SRC:.c=.o)
BUILD = build

intercept: clean
	intercept-build --append make build

all: clear dirs clean run

dirs:
	mkdir -p $(BUILD) src tests

run: build
	$(BUILD)/build

build: $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(BUILD)/$@ $?

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

clean:
	rm -Rf $(OBJ)

clear:
	clear
