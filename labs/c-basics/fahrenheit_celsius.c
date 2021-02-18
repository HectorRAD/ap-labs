#include <stdio.h>

/* print Fahrenheit-Celsius table */

int main(int argc, char *argv[])
{
    int fahr;

    if (argc == 2){
        fahr = atoi(argv[1]);
        printf("The value in celsius is: %3f \n",(5.0/9.0)*(fahr-32));
    }
    else if(argc == 4){
        int LOWER = atoi(argv[1]);
        int UPPER = atoi(argv[2]);
        int STEP = atoi(argv[3]);
        for (fahr = LOWER; fahr <= UPPER; fahr = fahr + STEP)
	        printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
    }
    else{
        printf("Wrong input");
    }
	

    return 0;
}
