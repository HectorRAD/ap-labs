#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>  
#include <sys/stat.h>


//#define REPORT_FILE "popo.txt"
#define ARRAY_SIZE 4000

//Contador global, que sirve como indice de paquetes siuuuu
int INDEX = 0;

//Contadores para paquetes
int instalados = 0;
int actualizados = 0;
int borrados = 0;
int alpm = 0;
int pacman = 0;
int alpmScrip = 0;
int packActualizados = 0;

//Variable global de la direccion del reporte a generar 
char* REPORT_FILE;

//variable para los paquetes no actualizados
char noActual[100000];

//Struct para manejar los distintos paquetes
struct Paquete {
    char nombre[30];
    char installed[20];
    char updated[20];
    char removed[20];
    int contador;
};


//Array de los paquetes 

struct Paquete arr[ARRAY_SIZE];

void generateReport();
void instalacion(char linea[]);
void borrar(char linea[]);
void actualizar(char linea[]);

//Funcion para definir que tipo de accion se realiza sobre el paquete
void tipoPaquete(char linea[]){
    //variables para cad tipo de accion
    char *tipoInstal;
    char *tipoBorrar;
    char *tipoActual;
    char* tipo;
    
    //se busca una instancia del identifiador de cada accion en la linea
    tipoInstal = strstr(linea, "[ALPM] installed");
    tipoBorrar = strstr(linea, "[ALPM] removed");
    tipoActual = strstr(linea, "[ALPM] upgraded");

    //Si se encuentra una accion de las buscadas, se procede a actualizar la informacion
    //ademas se llama a la funcion encargada de registrar cada tipo de cambio
    if(tipoActual != NULL){
        actualizados++;
        actualizar(linea);
    } 
    else if(tipoBorrar != NULL){
        borrados++;
        borrar(linea);
    }else if(tipoInstal != NULL){
        instalados++;
        instalacion(linea);
    }

}

//Contador de la seccion de estadisticas, similar a la funcion tipoPaquete
void count(char linea[]){
    char *alpmC;
    char *alpmScripC;
    char *pacmanC;

    //se busca una instancia de cada tipo de stat pedida
    alpmC = strstr(linea, "[ALPM]");
    alpmScripC = strstr(linea, "[ALPM-SCRIPTLET]");
    pacmanC = strstr(linea, "[PACMAN]");

    //Si se encuentra una, se procede a actualizar la informacion
    if(alpmC != NULL){
        alpm++;
    } 
    else if(alpmScripC!= NULL){
        alpmScrip++;
    }else if(pacmanC!= NULL){
        pacman++;
    }

}

//Funcion encargada de registrar la informacion de un nuevo paquete instalado
void instalacion(char linea[]){
    char* nombre;
    char* fecha;
    int i;

    nombre = strtok(linea," []");
    fecha  = nombre;

    //se tokeniza hasta obtener la palabra clave installed
    while (strcmp(nombre,"installed")!=0)
    {
        nombre = strtok (NULL, " ");
    }

    //se tokeniza una vez mas para obtener el nombre
    nombre = strtok(NULL," ");

    //se registra el nombre y la fecha de instalacion, se inicilizan los demas valores
    strcpy(arr[INDEX].nombre, nombre);
    strcpy(arr[INDEX].installed, fecha);
    strcpy(arr[INDEX].removed, "-");
    strcpy(arr[INDEX].updated, "-");
    arr[INDEX].contador = 0;
    INDEX ++;
}

//funcion encargada de actualizar la informacion de un paquete que ha sido borrado 
void borrar(char linea[]){
    char* nombre;
    char* fecha;
    int i = 0;

    nombre = strtok(linea," []");
    fecha  = nombre;

    //se tokeniza hasta obtener la palabra clave removed
    while (strcmp(nombre,"removed")!=0)
    {
        nombre = strtok (NULL, " ");
    }

    //se tokeniza una vez mas para obtener el nombre
    nombre = strtok(NULL," ");

    //se recorre el array hasta encontrar el paquete con el nombre que coincida
    //cuando se obtiene, se agrega la fecha de borrado
    for(i = 0; i < ARRAY_SIZE ; i++){
        if(strcmp(arr[i].nombre,nombre)==0){
            strcpy(arr[i].removed, fecha);
            return;
        }
    }
}

//funcion encargada de actualizar la informacion de un paquete cuando se actualiza 
void actualizar(char linea[]){
    char* nombre;
    char* fecha;
    int i = 0;

    nombre = strtok(linea," []");
    fecha  = nombre;

    //se tokeniza hasta obtener la palabra clave upgraded
    while (strcmp(nombre,"upgraded")!=0)
    {
        nombre = strtok (NULL, " ");
    }

    //se tokeniza una vez mas para obtener el nombre
    nombre = strtok(NULL," ");

    //se recorre el array hasta encontrar el paquete con el nombre que coincida
    //cuando se obtiene, se agrega la fecha de aztualizacion y se suma 1 al contador de actualizaciones
    for(i = 0; i < ARRAY_SIZE ; i++){
        if(strcmp(arr[i].nombre,nombre)==0){
            strcpy(arr[i].updated, fecha);
            arr[i].contador ++;
            return;
        }
    }
}

//funcion principal, encargada de leer cada linea del log y pasarla a las demas funciones
void analizeLog(char *logFile) {
    printf("Generating Report from: [%s] log file\n", logFile);

    // Implement your solution here.
    FILE* file = fopen(logFile, "r");

    //Se valida que el archivo existe
    if(!file){
        printf("\n ERROR EL ARCHIVO %s NO EXISTE\n  ", logFile);
        return;
    }

    char linea[500];

    //Ciclo que lee el archivo hasta que no se obtienen lineas
    while (fgets(linea, sizeof(linea), file)) {
        //se llama al contador y al registrador del tipo de linea
        count(linea);
        tipoPaquete(linea);
    }

    //finalmente se llama al denerador del reporte y se cierra el archivo
    printf("Report is generated at: [%s]\n", REPORT_FILE);
    generateReport(REPORT_FILE);
    fclose(file);
}

//funcion encargada de general el reporte 
void generateReport() {

    //se recorre el arreglo y se registran los archivos sin updates
    //Ademas se cuentan los archivos que han sido actualizados
    int i;
    for(i = 0; i < INDEX ; i++){
        if(arr[i].contador == 0){
            strcat(noActual, arr[i].nombre);
            strcat(noActual,", ");
        }else{
            packActualizados++;
        }
    }

    //Se realizan las System calls requeridas en el read me del proyecto

    //Primera System Call open()
    int fil = open(REPORT_FILE, O_CREAT | O_WRONLY , 0600);
    
    //Segunda System Call write()
	write(fil, "Pacman Packages Report\n", strlen("Pacman Packages Report\n"));
    write(fil, "----------------------\n", strlen("Pacman Packages Report\n"));

    //Tercera System Call close()
    close(fil);   

    //Una vez se hacen las System Calls requeridas, se procede a relizar la escritura
    //Usando otras funciones mejores y mucho mas utiles para el proyecto
    FILE* file = fopen(REPORT_FILE,"r+");

    fseek(file, 0, SEEK_END);
    fprintf(file, "----------------------\n");
	fprintf(file, " - Installed packages: %d\n", instalados);
	fprintf(file, " - Removed packages: %d\n", borrados);
    fprintf(file, " - Total Upgrades: %d\n", actualizados);
	fprintf(file, " - Upgraded packages: %d\n", packActualizados);
	fprintf(file, " - Currently installed: %d\n", instalados - borrados);

    fprintf(file, "----------------------\n");
    fprintf(file, "     General Stats    \n");
    fprintf(file, "----------------------\n");
    fprintf(file, "- Oldest package: %s\n", arr[0].nombre);
    fprintf(file, "- Newest package: %s\n", arr[INDEX-1].nombre);
    fprintf(file, "- Package with no upgrades: %s\n", noActual);
    fprintf(file, "- [ALPM-SCRIPTTLET] type count : %d\n", alpmScrip);
    fprintf(file, "- [ALPM] count : %d\n", alpm);
    fprintf(file, "- [PACMAN] count : %d\n", pacman);
    
    fprintf(file, "----------------------\n");
    fprintf(file, "    List of Packages  \n");
    fprintf(file, "----------------------\n");

    //Se recorre el array, imprimiendo la informacion de cada paquete
	for(int i = 0; i < INDEX; i++) {
	
        fprintf(file, " - Package Name: %s\n", arr[i].nombre);
        fprintf(file, "\t - Install date: %s\n", arr[i].installed);
        fprintf(file, "\t - Last update date: %s\n", arr[i].updated);
        fprintf(file, "\t - Number of updates: %d\n", arr[i].contador);
        fprintf(file, "\t - Removal date: %s\n\n", arr[i].removed);
		
	}

    //Se cierra el archivo
	fclose(file);
}

int main(int argc, char **argv) {

    //Se verifica que se reciba el numero de atributos adecuados
    if (argc < 5 || argc > 5) {
	    printf("ELEMENTOS INCORRECTOR PARA CORRER:. /pacman-analizer.o \n");
	    return 1;
    }
    //Se verifica que la entrada sea adecuada, se asigna la variable de archivo y se llama a la funcion principal
    else if (argc == 5 && strcmp(argv[1],"-input")==0 && strcmp(argv[3],"-report")==0){
        REPORT_FILE=argv[4];
        analizeLog(argv[2]);
    }

    return 0;
}