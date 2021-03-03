#include <stdio.h>
#include <stdlib.h>

/* month_day function's prototype*/

char* month_name(int n) {

   static char* name[] = {
       "Illegal month",
       "Jan", "Feb", "Mar",
       "Apr", "May", "Jun",
       "Jul", "Aug", "Sep",
       "Oct", "Nov", "Dec"
   };

   return (n < 1 ||  n > 12) ? name[0] : name[n];
}

void month_day(int year, int yearday){

    int daytab[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if(yearday > 366 || yearday <= 0){
        printf("INVALID DAY \n");
        return;
    }

    if(year%4 == 0 || year%100 == 0)
        daytab[2] = 29;
    else
        daytab[2] = 28;

    int* pdaytab = &daytab[0];

    int day = 0;
    int tempday = 0;
    int month = 0;

    //printf("Puntero: %2d \n", *(pdaytab+1));

    if(yearday == 366 && year%4==0){
        month =  12;
        tempday = 31;
    }
    else if(yearday >31){
        while(yearday >= 0){
            month++; 
            tempday = yearday;
            yearday -= *(pdaytab + month);
            //printf("Prueba dia %3d mes %1d \n",tempday,month);
        }
    }

    else
        month ++; 

    //day = day - tempday;
    char* monthname = month_name(month); 
    printf("%3s %2d, %4d \n",monthname,tempday,year);
}

int main(int argc, char *argv[]) {

    if(argc == 3){
        month_day(atoi(argv[1]),atoi(argv[2]));
    } 
    else
        printf("Datos invalidos \n");

    
    return 0;
}

