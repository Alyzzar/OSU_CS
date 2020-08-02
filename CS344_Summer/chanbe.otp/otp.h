#ifndef OTP_H
#define OTP_H

#define MAX_CONNECTIONS 5

const char alph[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

void intToStr (int input[], char output[], int len);
void strToInt (char input[], int output[], int len);
void enc (char, char, char, int);
void dec (char, char, char, int);

void validateText(char, int, char);
int validateLen(char, char);

//OTP_c args
//argv[1]	argv[2]		argv[3] 	'e'||'d'
//text		key 		port #		option
int otp_c (char*, char*, char*, char);

//OTP_c args
//argv[1]		'e'||'d'
//port #		option
int otp_d(char*, char);

#endif
