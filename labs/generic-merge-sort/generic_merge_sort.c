#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE 100

char *array[ARRAY_SIZE];

char* REPORT_FILE;
char* INPUT_FILE;
int isNum;

void report();
int numcmp(char *chr, char *chr2);


int numcmp(char *s1, char *s2)
{
    double v1,
        v2;
    v1 = atof(s1);
    v2 = atof(s2);
    if (v1 < v2){
        return -1;
    }
    else if (v1 > v2){
        return 1;
    }
    else{
        return 0;
    }
}

void dataset(){

    FILE* file = fopen(INPUT_FILE, "r");

    //Se valida que el archivo existe
    if(!file){
        printf("\n ERROR EL ARCHIVO %s, NO EXISTE\n  ", INPUT_FILE);
        return;
    }

    char linea[500];
    int i = 0;
    
    //Ciclo que lee el archivo hasta que no se obtienen lineas
    while (fgets(linea, sizeof(linea), file)) {
        array[i] = malloc(strlen(linea));
        strtok(linea,"\n");
        strcpy(array[i],linea);
        
        i++;
    }

    fclose(file);
}


void report(){
    printf("Generating Report from: [%s]\n", INPUT_FILE);

    FILE* file = fopen(REPORT_FILE,"w");

    for(int i = 0; i < ARRAY_SIZE; i++){
        fprintf(file,"%s\n",array[i]);
    }

    printf("Report is generated at: [%s]\n", REPORT_FILE);
    fclose(file);
}

void merge(char *arr[], int l, int m, int r,int (*comp) (void *, void *)){
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
 
    char* L[n1];
    char* R[n2];

    for (i = 0; i < n1; i++){
        L[i] = arr[l + i];
    }
    for (j = 0; j < n2; j++){
        R[j] = arr[m + 1 + j];
    }
    i = 0;
    j = 0; 
    k = l; 
    while (i < n1 && j < n2) {
        if ((*comp)(L[i],R[j])<=0) { 
            arr[k++] = L[i++];
        }
        else {
            arr[k++] = R[j++];
        }
    }
 
    while (i < n1) {
        arr[k++] = L[i++];
    }
 
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(char *arr[], int l, int r, int (*comp) (void *, void *)){
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m, comp);
        mergeSort(arr, m + 1, r, comp);
        merge(arr, l, m, r,comp);
    }
}

int main(int argc, char *argv[]){

    int size;

    if(argc == 3 && strcmp(argv[1],"-n")==0){
        printf("ENTRADA VALIDA: NUMEROS\n");

        INPUT_FILE = argv[2];
        REPORT_FILE = "sorted_numbers.txt";
        isNum = 1;        
        dataset();
        mergeSort(array, 0, ARRAY_SIZE- 1, (int (*)(void*,void*))numcmp);
        report();

    }else if(argc < 2 || argc > 3){
        printf("ENTRADA INVALIDA\n");

    }else if(argc == 2 && strcmp(argv[1],"-n")==0){
        printf("ENTRADA INVALIDA\n");

    }else{
        printf("ETRADA VALIDA: STRINGS\n");
        INPUT_FILE = argv[1];
        REPORT_FILE = "sorted_strings.txt";
        isNum = 0;
        dataset();
        mergeSort(array, 0, ARRAY_SIZE- 1, (int (*)(void*,void*))strcmp);
        report();
    }

    return 0;
}
