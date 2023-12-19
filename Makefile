TARGET = server
CC = gcc
CFLAGS = -Wall -g
SRC = server.c parser/parser.c handler/command.c struct/hashmap.c struct/expiration.c cJSON/cJSON.c utils/utils.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ) *.o
