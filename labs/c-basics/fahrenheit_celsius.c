#include <stdio.h>

#define   LOWER  0       /* lower limit of table */
#define   UPPER  3000     /* upper limit */
#define   STEP   200      /* step size */

/* print Fahrenheit-Celsius table */

int main()
{
    int fahr;

    for (fahr = LOWER; fahr <= UPPER; fahr = fahr + STEP)
	printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));

    return 0;
}
