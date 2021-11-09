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
#include <dirent.h>
#include <ctype.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>
#include <pageio.h>
#include <indexio.h>
#include <query_indexer.c>

static int BUFF_SIZE = 1000;
int MAX_RANK = 0;

typedef struct rank {
    int id;
    int doc_rank;
    char *url;
} rank_t;

// //---------------------------- search_word ---------------------------------
// // Description:   compare two words
// // Inputs:        void pointer to the elements and void pointer
// //                to the key it should be compared to 
// // Outputs:       true if word found, otherwise false
// //---------------------------------------------------------------------------
// bool search_word(void *elementp, const void *keyp) {
//     word_t *word = (word_t*)elementp;
//     char *key = (char*)keyp;
    
//     if ( strcmp(word->word, key) == 0 ) {
//         return true;
//     }
//     return false;
// }

// //---------------------------- search_doc ---------------------------------
// // Description:   compare two document ids
// // Inputs:        void pointer to the elements and void pointer
// //                to the key it should be compared to 
// // Outputs:       true if document found, otherwise false
// //---------------------------------------------------------------------------
// bool search_doc(void *elementp, const void *keyp) {
//     document_t *doc = (document_t*)elementp;
//     int *key = (int*)keyp;
    
//     if ( doc->id == *key ) {
//         return true;
//     }
//     return false;
// }

//-------------------------------- find_max ---------------------------------
// Description:   find highest rank in a given queue
// Inputs:        void pointer to element in queue
// Outputs:       assigns highest rank to global variable MAX_RANK
//---------------------------------------------------------------------------
void find_max(void *elementp) {
    rank_t *doc = (rank_t*)elementp;
    
    if ( doc->doc_rank > MAX_RANK ) {
        MAX_RANK = doc->doc_rank;
    }
}

//----------------------- find_highest_rank ---------------------------------
// Description:   find document with highest rank
// Inputs:        void pointer to element in queue and void pointer
//                to key it should be compared to
// Outputs:       returns true if rank is highest, false otherwise
//---------------------------------------------------------------------------
bool find_highest_rank(void *elementp, const void *keyp) {
    rank_t *doc = (rank_t*)elementp;
    int *max_rank = (int*)keyp;
    
    if ( doc->doc_rank == *max_rank ) {
        return true;
    }
    return false;
}

//-------------------------- find_ranked_id ---------------------------------
// Description:   find document id associated with rank
// Inputs:        void pointer to element in queue and void pointer
//                to key it should be compared to
// Outputs:       returns true if id found, false otherwise
//---------------------------------------------------------------------------
bool find_ranked_id(void *elementp, const void *keyp) {
    rank_t *doc = (rank_t*)elementp;
    int *id = (int*)keyp;
    
    if ( doc->id == *id ) {
        return true;
    }
    return false;
}

//---------------------------- free_h_elements ------------------------------
// Description:   frees memory allocated to elements in hashtable
// Inputs:        void pointer to element 
// Outputs:       frees memory allocated to each element
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
// Description:   frees memory allocatef to sorted array
// Inputs:        sorted array of ranked docs & number of ranked docs in array 
// Outputs:       frees memory allocated to each element and array itself
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
// Outputs:       returns the url for the file given, null otherwise
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
// Inputs:        array of words (parsed query), input query (string), memory flag (bool)
//                pointer to number of words in query
// Outputs:       returns a queue of normalized strings
//                  - flag = true if input query is valid
//                  - flag = false if input query is not valid
//---------------------------------------------------------------------------
char** parse_string(char** array_words, char *input, bool *flag, int *num_elements) {
    char *delim = " \t\n";
    int i;
    int j = 0;

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

        // If first word is AND or OR, invalid query
        if ( ( strcmp(token, "and") == 0 || strcmp(token, "or") == 0 )
             && j == 0 ) {
            *flag = false;
            return NULL;
        }

        // Add each word in the query to an array of strings
        array_words[(*num_elements)] = malloc((strlen(token)+1)*sizeof(char));
        strcpy(array_words[(*num_elements)], token);
        (*num_elements)++;

        // Check previous word and current word for adjacent operators
        if ( j > 0 ) {
            if ( ( (strcmp(array_words[j], "and") == 0) || (strcmp(array_words[j], "or") == 0) )
                      && ( (strcmp(array_words[j-1], "and") == 0) || (strcmp(array_words[j-1], "or") == 0) ) ) {
                *flag = false;
                return NULL;    
            }
        }

        // Get next token/string in the input query
        token = strtok(NULL, delim);
        j++;
    }
    // If last word is AND or OR, invalid query
    if ( strcmp(array_words[(*num_elements)-1], "and") == 0 
         || strcmp(array_words[(*num_elements)-1], "or") == 0 ) {
        *flag = false;
        return NULL;
    }

    for ( i = 0; i < *num_elements; i++) {
        printf("%s ", array_words[i]);
    }
    printf("\n");

    return array_words;
}

//---------------------------- process_ands ---------------------------------
// Description:   parses thru input query and returns <andsequence> 
// Inputs:        query, AND sequence, pointer to number of words in 
//                query, and iteration
// Outputs:       array of strings, subarray of query if 'or' is found,
//                NULL if there is an error
//---------------------------------------------------------------------------
char** process_ands(char **query, char **and_sequence, int num_words, int *iteration, int *num_elements) {
    // Loop thru query and return and sequence 
    for ( ; *iteration < num_words; (*iteration)++) {
        
        if ( strcmp(query[*iteration], "or" ) == 0 ) {
            (*iteration)++;
            return and_sequence;
        }

        // Add each word in the query to an array of strings
        and_sequence[(*num_elements)] = malloc((strlen(query[*iteration])+1)*sizeof(char));
        strcpy(and_sequence[(*num_elements)], query[*iteration]);
        (*num_elements)++;
    }
    return and_sequence;
}

//---------------------------- find_doc_rank ---------------------------------
// Description:   parses thru input query and finds ranks of documents 
//                containing all queried words
// Inputs:        hashtable pointer, ranked queue pointer, directory of 
//                crawled pages, document id, number of words,
//                array of strings query, number of already ranked docs, 
//                and presence of word flag 
// Outputs:       queue of ranks and (number of elements in queue),
//                NULL if there is an error
//---------------------------------------------------------------------------
queue_t* find_doc_rank(hashtable_t *htp, queue_t *rankqp, char *pagesdir, int id, 
                       int num_words, char **query, int *num_ranked_docs, bool *word_flag) {
    int k;
    char *word;
    int curr_instances;
    int32_t success;

    int query_rank = 1000;

    char filepath[50] = {'\0'};
    sprintf(filepath, "%s%d", pagesdir, id);
    // Loop through crawled pages 
    while ( access(filepath, F_OK) == 0 ) {
        *word_flag = true;

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
                    // Update query rank if there is a new minimum count value
                    if ( curr_instances < query_rank ) {
                        query_rank = curr_instances;
                    }
                }
                else {
                    // If they aren't then set the flag to false
                    *word_flag = false;
                }
            }
            else {
                *word_flag = false;
            }
        }

        // If all the words in the query were present in the page then
        if ( *word_flag ) {
            // Initialize ranking of a document containing all words
            rank_t *temp_rank = malloc(sizeof(rank_t));
            temp_rank->id = id;
            temp_rank->doc_rank = query_rank;

            char *url = malloc(100*sizeof(char));
            success = get_doc_url(url, filepath);
            if ( success != 0 ) {
                printf("Failed getting url from crawler file!\n");
                return NULL;
            }
            temp_rank->url = url;

            // Add ranked document to a queue of ranked documents
            success = qput(rankqp, (void*)temp_rank);
            if ( success != 0 ) {
                printf("Failed adding element to queue of rankings!\n");
                return NULL;
            }
            (*num_ranked_docs)++;
        }
        query_rank = 1000;
        id++;

        // Update the filepath with the new filename (id)
        sprintf(filepath, "%s%d", pagesdir, id);
    }
    return rankqp;
}

//---------------------------- check_arg ---------------------------------
// Description:   check for correct user inputs
// Inputs:        argc (number of inputs passed from command line) and
//                argv (array of arguments from command line)
// Outputs:       0 if the input is correct, nonzero otherwise
//------------------------------------------------------------------------
int32_t check_arg(int argc, char** argv){
    // Check the number of input arguments is correct
    if ( argc < 3 || argc > 4 ) {
        printf("usage: query <pageDirectory> <indexFile> [-q]\n");
        return 1;
    }

    // Check if pagedir exists
    DIR *dirp = opendir(argv[1]);
    if ( dirp == NULL ) {
        printf("usage: query <pageDirectory> <indexFile> [-q]\n");
        printf("<pageDirectory> must exist\n");
        return 1;
    }
    closedir(dirp);

    return 0;
}



int main(int argc, char** argv) {
    // Check the inputs from the user
    int32_t correct_arg = check_arg(argc, argv);
    if ( correct_arg != 0 ){
        exit(EXIT_FAILURE);
    }

    // Get inputs from user
    char *pagesdir = argv[1];
    char *filepath = argv[2];


    char *buffer = calloc(BUFF_SIZE, BUFF_SIZE);
    char input[BUFF_SIZE];
    bool flag;
    bool word_flag;
    int i, j;
    int id;
    int iteration;
    int num_words;
    int num_elements;
    int num_ranked_docs;
    int32_t success;

    // Create index
    success = indexer(pagesdir, filepath);

    // Load index 
    hashtable_t *htp = indexload(filepath);
    if ( htp == NULL ) {
        exit(EXIT_FAILURE);
    }

    // Initialize queue of documents that contain all the words in the query
    queue_t *final_rank = qopen();

    // Prompt user for input until EOF / CTRL+D
    printf("> ");
    while ( fgets(input, BUFF_SIZE, stdin) != NULL ) {
        // if ( feof(stdin) ) {
        //     printf("\n> ");
        //     continue;
        // }
        i = 0;
        id = 1;
        iteration = 0;
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

        while ( iteration <= num_words-1 ) {
            num_elements = 0;
            char **and_sequence = malloc(20*sizeof(char*));
            // Get <andsequence> from query
            and_sequence = process_ands(query, and_sequence, num_words, &iteration, &num_elements);

            // Find ranks of documents containing all the queried words
            queue_t *rankqp = qopen();
            rankqp = find_doc_rank(htp, rankqp, pagesdir, id, num_elements, and_sequence, &num_ranked_docs, &word_flag);
            // if ( !word_flag )
            if ( rankqp == NULL ) {
                exit(EXIT_FAILURE);
            }
            
            // Conditions for or sequences
            if ( i > 0 ) {
                // Get doc from new <andsequence> ranking
                rank_t *and_sequence_doc = (rank_t*)qget(rankqp);
                // printf("");
                while ( and_sequence_doc != NULL ) {
                    // Compare new <andsequence> doc ids to final ranking doc ids  
                    rank_t *final_doc_rank = (rank_t*)qsearch(final_rank, &find_ranked_id, (void*)&and_sequence_doc->id);
                    if ( final_doc_rank != NULL ) {
                        // Update final ranking for each doc that is in both and sequences
                        final_doc_rank->doc_rank += and_sequence_doc->doc_rank;

                        // Free memory
                        free(and_sequence_doc->url);
                        free(and_sequence_doc);
                    }
                    else if ( final_doc_rank == NULL ) {
                        // Add unique doc from new <andsequence> to final ranking
                        success = qput(final_rank, (void*)and_sequence_doc);
                        if ( success != 0 ) {
                            printf("Failed adding element to final rank queue!");
                            exit(EXIT_FAILURE);
                        }
                    }
                    and_sequence_doc = (rank_t*)qget(rankqp);
                }
                qclose(rankqp);
            }
            else {
                // Concat initial <andsequence> to final ranking
                qconcat(final_rank, rankqp);
                // Close second queue when empty 
                if ( num_ranked_docs == 0 ) {
                    qclose(rankqp);
                }
            }
            i++;
            free_array(and_sequence, num_elements);
        }


        // Print the queue sorted
        j = 0;
        while ( j < num_ranked_docs ) {
            MAX_RANK = 0;
            qapply(final_rank, &find_max);
            rank_t *temp_rank = (rank_t*)qremove(final_rank, &find_highest_rank, &MAX_RANK);
            while ( temp_rank != NULL ) {
                printf("rank: %d doc: %d url: %s", temp_rank->doc_rank, temp_rank->id, temp_rank->url);

                free(temp_rank->url);
                free(temp_rank);

                temp_rank = (rank_t*)qremove(final_rank, &find_highest_rank, &MAX_RANK);
            }
            j++;
            free(temp_rank);
        }
        // Free memory allocated for each element in the array
        free_array(query, num_words);

        printf("> ");
    }

    // Free memory
    free(buffer);
    qclose(final_rank);
    happly(htp, &free_h_elements);
    hclose(htp);
    exit(EXIT_SUCCESS);
}