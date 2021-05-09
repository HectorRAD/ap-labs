
extern int infof(const char *format, ...);
extern int warnf(const char *format, ...);
extern int errorf(const char *format, ...);
extern int panicf(const char *format, ...);

int main() {
    infof("BIENVENIDO AL PROGRAMA %s!, v.%d \n", "logger", 2);
    warnf("AVERTENCIA: VARIABLES NO USADAS EN LAS LINEAS %d, %d, %d\n", 1, 5, 15402);
    errorf("ERROR: ENTRADA NO ACEPTADA %s\n", "inputPruebaPrueba");
    panicf("ERROR DE SEGEMENTACION: LINEA %d FUERA DE ALCANCE\n", 9);
    return 0;
}