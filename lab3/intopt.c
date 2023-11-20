#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#define EPSILON __FLT_EPSILON__
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
int glob;

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
        if(s->c[i] > EPSILON){
            return i;
        }
    }
    return -1;
}

void intopt_print(simplex_t* s);
void pivot(simplex_t* s, int row, int col);
int initial(simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var);
double xsimplex(simplex_t* s,int m, int n, double** a, double* b, double* c, double* x, double y, int* var, int h);
int init(simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var){
    glob+=1;
    int k,r;
    s->m = m;
    s->n = n;
    s->a = a;
    s->b = b;
    s->c = c;
    s->x = x;
    s->y = y;
    s->var = var;

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

    if (s->var == NULL){
        s->var = calloc(m+n+1, sizeof(int));
        for(int i=0; i < m+n+1; i++){
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

void prepare(simplex_t* s, int k){
    glob+=1;
    int m = s->m;
    int n = s->n;
    int i;
    //making room for x_m+n @ var[n] by moving var[n..n+m-1] one step
    for(i = m +n; i > n; i-=1){
        s->var[i]=s->var[i-1];
    }
    s->var[n] = m+n;
    n = n+1;
    for (i=0; i<m; i+=1){
        s->a[i][n-1]=1;
    }
    free(s->x); /* NOTE: This might not be required*/
    s->x = make_vector(m+n);
    free(s->c); /* NOTE: This might not be required*/
    s->c = make_vector(n);
    s->c[n-1] = -1;
    s->n = n;
    pivot(s,k,n-1);
}
int initial(simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var){
    glob+=1;
    int i,j,k;
    double w;
    k = init(s, m, n, a, b, c, x, y, var);
    if (s->b[k]>=0){
        return 1;
    }
    prepare(s,k);
    n = s->n;
    s->y = xsimplex(s,m,n,s->a, s->b, s->c, s->x,0, s->var,1);
    for (i = 0; i < m+n; i+=1){
        if (fabs(s->x[i])>EPSILON){
            free(s->x);
            free(s->c);
            return 0;
        } else {
            break;
        }
    }
    if (i>=n){
        for (j = k = 0; k<n; k +=1){
            if (fabs(s->a[i-n][k]) > fabs(s->a[i-n][j])){
                j = k;
            }
        }
        pivot(s, i-n, j);
        i = j;
    }
    if (i < n-1){
        k = s->var[i];
        s->var[i] = s->var[n-1];
        s->var[n-1] = k;
        for(k = 0; k < m; k+=1){
            w = s->a[k][n-1];
            s->a[k][n-1] = s->a[k][i];
            s->a[k][i] = w;
        }
    } else {
        //we do nothing
    }
    free(s->c);
    s->c = c;
    //free(s->y) // TODO if we have memory leak
    s->y = y;
    for (k = n-1; k < n+m-1; k+=1){
        s->var[k] = s->var[k+1];
    }
    n = s->n = s->n-1;
    double* t = make_vector(n);
    for (k = 0; k<n; k +=1){
        for (j = 0; j < n; j+=1){
            if (k == s->var[j]){
                t[j] = t[j] + s->c[k];
                goto next_k;
            }
        }
        for (j = 0; j<m; j+=1){
            if(s->var[n+j]==k){
                break;
            }
        }
        s->y = s->y + s->c[k]*s->b[j];
        for(i = 0; i < n; i += 1){
            t[i]=t[i]-s->c[k]*s->a[j][i];
        }
        next_k:;
    }
    for (i = 0; i < n; i+=1) s->c[i] = t[i];
    free(t);
    free(s->x);
    return 1;
}

void pivot(simplex_t* s, int row, int col){
    glob+=1;
    double** a = s->a;
    double* b = s->b;
    double* c = s->c;
    int m = s->m;
    int n = s->n;
    int i,j,t;
    t = s->var[col];
    s->var[col] = s->var[n+row];
    s->var[n+row] = t;
    s->y = s->y+c[col]*b[row]/a[row][col];
    for (i = 0; i < n; i = i+1){
        if (i != col){
            c[i] = c[i]-c[col]*a[row][i]/a[row][col];
        }
    }
    c[col] = - c[col]/a[row][col];
    for(i=0; i<m;i=i+1){
        if (i!=row){
            b[i] = b[i] - a[i][col] * b[row] / a[row][col];
        }
    }
    for(i=0; i<m;i=i+1){
        if (i!=row){
            for(j=0; j<n;j+=1){
                if(j!=col){
                    a[i][j] = a[i][j] - a[i][col]*a[row][j]/a[row][col];
                }
            }
        }
    }
    for(i=0; i < m; i +=1){
        if(i!=row){
            a[i][col] = -a[i][col]/a[row][col];
        }
    }
    b[row] = b[row]/a[row][col];
    a[row][col]= 1/a[row][col];
}
void free_simplex(simplex_t* s){
    free(s->var);
    for (int i = 0; i < s->m; i+=1){
    free(s->a[i]);
    }
    free(s->a);
    free(s->b);
    free(s->x);
    free(s->c);
    free(s);
}

double xsimplex(simplex_t* s,int m, int n, double** a, double* b, double* c, double* x, double y, int* var, int h){
    glob+=1;
    int i, row, col;
    if (!initial(s,m,n,a,b,c,x,y,var)){
        s->y=NAN;
        return s->y;
    }
    while ((col = select_nonbasic(s))>=0){
        row = -1;
        for (i = 0; i < m; i+=1){
            if (a[i][col]>EPSILON &&
             (row < 0 || (b[i]/a[i][col] < b[row]/a[row][col]))){
                row = i;
            }
        }
        if (row < 0){
            s->y=INFINITY;
            return s->y;
        }
        pivot(s, row,col);
    }
    if (h == 0){
        for(i = 0; i <n; i+=1){
            if (s->var[i]<n){
                x[s->var[i]] = 0;
            }
        }
        for(i = 0; i <m; i+=1){
            if (s->var[n+i]<n){
                x[s->var[n+i]] = s->b[i];
            }
        }
    } else {
        for(i = 0; i < n; i+=1) x[i] = 0;
        for(i = n; i < n+m; i+=1) x[i] = s->b[i-n];
    }
    return s->y;
}

double simplex(simplex_t* s,int m, int n, double** a, double* b, double* c, double* x, double y){
    glob+=1;
    return xsimplex(s,m,n,a,b,c,x,y,NULL,0);
}

void intopt_print(simplex_t* s){
    printf("maximize:\n");
    for (int i = 0; i < s->n; i+=1) printf("%.2lf x_%d + ", s->c[i], s->var[i]);
    printf("%.2lf \n", s->y);
    printf("subject to:\n");
    for (int i = 0; i < s->m; i+=1){
        printf("x_%d = ", s->var[i+s->n]);
        printf("%.2lf - (",s->c[i]);
        for (int j = 0; j < s->n; j+=1){
            printf("%.2lf x_%d", s->a[i][j], s->var[j]);
            if (j+1<s->n) printf(" + ");
        }
        printf(")\n");
    }
}


int main(int argc, char** argv){
    int m,n;
    simplex_t* s;
    s = malloc(sizeof(simplex_t));
    scanf("%d %d", &m, &n);
    //pretty_print(s);
    double** a = make_matrix(m,n+1);
    double* b = make_vector(m);
    double* x = make_vector(n+1);
    double* c = make_vector(n);
    double y = 0;
    double solution = simplex(s,m,n,a,b,c,x,y);
    printf("\nz = %f\n", s->y);
    free_simplex(s);
    return 0;
}

