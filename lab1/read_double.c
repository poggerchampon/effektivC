#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
static int isdigit_etc(int c){
    return isdigit(c) || c == '-' || c == '.' || c == '0';
}

static double next_double()
{
        double     x;
        int     c;
        int decimal_counter;
        x = 0.0;
        int count_decimals = 0;
        decimal_counter = 0;
        int is_neg = 0;
        while (isdigit_etc(c = getchar())){
                if (c == '.'){
                    count_decimals = 1;
                    continue;
                }
                if (c == '-'){
                    is_neg = 1;
                    continue;
                }
                x = 10 * x + (c - '0');
                if(count_decimals) {
                    decimal_counter+=1;
                    }
        }
        if(count_decimals){
            for (int i = 0; i < decimal_counter; i+=1) {
                x *= 0.1;
                }
        }
        if (is_neg){
            return -x;
        }
        return x;
}

int main(int argc, char** argv){
double m, n;
m = next_double();
n = next_double();
double j = 0.6;
printf("m = %f, n = %f, j = %f \n", m, n, j);



return 0;
}