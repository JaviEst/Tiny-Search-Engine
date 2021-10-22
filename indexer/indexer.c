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

int main(void) {
    int id = 1;
    char *dirname = "../pages/";

    webpage_t *page = pageload(id, dirname);

    int success;
    int pos = 0;
    char *result;
    // Loop thru html code and get all the words
    while ((pos = webpage_getNextWord(page, pos, &result)) > 0) {
        success = NormalizeWord(result);
        if ( success != 0 ) {
            exit(EXIT_FAILURE);
        }
        printf("%s\n", result);
        free(result);
    }
    exit(EXIT_SUCCESS);
}