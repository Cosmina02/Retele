#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <fstream>
#include <cstring>
#define MAX_MESSAGE 4096
#define MAX_FILE_SIZE 7340032 //7mb
extern int errno;

int port;

int main(int argc, char *argv[])
{
    int sd;
    struct sockaddr_in server;
    char input[MAX_MESSAGE], msg[MAX_MESSAGE], cale[500], nume[500];
    char *parola, user[255];
    char file[MAX_FILE_SIZE];
    if (argc != 3)
    {
        printf("[client]Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    port = atoi(argv[2]);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[client]Eroare la socket.\n");
        return errno;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);

    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[client] Eroare la connect().\n");
        return errno;
    }
    while (1)
    {
        fflush(stdout);
        bzero(input, 100);
        printf("[client] Introduceti o comanda: ");
        fflush(stdout);
        scanf("%s", input);
        fflush(stdout);
        if (write(sd, input, strlen(input)) <= 0)
        {
            perror("[client]Eroare la write() spre server.\n");
            return errno;
        }
        bzero(msg, MAX_MESSAGE);

        if (read(sd, msg, MAX_MESSAGE) < 0)
        {
            perror("[client]Eroare la read() de la server.\n");
            return errno;
        }
        printf("%s \n", msg);
        fflush(stdout);
        if (strstr(msg, "Introduceti userul dorit si apoi parola.")) //register
        {
            bzero(msg, MAX_MESSAGE);
            bzero(input, MAX_MESSAGE);
            printf("User: ");
            scanf("%s", user);
            fflush(stdout);
            strcpy(input, user);
            strcat(input, ":");
            parola = getpass("Password: ");
            strcat(input, parola);
            fflush(stdout);

            if (write(sd, input, strlen(input)) <= 0)
            {
                perror("[client]Eroare la read() de la server.\n");
                return errno;
            }
            bzero(input, MAX_MESSAGE);

            bzero(msg, MAX_MESSAGE);
            if (read(sd, msg, MAX_MESSAGE) < 0)
            {
                perror("[client]Eroare la read() de la server.\n");
                return errno;
            }
            if(strstr(msg,"succes")){
                printf("Acum ca sunteti inregistrat va puteti autentifica.\n Folositi comanda login.\n");
            }
            if(strstr(msg,"fail")){
                printf("A avut loc o eroare,va rugam sa va inregistrati din nou.\n");
            }
            fflush(stdout);
            bzero(msg, MAX_MESSAGE);
        }
        else
        {
            if (strstr(msg, "Quit!")) //quit
            {
                bzero(msg, MAX_MESSAGE);
                close(sd);
                break;
            }

            if (strstr(msg, "Introduceti userul si parola.")) //login
            {
                bzero(msg, MAX_MESSAGE);
                bzero(input, MAX_MESSAGE);
                printf("User: ");
                scanf("%s", user);
                fflush(stdout);
                strcpy(input, user);
                strcat(input, ":");
                parola = getpass("Password: ");
                strcat(input, parola);
                fflush(stdout);
                if (write(sd, input, strlen(input)) <= 0)
                {
                    perror("[client]Eroare la read() de la server.\n");
                    return errno;
                }
                bzero(input, MAX_MESSAGE);
                fflush(stdout);
                bzero(msg, MAX_MESSAGE);
                if (read(sd, msg, MAX_MESSAGE) < 0)
                {
                    perror("[client]Eroare la read() de la server.\n");
                    return errno;
                }
                if(strstr(msg,"succes")){
                printf("Acum ca v-ati autentificat puteti accesa operatiile cu directoare.\n");
            }
            if(strstr(msg,"fail")){
                printf("A avut loc o eroare,va rugam sa va autentificati din nou.\n");
            }
                fflush(stdout);
                bzero(msg, MAX_MESSAGE);
            }

            if (strstr(msg, "Introduceti calea directorului al carui continut doriti sa-l vedeti.")) //show_content
            {
                bzero(msg, MAX_MESSAGE);
                bzero(input, MAX_MESSAGE);
                printf("Calea: ");
                scanf("%s", cale);

                fflush(stdout);
                strcpy(input, cale);
                if (write(sd, input, strlen(input)) <= 0)
                {
                    perror("[client]Eroare la read() de la server.\n");
                    return errno;
                }
                bzero(input, MAX_MESSAGE);
                fflush(stdout);
                bzero(msg, MAX_MESSAGE);
                if (read(sd, msg, MAX_MESSAGE) < 0)
                {
                    perror("[client]Eroare la read() de la server.\n");
                    return errno;
                }
                printf("Acesta este continutul directorului ales:\n");
                printf("%s\n", msg);
                fflush(stdout);
                bzero(msg, MAX_MESSAGE);
            }
            if (strstr(msg, "Introduceti calea fisierului,sau doar numele(in cazul in care este in directorul vizionat anterior).")) //show_atributes
            {
                bzero(msg, MAX_MESSAGE);
                bzero(input, MAX_MESSAGE);
                bzero(nume, 500);
                printf("Introduceti datele astfel: cale(sau nume)_calea(sau numele)(exp:  cale_/home/cosmina/Desktop/proiect sau nume_proiect) :\n");
                scanf("%s", nume);
                fflush(stdout);
                if (strstr(nume, "cale"))
                {
                    char word[]="cale_";
                    int n=0,i,j=0,k=0,flag=0;
                    for (i = 0; nume[i] != '\0'; i++)
                    {
                        k = i;

                        while (nume[i] == word[j])
                        {
                            i++, j++;
                            if (j == strlen(word))
                            {
                                flag = 1;
                                break;
                            }
                        }
                        j = 0;

                        if (flag == 0)
                            i = k;
                        else
                            flag = 0;

                        input[n++] = nume[i];
                    }

                    input[n] = '\0';
                }
                if (strstr(nume, "nume"))
                {
                   char word[]="nume_",word2[300];
                    int n=0,i,j=0,k=0,flag=0;
                    for (i = 0; nume[i] != '\0'; i++)
                    {
                        k = i;

                        while (nume[i] == word[j])
                        {
                            i++, j++;
                            if (j == strlen(word))
                            {
                                flag = 1;
                                break;
                            }
                        }
                        j = 0;

                        if (flag == 0)
                            i = k;
                        else
                            flag = 0;

                        word2[n++] = nume[i];
                    }

                    input[n] = '\0';
                    strcpy(input, cale);
                    strcat(input, "/");
                    strcat(input, word2);
                    
                }
                if (write(sd, input, strlen(input)) <= 0)
                {
                    perror("[client]Eroare la read() de la server.\n");
                    return errno;
                }
                bzero(input, MAX_MESSAGE);
                fflush(stdout);
                bzero(msg, MAX_MESSAGE);
                if (read(sd, msg, MAX_MESSAGE) < 0)
                {
                    perror("[client]Eroare la read() de la server.\n");
                    return errno;
                }
                printf("%s\n", msg);
                fflush(stdout);
                bzero(msg, MAX_MESSAGE);
            }

            if(strstr(msg,"Introduceti numele fisierului pe care doriti sa il transferati.")){ //transfer file

                bzero(msg, MAX_MESSAGE);
                bzero(input, MAX_MESSAGE);
                printf("Introduceti numele fisierului: ");
                scanf("%s",input);
                if (write(sd, input, strlen(input)) <= 0)
                {
                    perror("[client]Eroare la read() de la server.\n");
                    return errno;
                }
                bzero(input, MAX_MESSAGE);
                fflush(stdout);
                //modif sa poata alege daca salveaza cu numele inputului sau nu. mi e lene acuma aya e
                bzero(file, MAX_FILE_SIZE);
                if (read(sd, file, MAX_FILE_SIZE) < 0)
                {
                    perror("[client]Eroare la read() de la server.\n");
                    return errno;
                }
                fflush(stdout);
                char fisier[500];
                printf("Introduceti numele fisierului in care doriti sa stocati informatia primita(trebuie sa fie un fisier .txt \n");
                scanf("%s",fisier);
                FILE *fp;
                fp  = fopen (fisier, "w");
                fputs(file,fp);
                fclose(fp);
                bzero(file, MAX_FILE_SIZE);
            }
        }

        bzero(msg, MAX_MESSAGE);
        fflush(stdout);
    }
}