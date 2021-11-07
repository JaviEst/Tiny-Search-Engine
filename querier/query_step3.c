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
int MAX_RANK = 0;

typedef struct rank {
    int id;
    int doc_rank;
    char *url;
} rank_t;

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

//-------------------------------- find_max ---------------------------------
// Description:   find highest rank in the queue
// Inputs:        
// Outputs:       
//---------------------------------------------------------------------------
void find_max(void *elementp) {
    rank_t *doc = (rank_t*)elementp;
    
    if ( doc->doc_rank > MAX_RANK ) {
        MAX_RANK = doc->doc_rank;
    }
}

//----------------------- find_highest_rank ---------------------------------
// Description:   find document with the highest rank
// Inputs:        
// Outputs:       
//---------------------------------------------------------------------------
bool find_highest_rank(void *elementp, const void *keyp) {
    rank_t *doc = (rank_t*)elementp;
    int *max_rank = (int*)keyp;
    
    if ( doc->doc_rank == *max_rank ) {
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

//---------------------------- free_ranked_array ----------------------------
// Description:   Free the memory allocates to the sorted array
// Inputs:        sorted array of ranked docs & number of ranked docs in the array 
// Outputs:       frees memory allocated to the elements and array
//---------------------------------------------------------------------------
void free_array(char **array, int num_elements) {
    int j;
    // Free memory allocated for each element in the array
    for ( j = 0; j < num_elements; j++ ) {
        free(array[j]);
    }
    free(array);
}

//------------------------------ get_doc_url --------------------------------
// Description:   get url from id files
// Inputs:        filepath
// Outputs:       returns the url for the file given / returns null otherwise
//---------------------------------------------------------------------------
int32_t get_doc_url(char *url, char *filepath) {
    FILE *fp = fopen(filepath, "r");

    // Read first line of file as it contains the url of the page
    if ( fgets(url, 100, fp) == NULL){
        return 1;
    }
    fclose(fp);
    return 0;
}

//---------------------------- parse_string ---------------------------------
// Description:   parses thru input query to verify and normalize characters
// Inputs:        input query (string) & memory flag (bool)
// Outputs:       returns a queue of normalized strings
//                  - flag = true if input query is valid
//                  - flag = false if input query is not valid
//---------------------------------------------------------------------------
char** parse_string(char** array_words, char *input, bool *flag, int *num_elements) {
    char *delim = " \t\n";
    int i;

    // Loop thru input query string by string
    char *token = strtok(input, delim);
    while ( token != NULL ) {
        // Loop thru string and check if characters are valid
        for (i = 0; i < strlen(token); i++) {
            if( !isalpha(token[i]) ) {
                *flag = false;
                return NULL;
            }
            // If valid convert to lowercase
            token[i] = tolower(token[i]);
        }

        // Ignore word if it is AND or OR
        if ( strcmp(token, "and") != 0 && strcmp(token, "or") != 0 ) {
            // Add each word in the query to an array of strings
            array_words[(*num_elements)] = malloc((strlen(token)+1)*sizeof(char));
            strcpy(array_words[(*num_elements)], token);
            (*num_elements)++;
        }

        // Get next token/string in the input query
        token = strtok(NULL, delim);
    }

    for ( i = 0; i < *num_elements; i++) {
        printf("%s ", array_words[i]);
    }
    printf("\n");

    return array_words;
}


int main(void) {
    char *buffer = calloc(BUFF_SIZE, BUFF_SIZE);
    char input[BUFF_SIZE];
    char *word;
    bool flag;
    bool word_flag;
    int curr_instances;
    int32_t success;
    int j, k;
    int num_words;
    int num_ranked_docs;

    char *filepath = "../index/indexnm";
    char *pagesdir = "../pages/";
    int query_rank = 1000;
    int id;

    // Load index 
    hashtable_t *htp = indexload(filepath);
    if ( htp == NULL ) {
        exit(EXIT_FAILURE);
    }

    // Initialize queue of documents that contain all the words in the query
    queue_t *rankqp = qopen();

    // Prompt user for input until EOF / CTRL+D
    printf("> ");
    while ( fgets(input, BUFF_SIZE, stdin) != NULL ) {
        id = 1;
        num_words = 0;
        num_ranked_docs = 0;
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

        char **array_words = malloc(50*sizeof(char*));
        // Check query queue for valid input
        char **query = parse_string(array_words, buffer, &flag, &num_words);
        if ( query == NULL && flag == false) {
            printf("[invalid query]\n> ");
            free_array(array_words, num_words);
            continue;
        }
        if ( query == NULL && flag == true) {
            exit(EXIT_FAILURE);
        }

        char filepath[50] = {'\0'};
        sprintf(filepath, "%s%d", pagesdir, id);
        // Loop until there are no more pages crawled
        while ( access(filepath, F_OK) == 0 ) {
            word_flag = true;

            // Loop thru query word by word
            for ( k = 0; k < num_words; k++ ) {
                word = query[k];
                // Search for word in indexer
                word_t *result = (word_t*)hsearch(htp, &search_word, word, strlen(word));
                if ( result != NULL ) {
                    // Check if the words in the query are all present in the same document
                    document_t *doc = (document_t*)qsearch(result->docs, &search_doc, (void*)&id);
                    if ( doc != NULL ) {
                        // If they are then find the query rank
                        curr_instances = doc->instances;
                        // printf("%s:%d ", word, curr_instances);
                        // Update query rank if there is a new minimum count value
                        if ( curr_instances < query_rank ) {
                            query_rank = curr_instances;
                        }
                    }
                    else {
                        // If they aren't then set the flag to false
                        word_flag = false;
                    }
                }
                else {
                    word_flag = false;
                }
            }
            // If all the words in the query were present in the page then
            if ( word_flag ) {
                // Initialize ranking of a document containing all words
                rank_t *temp_rank = malloc(sizeof(rank_t));
                temp_rank->id = id;
                temp_rank->doc_rank = query_rank;

                char *url = malloc(100*sizeof(char));
                success = get_doc_url(url, filepath);
                if ( success != 0 ) {
                    printf("Failed getting url from crawler file!\n");
                    exit(EXIT_FAILURE);
                }
                temp_rank->url = url;

                // Add ranked document to a queue of ranked documents
                success = qput(rankqp, (void*)temp_rank);
                if ( success != 0 ) {
                    printf("Failed adding element to queue of rankings!\n");
                    exit(EXIT_FAILURE);
                }
                num_ranked_docs++;
            }
            query_rank = 1000;
            id++;

            // Update the filepath with the new filename (id)
            sprintf(filepath, "%s%d", pagesdir, id);
        }
        // Print the queue sorted
        j = 0;
        while ( j < num_ranked_docs ) {
            MAX_RANK = 0;
            qapply(rankqp, &find_max);
            rank_t *temp_rank = (rank_t*)qremove(rankqp, &find_highest_rank, &MAX_RANK);
            while ( temp_rank != NULL ) {
                j++;
                printf("rank: %d doc: %d url: %s", temp_rank->doc_rank, temp_rank->id, temp_rank->url);

                free(temp_rank->url);
                free(temp_rank);

                temp_rank = (rank_t*)qremove(rankqp, &find_highest_rank, &MAX_RANK);
            }
            free(temp_rank);
        }
        // Free memory allocated for each element in the array
        free_array(query, num_words);

        printf("> ");
    }
    // Free memory
    free(buffer);
    qclose(rankqp);
    happly(htp, &free_h_elements);
    hclose(htp);
    exit(EXIT_SUCCESS);
}