#include <stdio.h>
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
#include "FTPcomands.h"

int show_content(int fd)
{
    DIR *dir;
    struct dirent *de;
    struct stat st;
    char cale[MAX_PATH];
    char output[MAX_PATH];
    char buffer[MAX_PATH];
    int bytes_read = read(fd, cale, MAX_PATH);
    if (bytes_read < 0)
    {
        perror("[server]Eroare la read() de la client.\n");
        return 0;
    }

    if (stat(cale, &st) != 0)
    {
        fprintf(stderr, "[server]Eroare la stat pentru %s .\t", cale);
        perror("Cauza este");
    }
    if (NULL == (dir = opendir(cale)))
    {
        fprintf(stderr, "[server]Eroare deschidere director %s .\t", cale);
        perror("Cauza este");
    }

    /* parcurgerea pe orizontală a primului nivel, i.e. descendenții direcți ai argumentului primit de această funcție */
    while (NULL != (de = readdir(dir)))
    {
        if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) /* ignorăm intrările implicite "." și ".." din orice director */
        {
            sprintf(buffer, "%s\n", de->d_name);
            strcat(output, buffer);
        }
    }
    if (write(fd, output, strlen(output)) == -1)
    {
        fprintf(stderr, "[server]Error at write() in show_content");
        perror("The cause is");
    }
    closedir(dir);
}

int show_atributes(int fd)
{
    struct stat st;              /* această structură o vom folosi pentru a afla, cu apelul stat, metadatele asociate unui fișier */
    struct passwd *pwd;          /* această structură o vom folosi pentru a afla username-ul asociat unui UID */
    struct group *grp;           /* această structură o vom folosi pentru a afla groupname-ul asociat unui GID */
    char perm[10] = "---------"; /* aici vom construi forma simbolică pentru permisiunile fișierului */
    int result = 0;
    char cale[MAX_PATH];
    bzero(cale, MAX_PATH);
    int bytes_read = read(fd, cale, MAX_PATH);
    if (bytes_read < 0)
    {
        perror("[server]Eroare la read() de la client.\n");
        return 0;
    }
    if (0 != stat(cale, &st))
    {
        fprintf(stderr, "Eroare la stat pentru %s .\t", cale);
        perror("Cauza este");
        return 2; /* aici nu terminăm execuția cu exit(), ci revenim în apelant pentru a continua cu ceea ce a mai rămas de procesat */
    }
    char atributes[MAX_PATH];
    char buffer[MAX_PATH];
    sprintf(atributes, "Analizam fisierul/directorul: %s\n", cale);
    strcat(buffer, atributes);
    sprintf(atributes, "\tTipul acestui fisier: ");
    switch (st.st_mode & S_IFMT)
    {
    case S_IFDIR:
        sprintf(atributes, "Director\n");
        result = 1;
        break;
    case S_IFREG:
        sprintf(atributes, "Fisier obisnuit\n");
        break;
    case S_IFLNK:
        sprintf(atributes, "Link\n");
        break;
    case S_IFIFO:
        sprintf(atributes, "FIFO\n");
        break;
    case S_IFSOCK:
        sprintf(atributes, "Socket\n");
        break;
    case S_IFBLK:
        sprintf(atributes, "Block device\n");
        break;
    case S_IFCHR:
        sprintf(atributes, "Character device\n");
        break;
    default:
        sprintf(atributes, "Unknown file type");
    }

    strcat(buffer, atributes);
    sprintf(atributes, "\tNumarul de nume alternative ale fisierului: %ld octeti\n", st.st_nlink);
    strcat(buffer, atributes);
    sprintf(atributes, "\tDimensiunea acestuia: %lld octeti\n", (long long)st.st_size);
    strcat(buffer, atributes);
    sprintf(atributes, "\tSpatiul ocupat de disc pentru stocarea acestuia: %lld sectoare (blocuri de 512 octeti)\n", (long long)st.st_blocks);
    strcat(buffer, atributes);

    sprintf(atributes, "\tPermisiunile acestuia, in notatie octala: %o\n", st.st_mode & 0777);
    strcat(buffer, atributes);
    if (S_IRUSR & st.st_mode)
        perm[0] = 'r';
    if (S_IWUSR & st.st_mode)
        perm[1] = 'w';
    if (S_IXUSR & st.st_mode)
        perm[2] = 'x';
    if (S_IRGRP & st.st_mode)
        perm[3] = 'r';
    if (S_IWGRP & st.st_mode)
        perm[4] = 'w';
    if (S_IXGRP & st.st_mode)
        perm[5] = 'x';
    if (S_IROTH & st.st_mode)
        perm[6] = 'r';
    if (S_IWOTH & st.st_mode)
        perm[7] = 'w';
    if (S_IXOTH & st.st_mode)
        perm[8] = 'x';

    sprintf(atributes, "\tPermisiunile in notatie simbolica: %s\n", perm);
    strcat(buffer, atributes);
    if (NULL != (pwd = getpwuid(st.st_uid)))
        sprintf(atributes, "\tProprietarul acestuia: %s (cu UID-ul: %ld)\n", pwd->pw_name, (long)st.st_uid);
    else
        sprintf(atributes, "\tProprietarul acestuia are UID-ul: %ld\n", (long)st.st_uid);
    strcat(buffer, atributes);
    if (NULL != (grp = getgrgid(st.st_gid)))
        sprintf(atributes, "\tGrupul proprietar al acestuia: %s (cu GID-ul: %ld)\n", grp->gr_name, (long)st.st_gid);
    else
        sprintf(atributes, "\tGrupul proprietar al acestuia are GID-ul: %ld\n", (long)st.st_gid);
    strcat(buffer, atributes);
    if (write(fd, buffer, strlen(buffer)) == -1)
    {
        fprintf(stderr, "[server]Error at write() in show_atributes");
        perror("The cause is");
    }
    return result;
}

int find_file(char *file_to_find, char *cale, char *returnare)
{

    DIR *dir;
    struct dirent *de;
    struct stat st;
    char nume[MAX_PATH];
    if (stat(cale, &st) != 0)
    {
        fprintf(stderr, "Eroare la stat pentru %s .\t", cale);
        perror("Cauza este");
    }

    if ((st.st_mode & S_IFMT) != S_IFDIR) //verific daca calea nu este director
    {
        if (strstr(cale, file_to_find))
        {
            printf(" am gasit fisierul %s ", file_to_find);
            printf(" in calea %s\n", cale);
            strcpy(returnare, cale);
        }
    }
    else
    {
        if (NULL == (dir = opendir(cale)))
        {
            fprintf(stderr, "Eroare deschidere director %s .\t", cale);
            perror("Cauza este");
        }

        /* parcurgerea pe orizontală a primului nivel, i.e. descendenții direcți ai argumentului primit de această funcție */
        while (NULL != (de = readdir(dir)))
        {
            if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) /* ignorăm intrările implicite "." și ".." din orice director */
            {
                sprintf(nume, "%s/%s", cale, de->d_name); /* construim calea nouă, ca și concatenare de string-uri */
                find_file(file_to_find, nume, returnare);
            }
        }
        closedir(dir);
    }
}

int transfer_fisier(int fd)
{
    char buffer[MAX_FILE_SIZE];
    char file_to_find[MAX_PATH];
    bzero(file_to_find, MAX_PATH);
    int bytes_read = read(fd, file_to_find, MAX_PATH);
    if (bytes_read < 0)
    {
        perror("Eroare la read() de la client.\n");
        return 0;
    }
    char *cale;
    char cale1[MAX_PATH];
    cale = new char[MAX_PATH];
    strcpy(cale, "/home");
    find_file(file_to_find, cale, cale1);
    if (strstr(cale1, "Eroare"))
    {
        if (write(fd, cale1, strlen(cale1)) == -1)
        {
            fprintf(stderr, "[server]Error at write() in transfer_file");
            perror("The cause is");
        }
    }
    else
    {
        printf("[server] Calea fisierului este: %s",cale1);
        std::ifstream read_file(cale1);
        std::string curentLine;
        if (read_file.is_open())
        {
            while (getline(read_file, curentLine))
            {
                strcat(buffer, curentLine.c_str());
                strcat(buffer, "\n");
            }
        }
        else
        {
            printf("[server]Unable to open the file!");
        }
        if (write(fd, buffer, strlen(buffer)) == -1)
        {
            fprintf(stderr, "[server]Error at write() in transfer_file");
            perror("The cause is");
        }
        read_file.close();
    }
}
