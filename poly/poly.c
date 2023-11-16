#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "error.h"
#include "poly.h"


struct poly_t{
    __uint128_t k;
};

int digit(const char* str){
    int sum = 0;
    while(*str){
        if (isdigit(*str)){
        sum = sum * 10 + (*str - '0');
        ++str;
        } else {
            break;
        }
    }
    return sum;
}



poly_t*	new_poly_from_string(const char* str){
    int k;
    while(*str){
        k = 0;
        //Do something with current
        if (isdigit(*str)){
            k = digit(str);
            for (int i = k; i >= 10;){
                printf("%d\n", i);
                i=i%10;
                ++str;
            }
            printf("%d\n",k);
        } else if (*str == ' ')
        {
            /* code */
        } else if (*str == '+' || *str == '-')
        {
            /* code */
        }
        
        
        ++str;
    }
    
    return NULL;
}

void free_poly(poly_t* p){
}
poly_t*	mul(poly_t* p , poly_t* q){
    return NULL;
}

void print_poly(poly_t* p){

}