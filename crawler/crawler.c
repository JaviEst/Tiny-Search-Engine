 /* crawler.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Tue Oct 12 10:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Implements a crawler ...
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "../utils/webpage.h"
#include "../utils/queue.h"

// Global
char *URL = "https://thayer.github.io/engs50/";
int DEPTH = 0;


// log one word (1-9 chars) about a given url
inline static void logr(const char *word, const int depth, const char *url)
{
  printf("%2d %*s%9s: %s\n", depth, depth, "", word, url);
}

//---------------------------- bag_delete -------------------------------
// Description:   delete a webpage
// Inputs:        pointer to a webpage
// Outputs:       deletes webpage
//-----------------------------------------------------------------------
void bag_delete(webpage_t *page) {
    webpage_delete(page);
}

//---------------------------- print_queue -------------------------------
// Description:   print a queue
// Inputs:        pointer to a queue
// Outputs:       prints a queue
//------------------------------------------------------------------------
void print_queue(queue_t *qp) {
    webpage_t *page = (webpage_t*)qget(qp);
    char *url = webpage_getURL(page);
    // Loop thru the entire queue
    while ( page != NULL ) {
        logr("Internal", DEPTH, url);
        bag_delete(page);
        page = (webpage_t*)qget(qp);
        url = webpage_getURL(page);
    }
    bag_delete(page);
}

int main(){
    // Create new webpage
    webpage_t *page = webpage_new(URL, DEPTH, NULL);
    queue_t *qp = qopen();

    // Checking that the fetch worked correctly
    if( !webpage_fetch( page ) ) {
        exit(EXIT_FAILURE);
    }

    // Get html code
    // char *html = webpage_getHTML(page);
    //printf("Found html: %s\n", html);

    webpage_t *internal_page;
    int32_t success;
    int pos = 0;
    char *result;
    // Scan the html code
    while ( ( pos = webpage_getNextURL( page, pos, &result ) ) > 0 ) {
        if ( IsInternalURL( result ) ) {
            internal_page = webpage_new(result, DEPTH, NULL);
            success = qput(qp, (void*)internal_page);
            if ( success != 0 ) {
                exit(EXIT_FAILURE);
            }
        }
        free(result);
    }

    // Print urls in the queue    
    print_queue(qp);

    // Free memory
    qclose(qp);
    bag_delete(page);

    exit(EXIT_SUCCESS);
}
