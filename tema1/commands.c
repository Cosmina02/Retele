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
#include "commands.h"
int login = 0;

int child_login(int fd_login[])
{
    char user_passwd[MAX_USERNAME_SIZE + MAX_PASSWORD_SIZE];
    FILE *input_file = fopen("login.txt", "r");
    if (input_file == NULL)
    {
        fprintf(stderr, "error at openc1()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    int bytes_read = read(fd_login[READ], user_passwd, MAX_USERNAME_SIZE + MAX_PASSWORD_SIZE);
    if (bytes_read == -1)
    {
        fprintf(stderr, "error at openc2()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    user_passwd[bytes_read] = '\0';
    const size_t line_size = 300;
    char *line = malloc(line_size);
    int verif = 0;
    if (close(fd_login[READ]) == -1)
    {
        fprintf(stderr, "error at closec1()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    while (fgets(line, line_size, input_file) != NULL)
    {
        if (strstr(line, user_passwd) != NULL)
        {
            verif = 1;
            char buffer[MAX_BUF_SIZE] = "succes";
            int bytes_written;
            bytes_written = write(fd_login[WRITE], buffer, MAX_BUF_SIZE);
            if (bytes_written == -1)
            {
                fprintf(stderr, "error at writec1()");
                perror("The cause is");
                exit(EXIT_FAILURE);
            }
            break;
        }
    }
    free(line);
    if (fclose(input_file) == -1)
    {
        fprintf(stderr, "error at closec3()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    if (verif == 0)
    {
        char buffer[MAX_BUF_SIZE] = "fail";
        printf("buffer in child %s", buffer);
        int bytes_written = write(fd_login[WRITE], buffer, MAX_BUF_SIZE);
        if (bytes_written == -1)
        {
            fprintf(stderr, "error at writec2()");
            perror("The cause is");
            exit(EXIT_FAILURE);
        }
    }
    if (close(fd_login[WRITE]) == -1)
    {
        fprintf(stderr, "error at closec4()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

int parent_login(char *user, char *parola, int *fd_login)
{
    strcat(user, ":");
    strcat(user, parola);
    int bytes_written = write(fd_login[WRITE], user, strlen(user));
    if (bytes_written == -1)
    {
        fprintf(stderr, "error at writep1()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    if (close(fd_login[WRITE]) == -1)
    {
        fprintf(stderr, "error at closep2()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    int status;
    wait(&status);
    char buffer[MAX_BUF_SIZE];
    int bytes_read = read(fd_login[READ], buffer, MAX_BUF_SIZE);
    if (bytes_read == 0)
    {
        fprintf(stderr, "error at readp1()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    buffer[bytes_read] = '\0';
    printf("%s\n", buffer);
    if (strcmp("succes", buffer) == 0)
        login = 1;
    if (close(fd_login[READ]) == -1)
    {
        fprintf(stderr, "error at closep3()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
}

int login_process(void)
{
    int fd_login[2], bytes_read;
    char user[MAX_USERNAME_SIZE];
    char parola[MAX_PASSWORD_SIZE];
    printf("introduceti userul ");
    scanf("%s", user);
    user[bytes_read] = '\0';
    printf("introduceti parola userului ");
    scanf("%s", parola);
    parola[bytes_read] = '\0';
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd_login) == -1)
    {
        fprintf(stderr, "error at socketpair()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    pid_t pid_login = fork();
    if (pid_login == -1)
    {
        fprintf(stderr, "error at fork()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    if (pid_login != 0)
    {
        //parent
        parent_login(user, parola, fd_login);
        run_command();
    }
    else
    {
        //child
        child_login(fd_login);
    }
    return 0;
}

int file_info(char cale[MAX_PATH], int fd)
{
    struct stat st;              /* această structură o vom folosi pentru a afla, cu apelul stat, metadatele asociate unui fișier */
    struct passwd *pwd;          /* această structură o vom folosi pentru a afla username-ul asociat unui UID */
    struct group *grp;           /* această structură o vom folosi pentru a afla groupname-ul asociat unui GID */
    char perm[10] = "---------"; /* aici vom construi forma simbolică pentru permisiunile fișierului */
    int result = 0;

    if (0 != stat(cale, &st))
    {
        fprintf(stderr, "Eroare la stat pentru %s .\t", cale);
        perror("Cauza este");
        return 2; /* aici nu terminăm execuția cu exit(), ci revenim în apelant pentru a continua cu ceea ce a mai rămas de procesat */
    }
    char atributes[MAX_PATH];
    char buffer[MAX_PATH];
    sprintf(atributes, "Analizam fisierul/directorul: %s\n", cale);
    strcat(buffer,atributes);
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

    strcat(buffer,atributes);
    sprintf(atributes, "\tNumarul de nume alternative ale fisierului: %ld octeti\n", st.st_nlink);
    strcat(buffer,atributes);
    sprintf(atributes, "\tDimensiunea acestuia: %lld octeti\n", (long long)st.st_size);
    strcat(buffer,atributes);
    sprintf(atributes, "\tSpatiul ocupat de disc pentru stocarea acestuia: %lld sectoare (blocuri de 512 octeti)\n", (long long)st.st_blocks);
    strcat(buffer,atributes);

    sprintf(atributes, "\tPermisiunile acestuia, in notatie octala: %o\n", st.st_mode & 0777);
    strcat(buffer,atributes);
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
   strcat(buffer,atributes);
    if (NULL != (pwd = getpwuid(st.st_uid)))
        sprintf(atributes, "\tProprietarul acestuia: %s (cu UID-ul: %ld)\n", pwd->pw_name, (long)st.st_uid);
    else
        sprintf(atributes, "\tProprietarul acestuia are UID-ul: %ld\n", (long)st.st_uid);
    strcat(buffer,atributes);
    if (NULL != (grp = getgrgid(st.st_gid)))
        sprintf(atributes, "\tGrupul proprietar al acestuia: %s (cu GID-ul: %ld)\n", grp->gr_name, (long)st.st_gid);
    else
        sprintf(atributes, "\tGrupul proprietar al acestuia are GID-ul: %ld\n", (long)st.st_gid);
    strcat(buffer,atributes);
    write(fd, buffer, strlen(buffer));
    return result;
}

int mystat_parent(char path[MAX_PATH], int fd1, int fd2)
{

    fd1 = open("mystat1", O_WRONLY); //deschid capatul de scriere al parintelui
    if (fd1 == -1)
    {
        fprintf(stderr, "error at opening fifo1()");
        perror("The cause is ");
    }
    scanf("Calea este %s", path);
    if (write(fd1, path, MAX_PATH) == -1)
    {
        fprintf(stderr, "error at writing in fifo1()");
        perror("The cause is ");
    }
    close(fd1);                      //inchid capatul de scriere al parintelui
    fd2 = open("mystat2", O_RDONLY); //deschid capatul de citire al parintelui
    if (fd2 == -1)
    {
        fprintf(stderr, "error at opening fifo2()");
        perror("The cause is ");
    }
    char atributes[MAX_PATH];
    int bytes_read = read(fd2, atributes, MAX_PATH);
    if (bytes_read == -1)
    {
        fprintf(stderr, "error at reading in fifo2()");
        perror("The cause is ");
    }
    if (bytes_read > 0)
    {
        printf("%s\n", atributes);
        fflush(stdout);
    }
    atributes[0] = '\0';
    close(fd2); //inchid capatul de citire al parintelui
}

int mystat_process(char path[MAX_PATH])
{

    mkfifo("mystat1", 0777); //parent to child
    mkfifo("mystat2", 0777); //child to parent
    int fd1, fd2, fd3, fd4;
    pid_t pid_stat = fork();
    if (pid_stat != 0)
    {
        //parent
        mystat_parent(path, fd1, fd2);
        run_command();
    }
    else
    {
        //child
        char text[MAX_PATH];
        fd3 = open("mystat1", O_RDONLY); //deschid capatul de citire al copilului
        if (fd3 == -1)
        {
            fprintf(stderr, "error at opening fifo3()");
            perror("The cause is ");
        }
        if (read(fd3, text, MAX_PATH) == -1)
        {
            fprintf(stderr, "error at reading in fifo3()");
            perror("The cause is ");
        }
        close(fd3);                      //inchid capatul de citire al copilului
        fd4 = open("mystat2", O_WRONLY); //deschid capatul de scriere al copilului
        if (fd4 == -1)
        {
            fprintf(stderr, "error at opening fifo4()");
            perror("The cause is ");
        }
        file_info(text, fd4);
        close(fd4);
    }
}

int parcurgere_directoare(char *cale, char *file_to_find)
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
        if (strstr(cale, file_to_find) != 0)
        {
            printf("am gasit fisierul %s ", file_to_find);
            printf(" in calea %s\n", cale);
            mystat_process(cale);
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
                parcurgere_directoare(nume, file_to_find);
            }
        }
        closedir(dir);
    }
}

int child_myfind(int fd_myfind[])
{

    char fisier[MAX_FISIER];
    int bytes_read = read(fd_myfind[READ], fisier, MAX_FISIER);
    if (bytes_read == -1)
    {
        fprintf(stderr, "error at writep1()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    char cale[6] = "/home";
    parcurgere_directoare(cale, fisier);
    if (close(fd_myfind[READ]) == -1)
    {
        fprintf(stderr, "error at closep2()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
}

int parent_myfind(int fd_myfind[])
{

    printf("introduceti fisierul pe care il cautati\n");
    char fisier[MAX_FISIER];
    scanf("%s", fisier);
    int bytes_written = write(fd_myfind[WRITE], fisier, MAX_FISIER);
    if (bytes_written == -1)
    {
        fprintf(stderr, "error at writep1()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    int status;
    wait(&status);
    if (close(fd_myfind[WRITE]) == -1)
    {
        fprintf(stderr, "error at closep2()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
}

int myfind_process(void)
{

    int fd_myfind[2];
    if (pipe(fd_myfind) == -1)
    {
        fprintf(stderr, "error at pipe1()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    pid_t pid_myfind = fork();
    if (pid_myfind == -1)
    {
        fprintf(stderr, "error at fork()");
        perror("The cause is");
        exit(EXIT_FAILURE);
    }
    if (pid_myfind != 0)
    {
        //parent
        parent_myfind(fd_myfind);
        run_command();
    }
    else
    {
        //child
        child_myfind(fd_myfind);
    }
}

void quit(void)
{
    exit(10);
}

int run_command(void)
{
    char command[100];
    printf("introduceti comanda\n");
    scanf("%s", command);
    
    if (strcmp(command, "quit") == 0)
    {
        quit();
    }

    if (login == 0)
    {
        if (strcmp(command, "login:") == 0)
        {
            command[0] = '\0';
            login_process();
        }
        else
        {
            if (strcmp(command, "myfind") == 0 || strcmp(command, "mystat") == 0)
            {
                printf("Va rog sa va logati mai intai\n");
                command[0] = '\0';
                run_command();
            }
            else
            {
                printf("Comanda invalida\n");
                command[0] = '\0';
                run_command();
            }
        }
    }
    else
    {
        if (strcmp(command, "login:") == 0)
        {
            printf("Sunteti deja logat,incercati alta comanda\n");
            command[0] = '\0';
            run_command();
        }
        else
        {
            if (strcmp(command, "myfind") == 0)
            {
                command[0] = '\0';
                myfind_process();
            }
            if (strcmp(command, "mystat") == 0)
            {
                command[0] = '\0';
                char path[MAX_PATH];
                printf("Dati calea fisierului ");
                scanf("%s", path);
                mystat_process(path);
            }
        }
    }
    return 0;
}
