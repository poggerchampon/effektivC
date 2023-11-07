#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct simplex_t  simplex_t;
struct simplex_t{
    int m;
    int n;
    int* var;
    double** a;
    double* b;
    double* x;
    double* c;
    double y;
    
};
int next_int(){
        int     x;
        int     c;

        x = 0;
        while (isdigit(c = getchar()))
                x = 10 * x + c - '0';

        return x;
}
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

double** make_matrix(int m, int n){
    double** a;
    int i;
    a = calloc(m, sizeof(double*));
    for (i = 0; i < m; i += 1)
    a[i] = calloc(n, sizeof(double));
    return a;
}

double* make_vector(int size){
    double* vec;
    vec = calloc(size, sizeof(double));
    return vec;
}

simplex_t* init(int m, int n){
    simplex_t* s;
    s = malloc(sizeof(simplex_t));
    
    s->m = m;
    s->n = n;
    s->a = make_matrix(m,n);
    s->b = make_vector(m);
    s->c = make_vector(n);
    s->x = make_vector(n+1);

    for (int i = 0; i < n; i += 1){
        s->c[i] = next_double();
    }

    for (int i = 0; i < m; i += 1){
        for (int j = 0; j < n; j += 1){
            s->a[i][j]=next_double();         
        }
    }
    for (int i = 0; i < m; i += 1){
        s->b[i] = next_double();       
    }
    return s;
}

void pretty_print(simplex_t* s){
    printf("max z = %10.3lfx + %10.3lfy \n", s->c[0], s->c[1] );
    for(int i=0; i<2;i++){
    printf("%10.3lfx %10.3lfy \u2264 %10.3lf3 \n", s->a[i][0], s->a[i][1], s->b[i]);}
}
int main(int argc, char** argv){
    int m,n;
    simplex_t* s;
    m = next_int();
    n = next_int();
    s = init(m,n);
    pretty_print(s);

    return 0;
}

