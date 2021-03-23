#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int mystrlen(char *str){
    int lenght = 0;
    
    for (int i = 0; str[i] != 0; i++) {
        lenght++;
    }

    return lenght;
}

char *mystradd(char *origin, char *addition){
    int origen = mystrlen(origin);
    int addLen = mystrlen(addition);
    char *nuevoStr = malloc(sizeof(char)*origen+ addLen);
    
    strcpy(nuevoStr, origin);
    strcpy(nuevoStr + origen, addition);
    return nuevoStr;
}

int mystrfind(char *origin, char *substr){

  int aux = 0;

  int origen = mystrlen(origin);
  int subLen = mystrlen(substr);
  for (int i = 0; i <= origen; i++) {
    if (origin[i] == substr[0] && aux < 1) {
      aux++;
    }
    else if (origin[i] == substr[aux]) {
      aux++;
    }
    else {
      aux = 0;
    }
    if (aux >= subLen) {
      return aux;
    }
  }
  return 0;
}