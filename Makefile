CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g

TARGET = chgmac

$(TARGET): main.c
	$(CC) $(CFLAGS) $^ -o $@
