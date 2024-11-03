CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lws2_32

TARGET = SendClient
SRCS = main.c SendClient.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean