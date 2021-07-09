#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include "connectComands.h"
#include "FTPcomands.h"
#define MAX_MESSAGE 4096
#define PORT 2024 //protul folosit

extern int errno; //cod de eroare
int main()
{
    struct sockaddr_in server; // struct pt server
    struct sockaddr_in from;
    char msg[MAX_MESSAGE];
    char msgrasp[MAX_MESSAGE];
    int sd;
    int logged_in = 0;
    //creare socket

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server] Eroare la socket().\n");
        return errno;
    }

    //pregatirea struct de date
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server] Eroare la bind(),\n");
        return errno;
    }

    if (listen(sd, 5) == -1)
    {
        perror("[server]Eroare la listen().\n");
        return errno;
    }

    while (1)
    {
        int client;
        int length = sizeof(from);
        printf("[server]Asteptam la portul %d...\n", PORT);
        fflush(stdout);

        client = accept(sd, (struct sockaddr *)&from, (socklen_t *)&length);
        if (client < 0)
        {
            perror("[server] Eroare la accept().\n");
            continue;
        }
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("[server] Eroare la fork()");
            exit(1);
        }
        else if (pid == 0)
        { //proces copil

            while (1)
            {
                bzero(msg, MAX_MESSAGE);
                printf("[server] Asteptam mesajul...\n");
                fflush(stdout);
                if (read(client, msg, MAX_MESSAGE) <= 0)
                {
                    perror("[server]Eroare la read() de la client.\n");
                    close(client);
                    continue;
                }
                printf("[server]Mesajul a fost receptionat...%s\n", msg);

                bzero(msgrasp, 100);

                if (strstr(msg, "help") && logged_in == 0)
                {
                    bzero(msg, MAX_MESSAGE);
                    strcpy(msgrasp, "Comenzile disponibile sunt:\n");
                    strcat(msgrasp, "> register\n");
                    strcat(msgrasp, "> login\n");
                    strcat(msgrasp, "> help\n");
                    strcat(msgrasp, "> quit\n");
                    if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                    {
                        perror("[server]Eroare la write() catre client.\n");
                        return errno;
                        continue;
                    }
                }
                else
                {
                    if (strstr(msg, "help") && logged_in == 1)
                    {
                        bzero(msg, MAX_MESSAGE);
                        strcpy(msgrasp, "Comenzile disponibile sunt:\n");
                        strcat(msgrasp, "> show_content\n");
                        strcat(msgrasp, "> show_atributes\n");
                        strcat(msgrasp, "> transfer_file\n");
                        strcat(msgrasp, "> help\n");
                        strcat(msgrasp, "> logout\n");
                        strcat(msgrasp, "> quit");
                        if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                        {
                            perror("[server]Eroare la write() catre client.\n");
                            return errno;
                            continue;
                        }
                    }
                    else
                    {

                        if (strstr(msg, "quit"))
                        {
                            bzero(msg, MAX_MESSAGE);
                            strcpy(msgrasp, "Quit!");
                            if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                            {
                                perror("[server]Eroare la write() catre client.\n");
                                return errno;
                            }
                            close(client);
                            break;
                        }
                        else
                        {
                            if (strstr(msg, "register") && logged_in == 0)
                            {
                                bzero(msg, MAX_MESSAGE);
                                strcpy(msgrasp, "Introduceti userul dorit si apoi parola.");
                                if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                                {
                                    perror("[server]Eroare la write() catre client.\n");
                                    return errno;
                                    continue;
                                }
                                else
                                    registerd(client);
                            }
                            else
                            {

                                if (strstr(msg, "login") && logged_in == 0)
                                {
                                    bzero(msgrasp, MAX_MESSAGE);
                                    strcpy(msgrasp, "Introduceti userul si parola.");
                                    if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                                    {
                                        perror("[server]Eroare la write() catre client.\n");
                                        return errno;
                                        continue;
                                    }
                                    else
                                    {
                                        int ok = 0;
                                        login(client, ok);
                                        if (ok == 1)
                                        {
                                            logged_in = 1;
                                        }
                                        printf("[server]logged_in= %d\n", logged_in);
                                    }
                                }
                                else
                                {
                                    if (logged_in == 0)
                                    {
                                        if (strstr(msg, "show_content"))
                                        {
                                            bzero(msgrasp, MAX_MESSAGE);
                                            strcpy(msgrasp, "Puteti executa aceasta comanda dupa ce va logati.");
                                            if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                                            {
                                                perror("[server]Eroare la write() catre client.\n");
                                                return errno;
                                                continue;
                                            }
                                        }
                                        else
                                        {
                                            if (strstr(msg, "show_atributes"))
                                            {
                                                bzero(msgrasp, MAX_MESSAGE);
                                                strcpy(msgrasp, "Puteti executa aceasta comanda dupa ce va logati.");
                                                if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                                                {
                                                    perror("[server]Eroare la write() catre client.\n");
                                                    return errno;
                                                    continue;
                                                }
                                            }
                                            else
                                            {
                                                if (strstr(msg, "transfer_file"))
                                                {
                                                    bzero(msgrasp, MAX_MESSAGE);
                                                    strcpy(msgrasp, "Puteti executa aceasta comanda dupa ce va logati.");
                                                    if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                                                    {
                                                        perror("[server]Eroare la write() catre client.\n");
                                                        return errno;
                                                        continue;
                                                    }
                                                }
                                                else
                                                {
                                                    bzero(msgrasp, MAX_MESSAGE);
                                                    strcpy(msgrasp, "!!!Comanda nerecunoscuta!!!");
                                                    strcat(msgrasp, "Comenzile disponibile sunt:\n");
                                                    strcat(msgrasp, "> register\n");
                                                    strcat(msgrasp, "> login\n");
                                                    strcat(msgrasp, "> help\n");
                                                    strcat(msgrasp, "> quit\n");
                                                    if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                                                    {
                                                        perror("[server]Eroare la write() catre client.\n");
                                                        return errno;
                                                        continue;
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    if (logged_in == 1)
                                    {
                                        if (strstr(msg, "logout"))
                                        {
                                            logged_in = 0;
                                            bzero(msgrasp, MAX_MESSAGE);
                                            strcpy(msgrasp, ">logged out\n");
                                            strcat(msgrasp, "Acum va puteti conecta cu un alt cont sau va puteti crea un nou cont.");
                                            if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                                            {
                                                perror("[server]Eroare la write() catre client.\n");
                                                return errno;
                                                continue;
                                            }
                                        }
                                        else
                                        {

                                            if (strstr(msg, "show_content"))
                                            {
                                                bzero(msgrasp, MAX_MESSAGE);
                                                strcpy(msgrasp, "Introduceti calea directorului al carui continut doriti sa-l vedeti.");
                                                if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                                                {
                                                    perror("[server]Eroare la write() catre client.\n");
                                                    return errno;
                                                    continue;
                                                }
                                                else
                                                    show_content(client);
                                            }
                                            else
                                            {
                                                if (strstr(msg, "show_atributes"))
                                                {
                                                    bzero(msgrasp, MAX_MESSAGE);
                                                    strcpy(msgrasp, "Introduceti calea fisierului,sau doar numele(in cazul in care este in directorul vizionat anterior).");
                                                    if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                                                    {
                                                        perror("[server]Eroare la write() catre client.\n");
                                                        return errno;
                                                        continue;
                                                    }
                                                    else
                                                        show_atributes(client);
                                                }
                                                else
                                                {

                                                    if (strstr(msg, "transfer_file"))
                                                    {
                                                        bzero(msgrasp, MAX_MESSAGE);
                                                        strcpy(msgrasp, "Introduceti numele fisierului pe care doriti sa il transferati.");
                                                        int buffer = write(client, msgrasp, strlen(msgrasp));
                                                        printf("buffer: %d\n", buffer);
                                                        if (buffer <= 0)
                                                        {
                                                            perror("[server]Eroare la write() catre client.\n");
                                                            return errno;
                                                            continue;
                                                        }
                                                        else
                                                        {

                                                            transfer_fisier(client);
                                                            printf("[server] sunt in else\n");
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (strstr(msg, "login"))
                                                        {
                                                            bzero(msgrasp, MAX_MESSAGE);
                                                            strcpy(msgrasp, "Sunteti deja logat,puteti schimba contul doar dupa ce introduceti comanda 'logout' .");
                                                            if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                                                            {
                                                                perror("[server]Eroare la write() catre client.\n");
                                                                return errno;
                                                                continue;
                                                            }
                                                        }
                                                        else
                                                        {
                                                            if (strstr(msg, "register"))
                                                            {
                                                                bzero(msgrasp, MAX_MESSAGE);
                                                                strcpy(msgrasp, "Pentru a crea un nou cont trebuie sa introduceti mai intai comanda 'logout' .");
                                                                if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                                                                {
                                                                    perror("[server]Eroare la write() catre client.\n");
                                                                    return errno;
                                                                    continue;
                                                                }
                                                            }
                                                            else
                                                            {
                                                                bzero(msgrasp, MAX_MESSAGE);
                                                                strcpy(msgrasp, "!!!Comanda nerecunoscuta!!!");
                                                                strcat(msgrasp, "Comenzile disponibile sunt:\n");
                                                                strcat(msgrasp, "> show_content\n");
                                                                strcat(msgrasp, "> show_atributes\n");
                                                                strcat(msgrasp, "> transfer_file\n");
                                                                strcat(msgrasp, "> help\n");
                                                                strcat(msgrasp, "> logout\n");
                                                                strcat(msgrasp, "> quit");
                                                                if (write(client, msgrasp, strlen(msgrasp)) <= 0)
                                                                {
                                                                    perror("[server]Eroare la write() catre client.\n");
                                                                    return errno;
                                                                    continue;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
