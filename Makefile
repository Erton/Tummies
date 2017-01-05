all:	tummies

CFLAGS=-W -Wall -Wextra -g -march=native

tummies:
	clang ${CFLAGS} -lSDL -lSDL_mixer -lpng -lm -o tummies main.c
clean:
	rm map.png tummies

