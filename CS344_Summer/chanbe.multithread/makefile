CC = gcc
CFLAGS = -g -Wall -pthread

all: line_processor main

line_processor: line_processor.c
	$(CC) $(CFLAGS) -o line_processor line_processor.c

main: main.rs
	rustc main.rs

clean:
	rm -f line_processor
