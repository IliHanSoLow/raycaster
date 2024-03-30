CFLAGS = -Wall -Wextra

all:
	zig cc -g -o build/main.out src/main.c -lGL -lglfw -lGLEW -lGLU

clean:
	rm -rf ./build/*
	rm -rf ./build/.*

run: all
	./build/main.out
