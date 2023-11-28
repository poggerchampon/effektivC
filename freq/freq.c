#include <stdio.h>
#include <string.h>
typedef struct freq_t freq_t;
struct freq_t {
    char * word;
    int count;
    freq_t * next;
};
freq_t* delete_word(freq_t* words, char* word){
    freq_t* current_word = words;
    freq_t* prev = NULL;
    printf("trying to delete %s: ", word);
    while (current_word != NULL)
    {
        if (strcmp(current_word->word, word) == 0){
                printf("deleted\n");
                freq_t* next = current_word->next;
                free(current_word->word);
                current_word->word = NULL;
                free(current_word);
            if (prev == NULL){
                return next;
            } else {
                prev->next=next;
                return words;
            }
        }
        prev = current_word;
        current_word = current_word->next;
    }
    printf("not found\n");
    return words;
}

freq_t* add_word(freq_t* words, char* word, size_t size, int is_prime){
    if (is_prime){
        return delete_word(words, word);
    }
    freq_t* current_word = words;
    freq_t* prev = NULL;
    while (current_word != NULL)
    {
        if (*(current_word->word) == *word){
            current_word->count+=1;
            printf("counted %s\n",current_word->word);
            return words;
        } else {
            prev = current_word;
            current_word = current_word->next;
        }
    }
    if (prev == NULL){
    current_word = malloc(sizeof(freq_t));
    current_word->word = malloc(1+size*sizeof(char));
    current_word->word = strcpy(current_word->word, word);
    current_word->count = 1;
    current_word->next = NULL;
    printf("added %s\n", word);
    return current_word;
    } else {
    current_word = malloc(sizeof(freq_t));
    current_word->word = malloc(1+size*sizeof(char));
    current_word->word = strcpy(current_word->word, word);
    current_word->count = 1;
    current_word->next = NULL;
    prev->next = current_word;
    }
    printf("added %s\n", word);
    return words;
}
freq_t* most_common(freq_t* words){
    freq_t* highest = NULL;
    int highest_count = 0;
    freq_t* current = words;
    while (current != NULL){
        if (current->count > highest_count){
            highest = current;
            highest_count = current->count;
        }
        current = current->next;
    }

    return highest;
}
int is_prime(int n) /*taken from: https://en.wikipedia.org/wiki/Primality_test Test for now*/
{
    if (n == 2 || n == 3)
        return 1;

    if (n <= 1 || n % 2 == 0 || n % 3 == 0)
        return 0;

    for (int i = 5; i * i <= n; i += 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    }

    return 1;
}
void free_freq(freq_t* words){
    freq_t* current_word = words;
    freq_t* prev= NULL;
    while(current_word != NULL){
        prev = current_word;
        free(current_word->word);
        current_word = prev->next;
        free(prev);
    }
}
void strip_word(char* word, size_t size){
    for (int i = 0; i < size; ++i){
        if(word[i] == '\n'){
            word[i] = 0;
        }
    }
}

int main(){
    size_t size;
    freq_t * words = NULL;
    char * word = NULL;
    int line = 0;
    while (getline(&word, &size, stdin) != -1){
        ++line;
        strip_word(word, strlen(word));
        size = strlen(word);
        words = add_word(words, word, size,is_prime(line));
    }
    //free(word);
    freq_t* res = most_common(words);
    printf("result: %s %d\n",res->word,res->count);
    if (words != NULL) free_freq(words);
    return 0;
}