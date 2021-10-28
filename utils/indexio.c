 /* indexio.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Tue Oct 26 10:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Implements ...
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
#include <indexio.h>
#include <hash.h>

// Global
FILE *fp;
static int HSIZE = 1000;

//---------------------------- index_to_file ----------------------------------
// Description:   helper function to write index of single word to a file
// Inputs:        void pointer to the element 
// Outputs:       writes/updates content to the following form:
//                    <word> <docID1> <count1>....<docIDN> <countN> 
//------------------------------------------------------------------------
void index_to_file(void *elementp) {
    word_t *word = (word_t*)elementp;

    // Check if file has been created/opened
    if ( fp == NULL ) {
        return;
    }
    
    // Write word to indexnm
    fprintf(fp, "%s", word->word);

    // Grab docID-count pair and write to file
    document_t *doc = (document_t*)qget(word->docs);
    while ( doc != NULL ) {
        fprintf(fp, " %d %d", doc->id, doc->instances);
        free(doc);
        doc = (document_t*)qget(word->docs);
    }
    fprintf(fp, "\n");
}

//---------------------------- indexsave ----------------------------------
// Description:   saves index to a file
// Inputs:        hashtable pointer, filepath
// Outputs:       content of indexnm should have the following:
//                    <word> <docID1> <count1>....<docIDN> <countN> 
//------------------------------------------------------------------------
int32_t indexsave(hashtable_t *htp, char *filepath) {
    fp = fopen(filepath, "w");
    happly(htp, &index_to_file);
    fclose(fp);

    // Check if file exists
    if( access( filepath, F_OK ) != 0 ) {
        return 1;
    } 
    return 0;
}

//---------------------------- indexload -------------------------------------
// Description:   loads a saved index 
// Inputs:        filepath where the file is stored
// Outputs:       returns a hastable populated with the info in file given
//---------------------------------------------------------------------------
hashtable_t *indexload(char *filepath) {
    // Open a hash table and open given file
    hashtable_t *htp = hopen(HSIZE);
    FILE *fp = fopen(filepath, "r");

    char temp_word[20] = {'\0'};
    char newline[20] = {'\0'};
    char c;
    long id;
    int instances;
    int32_t success;

    // Go through file line by line until reaching end of file
    while ( (fscanf(fp, "%s", temp_word)) != EOF ) {
        char *word = malloc(strlen(temp_word)+1);
        strcpy(word, temp_word);
        word_t *word_struct = malloc(sizeof(word_t));
        queue_t *qp  = qopen();

        // Go through entire line until reaching end of line
        fscanf(fp, "%s", newline);
        while ( strcmp(newline, "\n") != 0 ) {
            id = strtoul(newline, NULL, 0);
            fscanf(fp, "%d", &instances);

            // Create and populate document
            document_t *doc = malloc(sizeof(document_t));
            doc->id = id;
            doc->instances = instances;

            // Add document to the queue
            success = qput(qp, (void*)doc);
            if ( success != 0 ) {
                return NULL;
            }
            if ( (c = fgetc(fp)) == '\n' ) {
                break;
            }
            fscanf(fp, "%s", newline);
        }
        // Populate word struct and add to hash table
        word_struct->word = word;
        word_struct->docs = qp;
        success = hput(htp, (void*)word_struct, word, strlen(word));
        if ( success != 0 ) {
            return NULL;
        }  
    }
    fclose(fp);
    return htp;
}
