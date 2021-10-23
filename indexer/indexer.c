 /* indexer.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Fri Oct 22 10:33:29 2021 (-0400)
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

typedef struct word {
    char *word;
    int instances;
} word_t;

// Global variables
int HSIZE = 1000;
int SUM;

//---------------------------- NormalizeWord --------------------------------
// Description:   converts a word to lowercase
// Inputs:        pointer to a string
// Outputs:       the word should be lowercase
//---------------------------------------------------------------------------
int NormalizeWord(char *word) {
    // Check the length is greater than 3
    if ( strlen(word) < 3 ) {
        return 0;
    }

    int i;
    // Loop thru word and make every character lowercase
    for(i = 0; i < strlen(word); i++){
        if ( !isalpha(word[i]) ) {
            return 0;
        }
        word[i] = tolower(word[i]);
    }
    return 0;
}

//---------------------------- word_to_queue ------------------------------
// Description:   add a word to a queue
// Inputs:        queue, pointer to a character (string) 
// Outputs:       queue is populated with word
//------------------------------------------------------------------------
int word_to_queue(queue_t* qp, char* word) {
    int32_t success = qput(qp, (void*)word);
    return success;
}

//------------------------ word_struct_to_queue --------------------------
// Description:   add a word struct to a queue
// Inputs:        queue, pointer to a word_t 
// Outputs:       queue is populated with word_t
//------------------------------------------------------------------------
int word_struct_to_queue(queue_t* qp, word_t* word) {
    int32_t success = qput(qp, (void*)word);
    return success;
}

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

//---------------------------- count_words ---------------------------------
// Description:   counts the total instances of all words in webpage
// Inputs:        void pointer to the element 
// Outputs:       updates global sum
//---------------------------------------------------------------------------
void count_words(void *elementp) {
    word_t *word = (word_t*)elementp;
    SUM += word->instances;
}

//---------------------------- word_to_hash ------------------------------
// Description:   add a word to hashtable or update instances
// Inputs:        hashtable, pointer to search function, element (word),
//                key, iteration of words found
// Outputs:       hash is populated with word_t if not in hashtable
//                or update word_t instance
//------------------------------------------------------------------------
int word_to_hash(hashtable_t *htp,
                bool (*searchfn)(void* elementp, const void* searchkeyp),
                word_t* word, const char* key,
                int iteration) {

    int32_t success;
    // First word added to hashtable without checking
    if ( iteration == 0 ) {
        word->instances = 1;

        success = hput(htp, (void*)word, key, sizeof(key));
        return success;
    }

    // Check if the word is in hashtable
    word_t *result = (word_t*)hsearch(htp, searchfn, key, sizeof(key));
    if ( result != NULL ) {
        // If word found, update instance count
        result->instances += 1;
        return 0;
    }
    // Add unique word to hashtable
    word->instances = 1;
    success = hput(htp, (void*)word, key, sizeof(key));
    return success;
}


int main(void) {
    int id = 1;
    char *dirname = "../pages/";

    webpage_t *page = pageload(id, dirname);
    hashtable_t *htp = hopen(HSIZE);
    queue_t *wordq = qopen();
    queue_t *struct_wordq = qopen();

    char *result;
    int success;
    int pos = 0;
    int iteration = 0;
    // Loop thru html code and get all the words
    while ((pos = webpage_getNextWord(page, pos, &result)) > 0) {
        success = NormalizeWord(result);
        if ( success != 0 ) {
            exit(EXIT_FAILURE);
        }
        // Add word to queue
        success = word_to_queue(wordq, result);
        if ( success != 0 ) {
            exit(EXIT_FAILURE);
        }

        // Add word struct to queue
        word_t *word = malloc(sizeof(word_t));
        word->word = result;
        word->instances = 0;
        success = word_struct_to_queue(struct_wordq, word);

        // Add word to hash or update instances
        success = word_to_hash(htp, &search_word, word, (const char*)result, iteration);

        iteration++;
    }
    // Count a sum of all instances in hashtable
    SUM = 0;
    happly(htp, &count_words);
    printf("SUM: %d\n", SUM);

    // Loop thru the entire queue to free the word memory
    char *word_entry = (char*)qget(wordq);
    while ( word_entry != NULL ) {
        free(word_entry);
        word_entry = (char*)qget(wordq);
    }
    free(word_entry);

    // Loop thru the entire queue to free the struct word memory
    word_t *struct_entry = (word_t*)qget(struct_wordq);
    while ( struct_entry != NULL ) {
        free(struct_entry);
        struct_entry = (word_t*)qget(struct_wordq);
    }
    free(struct_entry);

    // Free memory
    qclose(wordq);
    qclose(struct_wordq);
    hclose(htp);
    webpage_delete(page);

    exit(EXIT_SUCCESS);
}