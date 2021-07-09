#include <stdio.h>
#define WRITE 1
#define READ 0
#define MAX_PASSWORD_SIZE 35
#define MAX_USERNAME_SIZE 35
#define MAX_BUF_SIZE 300
#define MAX_FISIER 10
#define MAX_PATH 4096

int login(int fd,int &ok);
int registerd(int fd);
bool blacklisted(char user[]);
