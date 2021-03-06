/**		Assignment 3: One-Time Pads  
  *		Written by Ben Chan for OSU CS 344, Summer 2020  
  *		
  *		This is the header for OTP. Function prototypes are listed here.
  **/
  
#ifndef OTP_H
#define OTP_H

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_CONNECTIONS 5

extern const char alph[27];

void intToStr (int[], char[], int);
void strToInt (char[], int[], int);
void enc (char[], char[], char[], int);
void dec (char[], char[], char[], int);

void error(const char*, int);
void validateText(char[], int);
int validateLen(char[], char[]);
void parseFile(char*, char (*) []);

int otp_c (char*, char*, char*, char*);
int otp_s(char*, char*);

#endif
