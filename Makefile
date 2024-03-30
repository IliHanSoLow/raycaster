CFLAGS = -Wall -Wextra

all:
	zig cc -g -o gl0 gl0.c -lGL -lglfw -lGLEW
