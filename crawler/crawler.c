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
#include <unistd.h>
#include <dirent.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>
#include <pageio.h>

// Global
uint32_t H_SIZE = 10000;
// URL = https://thayer.github.io/engs50/ 

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
    int page_depth = webpage_getDepth(page);
    // Loop thru the entire queue
    while ( page != NULL ) {
        logr("Internal", page_depth, url);
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

//---------------------------- check_arg ---------------------------------
// Description:   checking the ionputs from the user are correct
// Inputs:        argv / argc
// Outputs:       
//------------------------------------------------------------------------

int32_t check_arg(int argc, char** argv){
    // Check the number of input arguments is correct
    if ( argc != 4 ) {
        printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
        return 1;
    }

    // Check if pagedir exists
    DIR *dirp = opendir(argv[2]);
    if ( dirp == NULL ) {
        printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
        printf("<pagedir> must exist\n");
        return 1;
    }
    closedir(dirp);

    // Check if maxdepth is equal or greater than 0
    long maxdepth = strtoul(argv[3], NULL, 0);
    if ( maxdepth < 0 ) {
        printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
        printf("<maxdepth> must be equal to or greater than 0\n");
        return 1;
    }
    return 0;
}

int main(int argc, char** argv){
    // Check the inputs from the user
    int32_t correct_arg = check_arg(argc, argv);
    if ( correct_arg != 0 ){
        exit(EXIT_FAILURE);
    }

    // Assign arguments from command line
    char *seed_url = argv[1];
    char *page_dir = argv[2];
    long max_depth = strtoul(argv[3], NULL, 0);

    long depth = 0;

    // Create queues and hash
    queue_t *qp = qopen();
    queue_t *q_urlp = qopen();
    hashtable_t *htp = hopen(H_SIZE);

    bool url_flag = false;          // check for url duplicate
    // Add url to the hashtable
    int32_t success = url_to_hash(htp, &search_url, seed_url, (const char*)seed_url, 0, &url_flag);
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    // Add webpage to the queue
    success = web_to_queue(qp, seed_url, depth, NULL);
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    webpage_t *page;
    char *result;                   // scanned url
    char *url;
    int first_url = 1;              // check for first entry in hashtable
    int id = 1;                     // id in the page saved (name of the file)
    int pos = 0;
    int page_depth = 0;
    
    // Go thru all urls until max depth
    while ( page_depth <= max_depth && ( page = (webpage_t*)qget(qp) ) != NULL ) {        
        // Assign the right depth
        page_depth = webpage_getDepth(page);
        depth = page_depth + 1;

        if ( page_depth > max_depth ) {
            break;
        }

        // Checking that the fetch worked correctly
        while( !webpage_fetch( page ) ) {
            bag_delete(page);
            // Grab front page in the queue
            page = (webpage_t*)qget(qp);

            // Assign the right depth
            page_depth = webpage_getDepth(page);
            depth = page_depth + 1;
        }

        // Save the fetched webpage
        pagesave(page, id, page_dir);

        if ( depth <= max_depth ) {
            // Crawl the html code for urls
            while ( ( pos = webpage_getNextURL( page, pos, &result ) ) > 0 ) {
                // Add url to queue
                success = url_to_queue(q_urlp, result);
                if ( success != 0 ) {
                    printf("Second exit.\n");
                    exit(EXIT_FAILURE);
                }

                // Check if the url is internal
                if ( IsInternalURL( result ) ) {
                    // Check if the url is in the hashtable
                    success = url_to_hash(htp, &search_url, result, (const char*)result, first_url, &url_flag);
                    if ( success != 0 ) {
                        printf("Third exit.\n");
                        exit(EXIT_FAILURE);
                    }

                    // If url is not in hashtable add webpage to the queue
                    if ( url_flag ) {
                        success = web_to_queue(qp, result, depth, NULL);
                        if ( success != 0 ) {
                            printf("Fourth exit.\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
                first_url += 1;
            }
        }
        bag_delete(page);
        id += 1;
        pos = 0;
    }
    bag_delete(page);

    url = (char*)qget(q_urlp);
    // Loop thru the entire queue to free the url memory
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

    exit(EXIT_SUCCESS);
}
