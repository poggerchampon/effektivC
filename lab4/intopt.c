#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#define EPSILON __FLT_EPSILON__
typedef struct simplex_t  simplex_t;
typedef struct node_t node_t;
typedef struct list_t list_t;
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
struct node_t{
    int m;
    int n;
    int k;
    int h;
    double xh;
    double ak;
    double bk;
    double* min;
    double* max;
    double** a;
    double* b;
    double* x;
    double* c;
    double z;
};
struct list_t{
    node_t* element;
    list_t* next;
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
        if(s->c[i] > EPSILON){
            return i;
        }
    }
    return -1;
}

void intopt_print(simplex_t* s);
void pivot(simplex_t* s, int row, int col);
double intopt(int m, int n, double** a, double* b, double* c, double* x);
void succ(node_t* p, list_t* h, int m, int n, double** a, double* b, double* c, int k, double ak, double bk, double* zp, double* x);
int initial(simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var);
double xsimplex(int m, int n, double** a, double* b, double* c, double* x, double y, int* var, int h);
node_t* initial_node(int m, int n, double** a, double* b, double* c);
node_t* extend(node_t* p, int m, int n,double** a, double* b, double* c, int k, double ak, double bk);
int is_integer(double* xp);
int integer(node_t* p);
void bound(node_t* p, list_t* h, double* zp, double* x);
int branch(node_t* q, double z);
void free_node(node_t* p);
void free_list(list_t* h);
void add_last(list_t* h, node_t* p);
void shuffle_left(list_t* h);
int init(simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var){
    int i,k;
    s->m = m;
    s->n = n;
    s->a = a;
    s->b = b;
    s->c = c;
    s->x = x;
    s->y = y;
    s->var = var;

    if (s->var == NULL){
        s->var = (int*) calloc(m+n+1, sizeof(int));
        //s->var = (int*) malloc((m+n+1)*sizeof(int));
        for(i = 0; i < m+n; i++){
            s->var[i] = i;
        }
    }
    
    for (k = 0, i = 1 ; i < m; i++){
        if(b[i] < b[k]){
            k = i;
        }
    }
    
    return k;
}
node_t* initial_node(int m, int n, double** a, double* b, double* c){
    node_t* p = malloc(sizeof(node_t));
    p->a = make_matrix(m+1, n+1);
    p->b = make_vector(m+1);
    p->c = make_vector(n+1);
    p->x = make_vector(n+1);
    p->min = make_vector(m);
    p->max= make_vector(n);
    p->m = m;
    p->n = n;
    for (int i = 0; i < m; i+=1){
        p->b[i] = b[i];
        for (int j = 0; j < n; j+=1) p->a[i][j] = a[i][j];
    }
    for (int i = 0; i < n; i+=1){
        p->c[i] = c[i];
        p->min[i] = -INFINITY;
        p->max[i] = INFINITY;
    }
    return p;
}
node_t* extend(node_t* p, int m, int n,double** a, double* b, double* c, int k, double ak, double bk){
    node_t* q = malloc(sizeof(node_t));
    int i,j;
    q->k=k;
    q->ak=ak;
    q->bk=bk;
    if (ak > 0 && p->max[k] < INFINITY){
        q->m = p->m;
    } else if (ak < 0 && p->min[k] > 0)
    {
        q->m=p->m;
    } else {
        q->m=p->m +1;
    }
    q->n = p->n;
    q->h= -1;
    q->a = make_matrix(q->m+1, q->n+1);
    q->b = make_vector(q->m+1);
    q->c = make_vector(q->n+1);
    q->x = make_vector(q->n+1);
    q->min = make_vector(n);
    q->max = make_vector(n);
    for (i = 0; i < n; i+=1){
        q->min[i] = p->min[i];
        q->max[i] = p->max[i];
        q->c[i] = c[i];
    }
    for (i = 0; i < m; i+=1){
        q->b[i] = b[i];
        for (j = 0; j < n; j+=1) q->a[i][j] = a[i][j];
    }
    if (ak > 0){
        if (q->max[k] == -INFINITY || bk < q->max[k]){
            q->max[k] = bk;
        }
    } else if (q->min[k] == -INFINITY ||-bk > q->min[k])
    {
        q->min[k] = -bk;
    }
    for (i = m, j = 0; j < n; j+=1){
        if(q->min[j]>-INFINITY){
            q->a[i][j] = -1;
            q->b[i] = -q->min[j];
            i += 1;
        }
        if(q->max[j]<INFINITY){
            q->a[i][j] = 1;
            q->b[i] = q->max[j];
            i+=1;
        }
    }
    return q;
}
int is_integer(double* xp){
    double x = *xp;
    double r = round(x);
    if (fabs(r-x) < EPSILON){
        *xp = r;
        return 1;
    }
    return 0;
}
int integer(node_t* p){
    int i;
    for (i = 0; i < p->n; i+=1){
        if (!is_integer(&p->x[i])){
            return 0;
        }
    }
    return 1;
}
void shuffle_left(list_t* h){
    list_t* current = h;
    if (current->next!=NULL){
        free_node(current->element);
    }
    while (current->next != NULL){
        current->element = current->next->element;
        current = current->next;
    }
    free(current);
}
void delete_node(list_t* h, double bound){
    list_t* current = h;
    list_t* prev = NULL;
    while(current != NULL){
        node_t* q = current->element;
        if(q->z<bound){
            if (prev != NULL){
                prev->next = current->next;
                free_node(q);
                free(current);
                current = prev->next;
            } else {
                shuffle_left(h); // this could turn h into NULL;
            }
        } else {
            current = current->next;
        }

    }
}
void bound(node_t* p, list_t* h, double* zp, double* x){
    if(p->z > *zp){
        *zp = p->z;
        for (int i = 0; i < p->n; i+=1) p->x[i] = x[i];
        delete_node(h, p->z);
    }
}
int branch(node_t* q, double z){
    double min, max;
    if (q->z<z)
    {
        return 0;
    }
    for (int h = 0; h <q->n; h+=1){
        if (!is_integer(&q->x[h])){
            if (q->min[h] < -INFINITY) min = 0;
            else min = q->min[h];
            max = q->max[h];
            if (floor(q->x[h])<min || ceil(q->x[h])>max){
                continue;
            }
            q->h = h;
            q->xh = q->x[h];
            /*
            for (int i = 0; i < q->m+1; i+=1){
                free(q->a[i]);
            }
            free(q->a);
            free(q->b);
            free(q->c);
            free(q->x);
            */
            return 1;
        }
    }
    return 0;
}
void free_node(node_t* p){
    if(p == NULL){
        return;
    }
    if (p->min != NULL){
    free(p->min);
    }
    if (p->max != NULL){
    free(p->max);
    }
    if (p->b != NULL){
        free(p->b);
    }
    if (p->c != NULL){
        free(p->c);
    }
    if (p->x != NULL){
        free(p->x);
    }
    if (p->a != NULL){
        for (int i = 0; i < p->m+1; i+=1){
            free(p->a[i]);
        }
        free(p->a);
    }
    free(p);
}
void free_list(list_t* h){
    list_t* next = h;
    while (next != NULL){
        free_node(h->element);
        list_t* temp = next->next;
        free(next);
        next = temp;
    }
}
void prepare(simplex_t* s, int k){
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
        s->a[i][n-1]=-1;
    }
    //free(s.x); /* NOTE: This might not be required*/
    s->x = make_vector(m+n);
    //free(s.c); /* NOTE: This might not be required*/
    s->c = make_vector(n);
    s->c[n-1] = -1;
    s->n = n;
    pivot(s,k,n-1);
}
int initial(simplex_t* s, int m, int n, double** a, double* b, double* c, double* x, double y, int* var){
    int i,j,k;
    double w;
    k = init(s, m, n, a, b, c, x, y, var);
    if (s->b[k]>=0){
        return 1;
    }
    prepare(s,k);
    n = s->n;
    s->y = xsimplex(m,n,s->a, s->b, s->c, s->x,0, s->var,1);
    for (i = 0; i < m+n; i+=1){
        if (s->var[i]==(s->m+s->n-1)){
        if (fabs(s->x[i])>EPSILON){
            free(s->x);
            free(s->c);
            return 0;
        } else {
            break;
        }
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
    //free(s.y) // TODO if we have memory leak
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
    for(i = 0; i < n; i+=1){
        if (i != col){
            a[row][i] = a[row][i]/a[row][col];
        }
    }
    b[row] = b[row]/a[row][col];
    a[row][col]= 1/a[row][col];
}
void add_last(list_t* h, node_t* p){
    while (h->next != NULL){
        h = h->next;
    }
    h->next = malloc(sizeof(list_t));
    h->next->element = p;
}
double xsimplex(int m, int n, double** a, double* b, double* c, double* x, double y, int* var, int h){
    simplex_t s;
    int i, row, col;
    if (!initial(&s,m,n,a,b,c,x,y,var)){
        free(s.var);
        return NAN;
    }
    while ((col = select_nonbasic(&s))>=0){
        row = -1;
        for (i = 0; i < m; i+=1){
            if (a[i][col]>EPSILON &&
             (row < 0 || (b[i]/a[i][col] < b[row]/a[row][col]))){
                row = i;
            }
        }
        if (row < 0){
            free(s.var);
            return INFINITY;
        }
        pivot(&s, row,col);
    }
    if (h == 0){
        for(i = 0; i <n; i+=1){
            if (s.var[i]<n){
                x[s.var[i]] = 0;
            }
        }
        for(i = 0; i <m; i+=1){
            if (s.var[n+i]<n){
                x[s.var[n+i]] = s.b[i];
            }
        }
        free(s.var);
    } else {
        for(i = 0; i < n; i+=1) x[i] = 0;
        for(i = n; i < n+m; i+=1) x[i] = s.b[i-n];
    }
    return s.y;
}
double simplex(int m, int n, double** a, double* b, double* c, double* x, double y){
    return xsimplex(m,n,a,b,c,x,y,NULL,0);
}
void intopt_print(simplex_t* s){
    printf("maximize:\n");
    for (int i = 0; i < s->n; i+=1) printf("%.2lf x_%d + ", s->c[i], s->var[i]);
    printf("%.2lf \n", s->y);
    printf("subject to:\n");
    for (int i = 0; i < s->m; i+=1){
        printf("x_%d = ", s->var[i+s->n]);
        printf("%.2lf - (",s->b[i]);
        for (int j = 0; j < s->n; j+=1){
            printf("%.2lf x_%d", s->a[i][j], s->var[j]);
            if (j+1<s->n) printf(" + ");
        }
        printf(")\n");
    }
}
void succ(node_t* p, list_t* h, int m, int n, double** a, double* b, double* c, int k, double ak, double bk, double* zp, double* x){
    node_t* q = extend(p,m,n,a,b,c,k,ak,bk);
    if (q==NULL) return;
    q->z = simplex(q->m, q->n, q->a,q->b, q->c,q->x,0);
    if (isfinite(q->z)){
        if (integer(q)) {
            bound(q, h, zp, x);
        }
        else if (branch(q,*zp))
        {
            add_last(h,q);
            return;
        }
        
    }
    free_node(q);
}
double intopt(int m, int n, double** a, double* b, double* c, double* x){
    node_t* p = initial_node(m, n, a,b,c);
    list_t* h = malloc(sizeof(list_t));
    h->element = p;
    h->next = NULL;
    double z = -INFINITY;
    p->z = simplex(p->m, p->n,p->a,p->b,p->c,p->x, 0);
    if (integer(p) || isfinite(p->z)){
        z = p->z;
        if (integer(p)){
            for (int i  = 0; i < n; i+=1){
                x[i] = p->x[i];
            }
            free_list(h);
            return z;
        }
    }
    branch(p,z);
    while (h != NULL){
        p = h->element;
        succ(p,h,m,n,a,b,c,p->h, 1, floor(p->xh), &z, x);
        succ(p,h,m,n,a,b,c,p->h, -1, -ceil(p->xh), &z, x);
        free_node(p);
        list_t* temp = h->next;
        free(h);
        h = temp;
    }
    free_list(h);
    if (z == -INFINITY) return NAN;
    return z;
}
int main(int argc, char** argv){
    int m,n;
    scanf("%d %d", &m, &n);
    double** a = make_matrix(m,n+1);
    double* b = make_vector(m);
    double* x = make_vector(n+1);
    double* c = make_vector(n);
    for (int i = 0; i < n; i += 1){
        scanf("%lf", &c[i]);
    }

    for (int i = 0; i < m; i += 1){
        for (int j = 0; j < n; j += 1){
            scanf("%lf", &a[i][j]);      
        }
    }
    for (int i = 0; i < m; i += 1){
        scanf("%lf", &b[i]);     
    }
    double y = 0;
    double solution = intopt(m, n, a,b,c,x);
    printf("\nz = %f\n", solution);
    //free_simplex(s);
    for (int i = 0; i < m; i+=1){
        free(a[i]);
    }
    free(a);
    free(b);
    free(c);
    free(x);
    return 0;
}

