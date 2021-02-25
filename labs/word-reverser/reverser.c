#include <stdio.h>

#define LIMITE 1000


void reverse(char linea[]){
  int i, j;
  char aux;

  for (i = 0; linea[i] != '\0'; ++i)
    ;
  --i;
  
  if (linea[i] == '\n')
    --i;
  
  j = 0;
  
  while (j < i) {
    aux = linea[j];
    linea[j] = linea[i];
    linea[i] = aux;
    --i;
    ++j;
  }
}

int getLineas(char str[], int limite){
  int i, c;

  for (i = 0; i < limite-1 && (c = getchar()) != EOF && c != '\n'; ++i){
    str[i] = c;
  }

  if (c == '\n'){
    str[i] = c;
    ++i;
  }

  str[i] = '\0';
  return i;
}

int main(int argc, char **argv){
  int tam;
  char linea[LIMITE];

  while ((tam = getLineas(linea, LIMITE)) > 0){
    reverse(linea);
    printf("%s\n", linea);
  }

  return 0;
}