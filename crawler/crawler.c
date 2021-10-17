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
#include "../utils/hash.h"

// Global
char *URL = "https://thayer.github.io/engs50/";
int DEPTH = 0;
uint32_t H_SIZE = 50;


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
    webpage_delete((void*)page);
}

//---------------------------- web_to_queue -------------------------------
// Description:   add a webpage to a queue
// Inputs:        queue, pointer to a character (string) and depth 
// Outputs:       queue is populated with webpage
//------------------------------------------------------------------------
int web_to_queue(queue_t* qp, char* url, int depth, char* html) {
    webpage_t *internal_page= webpage_new(url, depth, html);
    int32_t success = qput(qp, (void*)internal_page);
    return success;
}

//---------------------------- url_to_queue ------------------------------
// Description:   add a webpage to a queue
// Inputs:        queue, pointer to a character (string) and depth 
// Outputs:       queue is populated with webpage
//------------------------------------------------------------------------
int url_to_queue(queue_t* qp, char* url) {
    int32_t success = qput(qp, (void*)url);
    return success;
}

//---------------------------- print_web_queue ---------------------------
// Description:   print a queue
// Inputs:        pointer to a queue
// Outputs:       prints a queue
//------------------------------------------------------------------------
void print_web_queue(queue_t *qp) {
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

//---------------------------- url_to_hash -------------------------------
// Description:   add a url to a hashtable
// Inputs:        hashtable, pointer to search function, element, key,
//                iteraion of urls found, pointer to a bool
// Outputs:       hash is populated with urls if the url is 
//                not already in the hashtable
//                updates a bool
//                  True if url is not in hashtable
//                  False if url is in the hashtable
//------------------------------------------------------------------------
int32_t url_to_hash(hashtable_t* htp,
                bool (*searchfn)(void* elementp, const void* searchkeyp),
                char* url, const char* key, 
                int iteration, bool* url_flag) {
    int32_t success;
    // First url added to hashtable without checking
    if ( iteration == 0 ) {
        success = hput(htp, (void*)url, key, sizeof(key));
        *url_flag = true;
        return success;
    }

    // Check if the url is in the hashtable
    char* check = (char*)hsearch(htp, searchfn, key, sizeof(key));
    if ( check != NULL ) {
        // If url found, then return false
        *url_flag = false;
        return 0;
    }

    // Add unique url to hashtable
    success = hput(htp, (void*)url, key, sizeof(key));
    *url_flag = true;
    return success;
}

//---------------------------- search_url -------------------------------
// Description:   compare two urls
// Inputs:        void pointer to the elements and void pointer 
//                to the key it should be compared to
// Outputs:       returns true if equal / false if different
//------------------------------------------------------------------------
bool search_url(void *elementp, const void* keyp) {
    char *url = (char*)elementp;
    char *key = (char*)keyp;

    if ( strcmp(url, key) == 0 ) {
        return true;
    }
    return false;
}


int main(){
    // Create new webpage, queue, and hash
    webpage_t *page = webpage_new(URL, DEPTH, NULL);
    queue_t *qp = qopen();
    queue_t *q_urlp = qopen();
    hashtable_t *htp = hopen(H_SIZE);

    // Checking that the fetch worked correctly
    if( !webpage_fetch( page ) ) {
        exit(EXIT_FAILURE);
    }

    // Get html code
    // char *html = webpage_getHTML(page);
    // printf("Found html: %s\n", html);

    char *result;
    int32_t success;

    bool url_flag = false;
    int iteration = 0;
    int pos = 0;
    // Scan the html code for urls
    while ( ( pos = webpage_getNextURL( page, pos, &result ) ) > 0 ) {
        // Add url to queue
        success = url_to_queue(q_urlp, result);
        if ( success != 0 ) {
            exit(EXIT_FAILURE);
        }

        // Check if the url is internal
        if ( IsInternalURL( result ) ) {
            // Check if the url is in the hashtable
            success = url_to_hash(htp, &search_url, result, (const char*)result, iteration, &url_flag);
            if ( success != 0 ) {
                exit(EXIT_FAILURE);
            }

            // If url is not in hashtable add webpage to the queue
            if ( url_flag ) {
                success = web_to_queue(qp, result, DEPTH, NULL);
                if ( success != 0 ) {
                    exit(EXIT_FAILURE);
                }
            }
        }
        // Update the iteration
        iteration += 1;
    }

    char *url = (char*)qget(q_urlp);
    // Loop thru the entire queue
    while ( url != NULL ) {
        free(url);
        url = (char*)qget(q_urlp);
    }
    free(url);

    // Print urls in the queue    
    print_web_queue(qp);

    // Free memory
    hclose(htp);
    qclose(qp);
    qclose(q_urlp);
    bag_delete(page);

    exit(EXIT_SUCCESS);
}
