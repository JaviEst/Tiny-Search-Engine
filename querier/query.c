 /* query.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Sat Oct 30 10:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Implements an indexer ...
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>
#include <pageio.h>
#include <indexio.h>

static int BUFF_SIZE = 1000;

//---------------------------- search_word ---------------------------------
// Description:   compare two words
// Inputs:        void pointer to the elements and void pointer
//                to the key it should be compared to 
// Outputs:       true if word found, otherwise false
//---------------------------------------------------------------------------
bool search_word(void *elementp, const void *keyp) {
    word_t *word = (word_t*)elementp;
    char *key = (char*)keyp;
    
    if ( strcmp(word->word, key) == 0 ) {
        return true;
    }
    return false;
}

//---------------------------- search_doc ---------------------------------
// Description:   compare two document ids
// Inputs:        void pointer to the elements and void pointer
//                to the key it should be compared to 
// Outputs:       true if document found, otherwise false
//---------------------------------------------------------------------------
bool search_doc(void *elementp, const void *keyp) {
    document_t *doc = (document_t*)elementp;
    int *key = (int*)keyp;
    
    if ( doc->id == *key ) {
        return true;
    }
    return false;
}

//---------------------------- free_h_elements ------------------------------
// Description:   Free the memory allocates to the elements in the hashtable
// Inputs:        void pointer to the element 
// Outputs:       frees memory allocated to the elements
//---------------------------------------------------------------------------
void free_h_elements(void *elementp) {
    word_t *word = (word_t*)elementp;
    free(word->word);

    // Loop to free the memory allocated for each document
    document_t *doc_entry = (document_t*)qget(word->docs);
    while ( doc_entry != NULL ) {
        free(doc_entry);
        doc_entry = (document_t*)qget(word->docs);
    }
    free(doc_entry);

    // Close the queue and free the word struct memory
    qclose(word->docs);
    free(word);
}

//---------------------------- parse_string ---------------------------------
// Description:   parses thru input query to verify and normalize characters
// Inputs:        input query (string) & memory flag (bool)
// Outputs:       returns a queue of normalized strings
//                  - flag = true if input query is valid
//                  - flag = false if input query is not valid
//---------------------------------------------------------------------------
queue_t* parse_string(char *input, bool *flag) {
    queue_t *queryqp = qopen();
    char *delim = " \t\n";
    int i;
    int32_t success;

    // Loop thru input query string by string
    char *token = strtok(input, delim);
    while ( token != NULL ) {
        // Loop thru string and check if characters are valid
        for (i = 0; i < strlen(token); i++) {
            if( !isalpha(token[i]) ) {
                *flag = false;
                qclose(queryqp);
                return NULL;
            }
            // If valid convert to lowercase
            token[i] = tolower(token[i]);
        }

        // Ignore word if it is AND or OR
        if ( strcmp(token, "and") != 0 && strcmp(token, "or") != 0 ) {
            // Add each word in the query to a queue
            success = qput(queryqp, (void*)token);
            if ( success != 0 ) {
                return NULL;
            }
        }

        // Get next token/string in the input query
        token = strtok(NULL, delim);
    }
    return queryqp;
}

int main(void) {
    char *buffer = calloc(BUFF_SIZE, BUFF_SIZE);
    char input[BUFF_SIZE];
    char *word;
    bool flag;
    int curr_instances;
    char *filepath = "../index/indexnm";
    int rank = 100;
    int id = 1;

    // Load index 
    hashtable_t *htp = indexload(filepath);
    if ( htp == NULL ) {
        exit(EXIT_FAILURE);
    }

    // Prompt user for input until EOF / CTRL+D
    printf("> ");
    while ( fgets(input, BUFF_SIZE, stdin) != NULL ) {
        flag = true;
        buffer = realloc(buffer, strlen(input)+1);
        // Check if reallocation worked
        if ( buffer == NULL ) {
            exit(EXIT_FAILURE);
        }

        strcpy(buffer, input);

        // If input is empty prompt the user again
        if ( buffer[0] == '\n' ) {
            printf("> ");
            continue;
        }

        // Check query queue for valid input
        queue_t *queryqp = parse_string(buffer, &flag);
        if ( queryqp == NULL && flag == false) {
            printf("[invalid query]\n> ");
            continue;
        }
        if ( queryqp == NULL && flag == true) {
            exit(EXIT_FAILURE);
        }

        // Loop thru query word by word
        word = (char*)qget(queryqp);
        while ( word != NULL ) {
            // Search for word in indexer
            word_t *result = (word_t*)hsearch(htp, &search_word, word, strlen(word));
            if ( result != NULL ) {
                // Loop thru queue of documents
                document_t *doc = (document_t*)qsearch(result->docs, &search_doc, (void*)&id);
                if ( doc != NULL ) {
                    curr_instances = doc->instances;
                    printf("%s:%d ", word, curr_instances);
                    // Update rank if there is a new minimum count value
                    if ( curr_instances < rank ) {
                        rank = curr_instances;
                    }
                }
            }
            word = (char*)qget(queryqp);
        }
        printf("-- %d\n> ", rank);
        rank = 100;
        qclose(queryqp);
    }
    // Free memory
    free(buffer);
    happly(htp, &free_h_elements);
    hclose(htp);
    exit(EXIT_SUCCESS);
}