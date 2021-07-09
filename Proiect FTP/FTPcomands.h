#include <stdio.h>
#define MAX_PATH 4096
#define MAX_FILE_SIZE 7340032 //7mb
int show_content(int fd);
int show_atributes(int fd);
int find_file(char* file_to_find,char *cale,char* returnare);
int transfer_fisier(int fd);