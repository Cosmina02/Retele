#include <stdio.h>
#define WRITE 1
#define READ 0
#define MAX_PASSWORD_SIZE 35
#define MAX_USERNAME_SIZE 35
#define MAX_BUF_SIZE 8
#define MAX_FISIER 10
#define MAX_PATH 4096
int run_command(void);
int login_process(void);
int child_login(int fd_login[]);
int parent_login(char user[],char parola[],int fd_login[]);
int myfind_process(void);
int parcurgere_directoare(char* cale,char* file_to_find);
int parent_myfind(int fd_myfind[]);
int child_myfind(int fd_myfind[]);
int mystat_process(char path[MAX_PATH]);
int mystat_parent(char path[MAX_PATH],int fd1,int fd2);
void quit(void);
int file_info(char text[MAX_PATH],int fd);
