
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <paths.h>
#include <dirent.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <fstream>
#include <cstring>
#include "connectComands.h"

bool blacklisted(char user[])
{
    printf("[server] verific blacklist");
    std::ifstream input_file("blacklist.txt");
    std::string curentLine;
    if (input_file.is_open())
    {
        // Keep reading the file
        while (getline(input_file, curentLine))
        {
            char line[MAX_PASSWORD_SIZE + MAX_USERNAME_SIZE];
            strcpy(line, curentLine.c_str());
            if (strstr(line, user) != NULL)
            {
                return 0;
            }
        }
    }
    else
    {
        printf("[server]Unable to open the file!");
    }
    input_file.close();
    return 1;
}


int login(int fd,int &ok)
{
    char user_password[MAX_USERNAME_SIZE + MAX_PASSWORD_SIZE];
    char credential[MAX_USERNAME_SIZE + MAX_PASSWORD_SIZE];
    bzero(user_password,MAX_PASSWORD_SIZE+MAX_USERNAME_SIZE);
    int bytes_read = read(fd, user_password, MAX_PASSWORD_SIZE+MAX_USERNAME_SIZE);
    if (bytes_read < 0)
    {
        perror("Eroare la read() de la client.\n");
        return 0;
    }
    fflush(stdout);
    strcpy(credential, user_password);
    char *p;
    p = strtok(credential, ":");

    if(blacklisted(p)==1){ //verific daca userul este in blacklist

    std::ifstream input_file("login.txt");
    std::string curentLine;
    if (input_file.is_open())
    {
        // Keep reading the file
        while (getline(input_file, curentLine))
        {
            char line[MAX_PASSWORD_SIZE + MAX_USERNAME_SIZE];
            strcpy(line, curentLine.c_str());
            if (strstr(line, user_password) != NULL)
            {
                ok = 1;
            }
        }
    }
    else
    {
        printf("[server]Unable to open the file!");
    }

    if (ok == 1)
    {
        char buffer[MAX_BUF_SIZE] = "succes";
        int bytes_written = write(fd, buffer, MAX_BUF_SIZE);
        if (bytes_written == -1)
        {
            fprintf(stderr, "error at write() in login");
            perror("The cause is");
        }
    }
    else
    {

        char buffer[MAX_BUF_SIZE] = "fail";
        int bytes_written = write(fd, buffer, MAX_BUF_SIZE);
        if (bytes_written == -1)
        {
            fprintf(stderr, "error at write() in login");
            perror("The cause is");
        }
    }
    input_file.close();
    }
    else
    {
        char buffer[MAX_BUF_SIZE] = "fail";
        int bytes_written = write(fd, buffer, MAX_BUF_SIZE);
        if (bytes_written == -1)
        {
            fprintf(stderr, "error at write() in login");
            perror("The cause is");
        }
    }
}


int registerd(int fd)
{
    char user_password[MAX_USERNAME_SIZE + MAX_PASSWORD_SIZE];
    char credential[MAX_USERNAME_SIZE + MAX_PASSWORD_SIZE];
    bzero(user_password,MAX_PASSWORD_SIZE+MAX_USERNAME_SIZE);
    int bytes_read = read(fd, user_password,MAX_PASSWORD_SIZE+MAX_USERNAME_SIZE);
    if (bytes_read < 0)
    {
        perror("Eroare la read() de la client.\n");
        return 0;
    }
    strcpy(credential, user_password);
    std::ifstream input_file("login.txt");
    int ok = 0;
    std::string curentLine;
    if (input_file.is_open())
    {
        // Keep reading the file
        while (getline(input_file, curentLine))
        {
            char line[MAX_PASSWORD_SIZE + MAX_USERNAME_SIZE];
            strcpy(line, curentLine.c_str()); //transform linia curenta din string in char si o pastrez in "line"
            char *p;
            p = strtok(user_password, ":"); //extrag doar user-ul din perechea user-parola
            if (strstr(line, p) != NULL)    //caut sa vad daca exista alt user cu acelasi nume
            {
                ok = 1;
            }
        }
    }
    else
    {
        printf("[server]Unable to open the file!");
    }
    int bytes_written;
    std::ofstream output_file("login.txt", std::ios::app);
    if (ok == 0) //daca nu exista alt user cu acelasi nume,salvez perechea user-parola in fisierul cu conturi
    {
        output_file << credential << std::endl;
        char buffer[MAX_BUF_SIZE] = "succes";
        bytes_written = write(fd, buffer, MAX_BUF_SIZE);
        if (bytes_written == -1)
        {
            fprintf(stderr, "error at write() in register");
            perror("The cause is");
        }
    }
    else
    {
        char buffer[MAX_BUF_SIZE] = "fail";
        bytes_written = write(fd, buffer, MAX_BUF_SIZE);
        if (bytes_written == -1)
        {
            fprintf(stderr, "error at write() in register");
            perror("The cause is");
        }
    }
    input_file.close();
    output_file.close();
}
