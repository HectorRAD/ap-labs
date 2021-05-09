//CODIGO BASADO EN LA DOCUMENTACION DE LA API inotify
//OBTENIDA DEL SIGUIENTE LINK https://man7.org/linux/man-pages/man7/inotify.7.html

#define _XOPEN_SOURCE 500
#define NAME_SIZE 64 // Limite superior para el nombre de archivos
#define EVENT_SIZE  ( sizeof (struct inotify_event) ) 
#define BUFFER_SIZE (1024  * ( EVENT_SIZE + NAME_SIZE ))

#include "logger.h"
#include <stdlib.h>
#include <ftw.h>
#include <unistd.h>
#include <sys/inotify.h>


int inotifyFd; //id del file descriptor de la API inotify 
int wd=0; //id del watch descriptor de la API
ssize_t numRead; //numread para Read()
char buffer[BUFFER_SIZE];
char *directorio; 
int flags = 0;

static int mostrar_camb(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    
	if(tflag==FTW_D){

        wd = inotify_add_watch(inotifyFd, fpath, IN_MOVE | IN_CREATE | IN_DELETE);
        
		if(wd==-1){
            errorf("ERROR: IMPOSIBLE AGREGAR WATCH A INOTIFY");
        } 
    }

    return 0;
}

static int inicializar(char* dir){

	directorio = dir; 
	inotifyFd = inotify_init(); 

	if(inotifyFd<0){
        errorf("ERROR EN EL FD\n");
        return -1;
    }

    if (nftw(directorio, mostrar_camb, 20, flags) == -1) {
       errorf("ERROR: IMPOSIBLE CREAR NFTW.");
       return -1;
    }else{
        infof("\nStarting File/Directory Monitor on %s\n", directorio);
        panicf("-----------------------------------------------------\n");
    }

	return 0;

}

static int validarRead(){
	
	numRead=read(inotifyFd, buffer, BUFFER_SIZE );

	if(numRead == -1){
		errorf("ERROR AL USAR READ\n");
		return 0;
	}
	else if(numRead == 0){
		panicf("NO SE HAN OBTENIDO DIRECTORIOS\n");
		return 0;
	}
}

int main(int argc, char **argv) {
    
	initLogger("stdout");
    
	if(argc!=2){
        errorf("PARAMETROS INSUFICIENTES, SIGA LA ESTRUCTURA: sudo ./monitor /<dir>\n");
        return 0;
    }

	int sinError = inicializar(argv[1]);

	if(sinError == -1){
		return 0;
	}

    while(1){
        
		int contador = 0;
 
		validarRead();

        while (contador < numRead ) {
            struct inotify_event *event=(struct inotify_event*)&buffer[contador];
            
			if(event->len){

				if(event-> mask & IN_DELETE){
                    
					if(event-> mask & IN_ISDIR){
                        infof("- [Directory - Removal] - %s\n", event->name);
                    }    
                    else{
                        infof("- [File - Removal] - %s\n", event->name);
                    }

                }

				else if (event->cookie!=0){
                    
					if(event->mask & IN_MOVED_FROM){
                        if(event-> mask & IN_ISDIR){
                            infof("- [Directory - Rename] - %s -> ", event->name);
                        }    
                        else{
                            infof("- [File - Rename] - %s -> ", event->name);
                        }
                    }

                    else infof("%s\n",event->name);
                }

                else if(event-> mask & IN_CREATE){

                    if(event-> mask & IN_ISDIR){
                        infof("- [Directory - Create] - %s\n", event->name);
                    }    
                    else{
                        infof("- [File - Create] - %s\n", event->name);
                    }
                }

				else{
                    warnf("UNKNOW ACTION");
                }

                contador+=EVENT_SIZE+event->len;
            }
        }
    }

    inotify_rm_watch( inotifyFd, wd );
    close( inotifyFd );
    return 0;
} 