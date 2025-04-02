CC = gcc
CFLAGS = -Wall -Wextra -pthread

TARGET = cigarettesmokers
SRC = cigarettesmokers.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean