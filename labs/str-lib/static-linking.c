#include <stdio.h>
#include <string.h>

int mystrlen(char *str);
char *mystradd(char *origin, char *addition);
int mystrfind(char *origin, char *substr);

int main(int argc, char **argv) {

    if (argc < 4 || argc > 4) {
        printf("ERROR! Se necesitan tres argumentos, bajo el siguiente formato: \n");
        printf("./main <funcion> <string1> <string2>\n");

    }else{

            if (strcmp(argv[1],"-add")==0) {

                int lenght = mystrlen(argv[2]);
                char *strNew = mystradd(argv[2], argv[3]);
                int newlenght = mystrlen(strNew);
                
                printf("Initial Lenght\t   : %d\nNew String\t   : %s\nNew Lenght\t   : %d\n", lenght, strNew,newlenght);

            }else if(strcmp(argv[1],"-find")==0) {

                int pos = mystrfind(argv[2], argv[3]);

                printf("['%s'] string was found at [%d] position\n",argv[3],pos);

            }else{
                printf("Funcion no valida, usar '-add' o '-find'\n");
            }
    }

  return 0;
}