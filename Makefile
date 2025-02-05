CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS = -lm
TARGET = ex2
SRC = golfile.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)