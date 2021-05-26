//Based on (https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64)

#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "logger.h"

#define OUTPUT_CODIFICADO "-encoded.txt"
#define OUTPUT_DECODIFICADO "-decoded.txt"

int progress;
long inputSize;

//mapa base con los valores de base64
char base46_map[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                     'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                     'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

char* codificar(char* plain);
char* decodificar(char* cifra);
char*(*generateOutput)(void *);


//Funcion encargada de manejar las seniales de usuario
void manSenial(int sig){
	if(sig == SIGINT){
		warnf("Se detuvo el proceso con un progreso de %.2f%%\n",(progress * 100.0) / inputSize);
		exit(7);
	} else if(sig == SIGUSR1){
		infof("El progreso actual es de %.2f%%\n",(progress * 100.0) / inputSize);
	}
}


//metodo encargado de hace codificar, basado en base64Encode
char* codificar(char* plain) {

    char contador = 0;
    char buffer[3];
    char* cifra = malloc(strlen(plain) * 4 / 3 + 4);
    int i = 0, c = 0;

    for(i = 0; plain[i] != '\0'; i++) {
        buffer[contador++] = plain[i];
        if(contador == 3) {
            cifra[c++] = base46_map[buffer[0] >> 2];
            cifra[c++] = base46_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
            cifra[c++] = base46_map[((buffer[1] & 0x0f) << 2) + (buffer[2] >> 6)];
            cifra[c++] = base46_map[buffer[2] & 0x3f];
            contador = 0;
        }
    }

    if(contador > 0) {
        cifra[c++] = base46_map[buffer[0] >> 2];
        if(contador == 1) {
            cifra[c++] = base46_map[(buffer[0] & 0x03) << 4];
            cifra[c++] = '=';
        } else {                      
            cifra[c++] = base46_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
            cifra[c++] = base46_map[(buffer[1] & 0x0f) << 2];
        }
        cifra[c++] = '=';
    }

	//caracter para padding de strings 
    cifra[c] = '\0';   
    return cifra;
}


//Metodo encargado en decodificar basado en Base64Decode
char* decodificar(char* cifra) {

    char contador = 0;
    char buffer[4];
    char* plain = malloc(strlen(cifra) * 3 / 4);
    int i = 0, p = 0;

    for(i = 0; cifra[i] != '\0'; i++) {
        char k;
        for(k = 0 ; k < 64 && base46_map[k] != cifra[i]; k++);
        buffer[contador++] = k;
        if(contador == 4) {
            plain[p++] = (buffer[0] << 2) + (buffer[1] >> 4);
            if(buffer[2] != 64)
                plain[p++] = (buffer[1] << 4) + (buffer[2] >> 2);
            if(buffer[3] != 64)
                plain[p++] = (buffer[2] << 6) + buffer[3];
            contador = 0;
        }
    }

    plain[p] = '\0';  
    return plain;
}

int main(int argc, char **argv){

	progress = 0;
	inputSize = 1;

	if(signal(SIGUSR1,manSenial) == SIG_ERR){
		warnf("Could not catch SIGUSR1 signal\n");
	}

	if(signal(SIGINT,manSenial) == SIG_ERR){
		warnf("Could not catch SIGINT signal\n");
	}

	//se indica que manejador usar para la senial
	signal(SIGINT, manSenial);
    signal(SIGUSR1, manSenial);

	//Se valida una entrada correcta
	if(argc < 3){
		errorf("Error. Usa el formato: ./base64 <--encode/--decode> <file>\n");
		return 1;
	} else if(strcmp(argv[1],"--encode") * strcmp(argv[1],"--decode") != 0) {
		errorf("Error. Comando no aceptado.\n");
		return 2;
	}

	//se asigna el nombre del archivo y se verifica que exista 
	char *filename = argv[2];
	int entrada = open(filename, O_RDONLY);
	if(entrada < 0){
		errorf("Error. El archivo no esxiste\n");
		return 3;
	}

	char *outputF;
	char filename2 [50];

	//se quita el tipo de archivo del nombre y se agrega a una string vac[ia ]
	strncat(filename2,filename,strlen(filename)-4);

	//se define la funcion a usar
	if(strcmp(argv[1],"--encode") == 0){
		outputF = strcat(filename2,OUTPUT_CODIFICADO);
		generateOutput = (char*(*)(void *))(&codificar);
	} else{
		outputF = strcat(filename2,OUTPUT_DECODIFICADO);;
		generateOutput = (char*(*)(void *))(&decodificar);
	}

	//se abre el archivo destino
	int salida = open(outputF, O_WRONLY|O_TRUNC|O_CREAT,0664);
	if(salida < 0){
		errorf("Error. Imposible abrir el archivo de salida\n");
		return 4;
	}

	inputSize = lseek(entrada,0,SEEK_END);

	if(inputSize < 0){
		errorf("Error. El archico excede la entrada soportada\n");
		return 8;
	}

	if(lseek(entrada, -inputSize, SEEK_END) < 0){
		errorf("Error. Imposible llegar al inicio del archivo\n");
		return 9;
	}

	int progressBytes = -1;
	char inputBuffer[129];
	char *outputText;

	//se lee el archivo, se codifica y se escribe en el destino
	while((progressBytes = read(entrada, inputBuffer, 128)) > 0){
		inputBuffer[progressBytes] = '\0';
		outputText = generateOutput(inputBuffer);

		if(write(salida,outputText, strlen(outputText)) < 0){
			errorf("Error. Could not properly write into file\n");
			return 11;
		}

		progress += progressBytes;

	}

	//Validacion de resultados
	if(progressBytes < 0){
		errorf("Error. Could not properly read file\n");
		return 10;
	}

	//Se cierran los archivos
	if(close(entrada) < 0){
		errorf("Error. Imposible cerrar el archivo de entrada\n");
		return 5;
	}

	if(close(salida) < 0){
		errorf("Error. Imposible cerrar el archivo de salida\n");
		return 6;
	}

	infof("Se ha generado correctamente el archivo %s\n", outputF);

    	return 0;
}