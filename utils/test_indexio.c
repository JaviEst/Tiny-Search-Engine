/* test_indexio.c --- 
 * 
 * 
* Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Tue Oct 26 09:22:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test indexsave and indexload.
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
#include <hash.h>
#include <queue.h>
#include <indexio.h>

// Global
static int HSIZE = 1000;

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


int main(void) {
    hashtable_t *htp = hopen(HSIZE);
    queue_t *struct_wordq = qopen();
    queue_t *qqdoc = qopen();
    char *filepath = "../index/indexnm";

    int i, j;
    int32_t success;
    char word[15] = {'\0'};
    
    for( i = 0; i < 10; i++) {
        word_t *word_struct = malloc(sizeof(word_t));
        queue_t *qp = qopen();
        sprintf(word, "test%d", i);

        for( j = 0; j < 10; j++) {
            document_t *doc = malloc(sizeof(document_t));
            doc->id = j;
            doc->instances = j+i;

            // Add document to the queue
            success = qput(qp, (void*)doc);
            if ( success != 0 ) {
                exit(EXIT_FAILURE);
            }   
        }
        success = qput(qqdoc, (void*)qp);
        if ( success != 0 ) {
            exit(EXIT_FAILURE);
        } 

        // Populate word struct and add to hash table
        word_struct->word = word;
        word_struct->docs = qp;
        success = hput(htp, (void*)word_struct, word, strlen(word));
        if ( success != 0 ) {
            exit(EXIT_FAILURE);
        } 

        success = qput(struct_wordq, (void*)word_struct);
        if ( success != 0 ) {
            exit(EXIT_FAILURE);
        } 
    }
    // Test saving of index
    success = indexsave(htp, filepath);
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    } 

    // Test loading of index
    hashtable_t *test_htp = indexload(filepath);
    if ( test_htp == NULL ) {
        exit(EXIT_FAILURE);
    }

    filepath = "../index/indextest";

    // Test saving of index that was just loaded
    success = indexsave(test_htp, filepath);
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

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
    qclose(struct_wordq);
    qclose(qqdoc);
    hclose(htp);
    happly(test_htp, &free_h_elements);
    hclose(test_htp);
    exit(EXIT_SUCCESS);
}