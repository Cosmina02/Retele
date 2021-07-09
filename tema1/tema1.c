#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <paths.h>
#include "commands.h"


    int main(){
      printf(" Puteti introduce urmatoarele comenzi:\n");
      printf(" login:\n myfind \n mystat \n quit \n");
       run_command();
        
       return 0;
    }