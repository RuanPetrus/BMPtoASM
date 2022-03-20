CFLAGS=-std=c11
# CFLAGS=-Wall -Wextra -std=c11 -pedantic
LIBS=
SRC=main.c

BMPtoASM: main.c
	$(CC) $(CFLAGS) -o BMPtoASM $(SRC) $(LIBS)

