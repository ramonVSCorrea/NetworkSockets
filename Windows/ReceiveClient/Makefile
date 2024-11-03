CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lws2_32

TARGET = ReceiveClient
SRCS = main.c ReceiveClient.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean