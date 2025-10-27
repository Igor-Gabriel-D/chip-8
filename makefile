CFLAGS=-std=c17 -Wall -Wextra -Werror

all:
	gcc chip-8.c -o chip-8 $(CFLAGS) `sdl2-config --cflags --libs`
