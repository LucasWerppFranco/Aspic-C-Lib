CC=gcc
CFLAGS=-Iinclude -Wall
SRC=src/main.c src/game.c src/map.c src/player.c src/enemy.c
OBJ=$(SRC:.c=.o)
BIN=game

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN) -lm

clean:
	rm -f $(BIN) *.o

