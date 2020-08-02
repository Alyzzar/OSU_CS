CC = gcc
CFLAGS = -g -Wall -pedantic-errors

all: keygen dec_client dec_server enc_client enc_server

coder.o: coder.c otp.h
	echo "Compiling coder.o."
	$(CC) $(CFLAGS) -c coder.c
otp.o: otp.c otp.h
	echo "Compiling otp.o."
	$(CC) $(CFLAGS) -c otp.c

keygen:	keygen.c
	echo "Compiling keygen."
	$(CC) $(CFLAGS) -o keygen keygen.c

dec_client: dec_client.c coder.o otp.o
	echo "Compiling dec_client."
	$(CC) $(CFLAGS) -o dec_client dec_client.c coder.o otp.o
dec_server: dec_server.c coder.o otp.o
	echo "Compiling dec_server."
	$(CC) $(CFLAGS) -o dec_server dec_server.c coder.o otp.o

enc_client: enc_client.c coder.o otp.o
	echo "Compiling enc_client."
	$(CC) $(CFLAGS) -o enc_client enc_client.c coder.o otp.o
enc_server: enc_server.c coder.o otp.o
	echo "Compiling enc_server."
	$(CC) $(CFLAGS) -o enc_server enc_server.c coder.o otp.o

clean:
	echo "Removing *.o and executable files."
	rm -f *.o
	rm -f dec_client dec_server enc_client enc_server keygen
