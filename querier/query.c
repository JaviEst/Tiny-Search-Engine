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

//---------------------------- parse_string ---------------------------------
// Description:   parses thru input query to verify and normalize characters
// Inputs:        input query (string) & memory flag (bool)
// Outputs:       returns the normalized string to be printed
//                  - flag = true if input query is valid
//                  - flag = false if input query is not valid
//---------------------------------------------------------------------------
char* parse_string(char *input, bool *flag) {
    char *buffer = malloc(strlen(input)+1);
    char *delim = " \t\n";
    int i;
    int iteration = 0;

    // Loop thru input query string by string
    char *token = strtok(input, delim);
    while( token != NULL ) {
        // Loop thru string and check if characters are valid
        for(i = 0; i < strlen(token); i++) {
            if( !isalpha(token[i]) ) {
                *flag = false;
                free(buffer);
                return "[invalid query]";
            }
            // If valid convert to lowercase
            token[i] = tolower(token[i]);
        }
        // If it is the first iteration initialize the buffer
        if( iteration == 0 ){
            strcpy(buffer, token);
        }
        // After first iteration concatenate to initialized value
        else {
            strcat(buffer, token);
        }
        // Add space between words
        strcat(buffer, " ");
        iteration++;

        // Get next token/string in the input query
        token = strtok(NULL, delim);
    }
    return buffer;
}

int main(void) {
    char *buffer = calloc(BUFF_SIZE, BUFF_SIZE);
    char input[BUFF_SIZE];
    char *output;
    bool flag;

    // Prompt user for input until EOF / CTRL+D
    printf("> ");
    while( fgets(input, BUFF_SIZE, stdin) != NULL ) {
        flag = true;
        buffer = realloc(buffer, strlen(input)+1);
        // Check if reallocation worked
        if( buffer == NULL ) {
            exit(EXIT_FAILURE);
        }

        strcpy(buffer, input);

        // If input is empty prompt the user again
        if( buffer[0] == '\n' ) {
            printf("> ");
            continue;
        }

        output = parse_string(buffer, &flag);
        printf("%s\n> ", output);
        // If the input is valid then free the memory allocated in parse_string
        if( flag ) {
            free(output);
        }
    }
    free(buffer);
    exit(EXIT_SUCCESS);
}