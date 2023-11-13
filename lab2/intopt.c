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
int select_nonbasic(simplex_t* s){
    int i;
    for(i=0; i < s->n; i++){
        if(s->c[i] > 0.000001){
            return i;
        }
    }
    return -1;
}
int init(int m, int n, simplex_t* s){
    
    int k,r;
    s->m = m;
    s->n = n;
    s->a = make_matrix(m,n);
    s->b = make_vector(m);
    s->c = make_vector(n);
    s->x = make_vector(n+1);
    

    for (int i = 0; i < n; i += 1){
        scanf("%lf", &s->c[i]);
    }

    for (int i = 0; i < m; i += 1){
        for (int j = 0; j < n; j += 1){
            scanf("%lf", &s->a[i][j]);      
        }
    }
    for (int i = 0; i < m; i += 1){
        scanf("%lf", &s->b[i]);     
    }

    if(s->var == NULL){
        s->var = calloc(m+n+1, sizeof(int));
        for(int i=0; i < m+n; i++){
            s->var[i] = i;
        }
    }
    
    for (k = 0, r = 1 ; r < m; r++){
        if(s->b[r] < s->b[k]){
            k = r;
        }
    }
    
    return k;
}

void pretty_print(simplex_t* s){
    printf("max z = %10.3lfx + %10.3lfy \n", s->c[0], s->c[1] );
    for(int i=0; i<2;i++){
    printf("%10.3lfx %10.3lfy \u2264 %10.3lf \n", s->a[i][0], s->a[i][1], s->b[i]);}
}

int main(int argc, char** argv){
    int m,n,k;
    simplex_t* s;
    s = malloc(sizeof(simplex_t));
    scanf("%d %d", &m, &n);
    k = init(m,n,s);
    pretty_print(s);
    printf("%d", k);

    return 0;
}

