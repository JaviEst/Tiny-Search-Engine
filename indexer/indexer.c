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

typedef struct document {
    int id;
    int instances;
} document_t;

typedef struct word {
    char *word;
    queue_t *docs;
} word_t;

// Global variables
int HSIZE = 1000;
int SUM;
long ID;

//---------------------------- NormalizeWord --------------------------------
// Description:   converts a word to lowercase
// Inputs:        pointer to a string
// Outputs:       the word should be lowercase
//---------------------------------------------------------------------------
bool NormalizeWord(char *word) {
    // Check the length is greater than 3
    if ( strlen(word) < 3 ) {
        return false;
    }

    int i;
    // Loop thru word and make every character lowercase
    for(i = 0; i < strlen(word); i++){
        if ( !isalpha(word[i]) ) {
            return true;
        }
        word[i] = tolower(word[i]);
    }
    return true;
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

//------------------------ docqueue_to_queue --------------------------
// Description:   add a doc queue to a queue
// Inputs:        queue, doc queue
// Outputs:       queue is populated with doc queue
//------------------------------------------------------------------------
int docqueue_to_queue(queue_t* qp, queue_t* docqueue) {
    int32_t success = qput(qp, (void*)docqueue);
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

//---------------------------- sumwords ---------------------------------
// Description:   counts the total instances of all words in webpage
// Inputs:        void pointer to the element 
// Outputs:       updates global sum
//---------------------------------------------------------------------------
void sumwords(void *elementp) {
    word_t *word = (word_t*)elementp;
    int i;

    for ( i = 1; i <= ID; i++ ) {
        document_t *doc = (document_t*)qsearch(word->docs, &search_doc, (void*)&i);
        if ( doc != NULL ){
            SUM += doc->instances;
        }
    }
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
                word_t* word, queue_t* qqdoc, const char* key,
                int id) {
    int32_t success;

    // Create document 
    document_t *doc = NULL;

    // Check if the word is in hashtable
    word_t *result = (word_t*)hsearch(htp, searchfn, key, strlen(key));
    if ( result != NULL ) {
        // If word found, find respective doc
        document_t *temp = (document_t*)qsearch(result->docs, &search_doc, (void*)&id);
        if ( temp != NULL ){
            temp->instances += 1;
            return 0;
        }
        else {
            // Create document
            doc = malloc(sizeof(document_t));
            doc->id = id;
            doc->instances = 1;

            // Add doc to document's queue
            success = qput(result->docs, (void*)doc);
            return success;
        }
        return 1;
    }

    // Create documents' queue for each word
    queue_t *docsq = qopen();
    doc = malloc(sizeof(document_t));
    doc->id = id;
    doc->instances = 1;

    // Add doc to document's queue
    success = qput(docsq, (void*)doc);
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    // Add docqueue to queue
    success = docqueue_to_queue(qqdoc, docsq);
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    word->docs = docsq;

    // Add unique word to hashtable
    success = hput(htp, (void*)word, key, strlen(key));
    return success;
}


int main(int argc, char** argv) {
    ID = strtoul(argv[1], NULL, 0);
    char *dirname = "../pages/";

    hashtable_t *htp = hopen(HSIZE);
    queue_t *wordq = qopen();
    queue_t *struct_wordq = qopen();
    queue_t *qqdoc = qopen();

    webpage_t *page;
    char *result;
    int success;
    bool word_flag = false;
    int pos = 0;
    int i;
    // Load up to the given id page
    for ( i = 1; i <= ID; i++ ) {
        page = pageload(i, dirname);
        // Loop thru html code and get all the words
        while ((pos = webpage_getNextWord(page, pos, &result)) > 0) {
            word_flag = NormalizeWord(result);
            if ( word_flag ) {
                // Add word to queue
                success = word_to_queue(wordq, result);
                if ( success != 0 ) {
                    exit(EXIT_FAILURE);
                }

                // Add word struct to queue
                word_t *word = malloc(sizeof(word_t));
                word->word = result;
                word->docs = NULL;
                success = word_struct_to_queue(struct_wordq, word);

                // Add word to hash or update instances
                success = word_to_hash(htp, &search_word, word, qqdoc, (const char*)result, i);
            }
            else {
                free(result);
            }
        }
        pos = 0;
        webpage_delete(page);
    }
    
    // Count a sum of all instances in hashtable
    SUM = 0;
    happly(htp, &sumwords);
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

    // Loop thru the entire queue to free the struct word memory
    queue_t *docqueue = (queue_t*)qget(qqdoc);
    while ( docqueue != NULL ) {
        document_t *doc_entry = (document_t*)qget(docqueue);
        while ( doc_entry != NULL ) {
            free(doc_entry);
            doc_entry = (document_t*)qget(docqueue);
        }
        free(doc_entry);
        qclose(docqueue);
        docqueue = (queue_t*)qget(qqdoc);
    }

    // Free memory
    qclose(wordq);
    qclose(struct_wordq);
    qclose(qqdoc);
    hclose(htp);

    exit(EXIT_SUCCESS);
}