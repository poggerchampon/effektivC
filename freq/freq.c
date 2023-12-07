#define  _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct freq_t freq_t;
struct freq_t {
    char * word;
    int count;
    freq_t * next;
};

enum INST {
    ADD = 1,
    DELETE = 0,
    NOT_FOUND = 2,
    FOUND = 3
};


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
char* stripped(char* word, size_t len){
    word[len-1] = '\0';
    return word;
}
enum INST locate_word(freq_t* word_list, char * word, enum INST instruction){
    freq_t* current_word = word_list;
    freq_t* prev = NULL;
    while (current_word != NULL)
    {
        if(current_word->word == NULL){

        } else {
        if (strcmp(current_word->word,word) == 0){
            goto found;
        }}
        prev = current_word;
        current_word = current_word->next;
    }
    
    return NOT_FOUND;
    found:;
    if (instruction == DELETE){
        if (current_word == word_list){
            free(current_word->word);
            current_word->count = 0;
            current_word->word = NULL;
        } else {
            prev->next = current_word->next;
            free(current_word->word);
            free(current_word);
        }
    } else if (instruction == ADD) {
        current_word->count++;
    }
    else {
        exit(1); //CODE IS FUCKED
    }
    return FOUND;
}
freq_t* next_empty(freq_t* word_list){ // word_list most exist
    if (word_list->word == NULL){
        return word_list; // assume only the head word is able to be NULL;
    }
    freq_t* current_word = word_list;
    while (current_word->next != NULL){
        current_word = current_word->next;
    }
    current_word -> next = malloc(sizeof(freq_t));
    current_word -> next -> next = NULL;
    current_word -> next -> word = NULL;
    current_word -> next -> count = 0;
    return current_word->next;
}

void try_to_delete(freq_t* word_list, char * word){
    printf("trying to delete %s: ", word);
    enum INST response = locate_word(word_list, word, DELETE);
    if (response == NOT_FOUND){
        printf("not found\n");
        return;
    }
    if (response == FOUND){
        printf("deleted\n");
        return;
    }
}

void try_to_add(freq_t* word_list, char * word, size_t len){
    enum INST response = locate_word(word_list, word, ADD);
    if (response == FOUND){
        printf("counted %s\n",word);
        return;
    }
    if (response == NOT_FOUND){
        freq_t* next_word = next_empty(word_list); //this is the next word;
        char* new_word = calloc(len, sizeof(char)); // create a new char*
        for (int i = 0; i < len; i++){
            new_word[i] = word[i]; //copies from buffer to actual word
        }
        next_word->count = 1;
        next_word->word = new_word;
        printf("added %s\n",word);
    }
}


void handle_line(freq_t* word_list, char * word, size_t len, int line){
    if (is_prime(line)){
        try_to_delete(word_list, stripped(word, len));
    } else {
        try_to_add(word_list, stripped(word, len),len);
    }
}

void free_word_list(freq_t* word_list){
    freq_t* current = word_list;
    freq_t* prev = NULL;
    while(current != NULL){
        if (current->word != NULL){
            free(current->word);
            prev = current;
            current = current->next;
            free(prev);
        }
    }
}


freq_t* most_common(freq_t* word_list){
    freq_t* current = word_list->next;
    freq_t* highest_word = word_list;
    while(current != NULL){
        if (current->count > highest_word->count){
            highest_word = current;
        }
        current = current->next;
    }
    return highest_word;
}
int main(){
    freq_t* word_list = malloc(sizeof(freq_t));
    word_list->word = NULL;
    word_list->count = 0;
    word_list->next = NULL; //make sure everything is initalised to values we want
    char *buffer;
    size_t bufsize = 32;
    size_t characters;

    buffer = (char *)malloc(bufsize * sizeof(char));
    int line = 1;
    while ((characters = getline(&buffer, &bufsize, stdin)) != -1){
        handle_line(word_list, buffer, characters, line);
        line++;
    }
    free(buffer);
    freq_t* res = most_common(word_list);
    printf("result: %s %i\n", res->word, res->count);
    free_word_list(word_list);
    return 0;
}