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
#include "../indexer/indexer.c"

// Global
static int HSIZE = 1000;

int main(void) {
    hashtable_t *htp = hopen(HSIZE);
    char *filepath = "../index/indexnm";

    int i, j;
    int32_t success;
    char word[15] = {'\0'};
    
    for( i = 0; i < 10; i++) {
        word_t *word_struct;
        queue_t *qp = qopen();
        sprintf(word, "test%d", i);

        for( j = 0; j < 10; j++) {
            document_t *doc;
            doc->id = j;
            doc->instances = j+i;

            // Add document to the queue
            success = qput(qp, (void*)doc);
            if ( success != 0 ) {
                exit(EXIT_FAILURE);
            }   
        }
        // Populate word struct and add to hash table
        word_struct->word = word;
        word_struct->docs = qp;
        success = hput(htp, (void*)word_struct, word, strlen(word));
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
    hashtable_t *test_htp = hopen(HSIZE);
    test_htp = indexload(filepath);
    if ( test_htp == NULL ) {
        exit(EXIT_FAILURE);
    }

    // Test saving of index that was just loaded
    success = indexsave(test_htp, filepath);
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}