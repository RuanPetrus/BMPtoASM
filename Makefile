CFLAGS=-Wall -Wextra -std=c11 -pedantic
LIBS=
SRC=main.c

main: main.c
	$(CC) $(CFLAGS) -o main $(SRC) $(LIBS)

