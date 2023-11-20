#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "error.h"
#include "poly.h"


struct poly_t{
    int e;
    int b;
    poly_t* next;
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
    poly_t* head = malloc(sizeof(poly_t));
    poly_t* p = head;
    p->e = 0;
    p->b = 0;
    p->next  = NULL;
    int done_with_term = 0;
    int looking_for_exp = 0;
    int looking_for_base = 1;
    int sign = 1;
    while(*str){
        k = 0;
        //Do something with current
        if (isdigit(*str)){
            k = digit(str);
            for (int i = k; i >= 10;){
                i=i/10;
                ++str;
            }
            if (looking_for_exp){
                p->e = k;
                looking_for_exp = 0;
                done_with_term = 1;
            } else if (looking_for_base) {
                p->b = sign*k;
                sign = 1;
                looking_for_base = 0;
            } else {
                p->b = sign*k;
                sign = 1;
                p->e = 0;
            }
        } else if (*str == ' ')
        {
            /* code */
        } else if (*str == '+')
        {
            sign = 1;

        }  else if ( *str == '-'){
            sign = -1;
        }
            else if (*str == 'x')
        {
            if (p->b == 0){
                p->b = sign;
            }
            ++str;
            if (*str == '^'){
                looking_for_exp = 1;
            } else {
                p->e = 1;
                done_with_term = 1;
                --str;
            }
        }
        
        
        ++str;
        if (done_with_term){
            poly_t* q = malloc(sizeof(poly_t));
            q->e = 0;
            q->b = 0;
            q->next = NULL;
            p->next = q;
            p = q;
            done_with_term = 0;
            looking_for_exp = 0;
            looking_for_base = 1;

        }
    }
    if (!done_with_term){
        if(p->b != 0){
            p->e = 0;
        } else {
            free(p);
        }
    }
    return head;
}

void free_poly(poly_t* p){
    poly_t* q;
    while (p->next != NULL){
        q = p->next;
        p->next = q->next;
        free(q);
    }
    free(p);
}
poly_t* find_last(poly_t* p){
    poly_t* q = p;
    while(q->next != NULL){
        q = q->next;
    }
    return q;
}
poly_t* find_last_before(poly_t* start, poly_t* target){
    poly_t* q = start;
    while(q->next != NULL){
        if(q->next == target){
            return q;
        }
        q = q->next;
    }
    return q;
}

void poly_scuffed(poly_t* start){
    poly_t* current = start;
    poly_t* previous = NULL;
    poly_t* next;
    int changed = 0;
    while(current != NULL){
        //printf("current exp:%d\n", current->e);
        next = current->next;
        if (previous == NULL){
            previous = current;
            current = next;
            continue;
        }
        if (next != NULL){
            if (next->e > current->e){
                changed = 1;
                previous->next = next;
                current->next = next->next;
                next->next = current;
                previous = next;
                continue;
            }
        }
        previous = current;
        current = next;
    }
    if (changed){
        poly_scuffed(start);
    }
}



poly_t* poly_sort(poly_t* r_start){
    if (r_start == NULL){
        return NULL;
    }
    if (r_start->next == NULL){
        return r_start;
    }
    poly_t* pivot = find_last(r_start);
    int mean = abs(r_start->e - pivot->e)/2;
    poly_t* previous_node = NULL;
    poly_t* current_node = r_start;
    poly_t* next;

    while(current_node != pivot){
        next = current_node->next;
        if(current_node->e<=mean){
            if(previous_node == NULL){
                r_start = next;
            } else {
                previous_node->next = next;
            }
            current_node->next = pivot->next;
            pivot->next = current_node;
        } else {
            previous_node = current_node;
        }
        current_node = next;
    }
    
    if(r_start == pivot){
        return r_start;
    }
    poly_t* lhs_end = find_last_before(r_start,pivot);
    lhs_end->next = NULL;
    if (pivot->next != NULL){
    pivot = poly_sort(pivot);
    }
    r_start = poly_sort(r_start);
    lhs_end->next = pivot;
    return r_start;
}

void poly_resolve(poly_t* r_start){
    poly_t* current = r_start->next;
    poly_t* prev = r_start;
    poly_t* next;
    while (current != NULL)
    {
        next = current->next;
        if(current->e == prev->e){
            prev->b += current->b;
            prev->next = next;
            free(current);
        } else {
        prev = current;
        }
        current = next;
        
    }
}


poly_t*	mul(poly_t* p , poly_t* q){
    poly_t* r_start = malloc(sizeof(poly_t));
    poly_t* r = r_start;
    poly_t* r_next;
    poly_t* p_itr = p;
    poly_t* q_itr;
    while(p_itr != NULL){
        q_itr = q;
        while (q_itr != NULL)
        {
            r->b=p_itr->b*q_itr->b;
            r->e=p_itr->e+q_itr->e;
            if (q_itr->next!=NULL ||p_itr->next != NULL){
            r_next = malloc(sizeof(poly_t));
            r->next = r_next;
            r = r_next;
            } else {
                r->next=NULL;
            }
            q_itr = q_itr->next;
        }
        p_itr = p_itr->next;
    }
    //poly_sort(r_start);
    poly_scuffed(r_start);
    poly_resolve(r_start);
    return r_start;
}

int abs(int i){
    return i>0 ? i : -i;
}

void print_poly(poly_t* p){
    poly_t* q = p;
    int first = 1;
    while (q != NULL){
        if (first){
            first = 0;
            if(q->b<0){
                printf("- ");
            }
        }
    if (q->b != 1 && q->b != -1 && q->e>0){
        printf("%d", abs(q->b));
    }
    if (q->e > 1){
        printf("x^%d", q->e);
    } else if ( q->e == 1){
        printf("x");
    } else {
        printf("%d", abs(q->b));
    }
    
    if (q->next != NULL){
        if (q->next->b > 0){
            printf(" + ");
        } else {
            printf(" - ");
        }
    }
    q = q->next;
    }
    printf("\n");
}