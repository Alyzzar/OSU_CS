#ifndef OTP_H
#define OTP_H

const char alph[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

void enc(char, char, char, int);
void dec(char, char, char, int);

//OTP_c args
//argv[1]	argv[2]		argv[3] 	'e'||'d'
//text		key 		port #		option
int otp_c(char*, char*, char*, char);

//OTP_c args
//argv[1]		'e'||'d'
//port #		option
int otp_d(char*, char);

#endif