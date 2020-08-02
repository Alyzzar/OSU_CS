CC = gcc
CFLAGS = -g -Wall -ansi -pedantic-errors

all: keygen dec_client dec_server enc_client enc_server

coder.o: coder.c otp.h
	$(CC) $(CFLAGS) -c coder.c
otp.o: otp.c otp.h
	$(CC) $(CFLAGS) -c otp.c

keygen:	keygen.c
	$(CC) $(CFLAGS) -o keygen keygen.c

dec_client: dec_client.c coder.o otp.o
	$(CC) $(CFLAGS) -o dec_client dec_client.c coder.o otp.o
dec_server: dec_server.c coder.o otp.o
	$(CC) $(CFLAGS) -o dec_server dec_server.c coder.o otp.o

enc_client: enc_client.c coder.o otp.o
	$(CC) $(CFLAGS) -o enc_client enc_client.c coder.o otp.o
enc_server: enc_server.c coder.o otp.o
	$(CC) $(CFLAGS) -o enc_server enc_server.c coder.o otp.o

clean:
	rm -f *.o
	rm -f dec_client dec_server enc_client enc_server keygen
