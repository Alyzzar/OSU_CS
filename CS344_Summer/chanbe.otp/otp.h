#ifndef OTP_H
#define OTP_H

#define MAX_CONNECTIONS 5

const char alph[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

void enc(char, char, char, int);
void dec(char, char, char, int);

int otp_c(char*, char*, char*, char);

int otp_d(char*, char);

#endif
