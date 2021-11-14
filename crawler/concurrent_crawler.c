 /* concurrent_crawler.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Sun Nov 14 10:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Implements a parallelized crawler ...
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
#include <pthread.h>
#include <time.h>
#include <webpage.h>
#include <lqueue.h>
#include <lhash.h>
#include <pageio.h>

// typedef struct id_page {
//     int id;
//     pthread_mutex_t lock;
// } id_t;

// typedef struct pos_page {
//     int pos;
//     pthread_mutex_t lock;
// } pos_t;

// typedef struct page_depth {
//     int depth;
//     pthread_mutex_t lock;
// } page_depth_t;

// typedef struct next_depth {
//     int depth;
//     pthread_mutex_t lock;
// } next_depth_t;

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
int web_to_queue(lqueue_t* lqp, char* url, int depth, char* html) {
    webpage_t *internal_page= webpage_new(url, depth, html);
    int32_t success = lqput(lqp, (void*)internal_page);
    return success;
}

//---------------------------- url_to_queue ------------------------------
// Description:   add a url to a queue
// Inputs:        queue, pointer to a character (string) 
// Outputs:       queue is populated with webpage
//------------------------------------------------------------------------
int url_to_queue(lqueue_t* lqp, char* url) {
    int32_t success = lqput(lqp, (void*)url);
    return success;
}

//---------------------------- print_web_queue ---------------------------
// Description:   print a queue
// Inputs:        pointer to a queue
// Outputs:       prints a queue
//------------------------------------------------------------------------
void print_web_queue(lqueue_t *lqp) {
    webpage_t *page = (webpage_t*)lqget(lqp);
    char *url = webpage_getURL(page);
    int page_depth = webpage_getDepth(page);
    // Loop thru the entire queue
    while ( page != NULL ) {
        logr("Internal", page_depth, url);
        bag_delete(page);
        page = (webpage_t*)lqget(lqp);
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
int32_t url_to_hash(lhashtable_t* lhtp,
                bool (*searchfn)(void* elementp, const void* searchkeyp),
                char* url, const char* key, 
                bool* url_flag) {
    int32_t success;

    // Check if the url is in the hashtable
    char* check = (char*)lhsearch(lhtp, searchfn, key, sizeof(key));
    if ( check != NULL ) {
        // If url found, then return false
        *url_flag = false;
        return 0;
    }

    // Add unique url to hashtable
    success = lhput(lhtp, (void*)url, key, sizeof(key));
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
// Description:   checking the inputs from the user are correct
// Inputs:        argv / argc
// Outputs:       
//------------------------------------------------------------------------
int32_t check_arg(int argc, char** argv){
    // Check the number of input arguments is correct
    if ( argc != 5 ) {
        printf("usage: crawler <seedurl> <pagedir> <maxdepth> <threadcount>\n");
        return 1;
    }

    // Check if pagedir exists
    DIR *dirp = opendir(argv[2]);
    if ( dirp == NULL ) {
        printf("usage: crawler <seedurl> <pagedir> <maxdepth> <threadcount>\n");
        printf("<pagedir> must exist\n");
        return 1;
    }
    closedir(dirp);

    // Check if maxdepth is equal or greater than 0
    long maxdepth = strtoul(argv[3], NULL, 0);
    if ( maxdepth < 0 ) {
        printf("usage: crawler <seedurl> <pagedir> <maxdepth> <threadcount>\n");
        printf("<maxdepth> must be equal to or greater than 0\n");
        return 1;
    }

    // Check if threadcount is greater than 0
    long threadcount = strtoul(argv[4], NULL, 0);
    if ( threadcount < 1 ) {
        printf("usage: crawler <seedurl> <pagedir> <maxdepth> <threadcount>\n");
        printf("<threadcount> must be equal to or greater than 1\n");
        return 1;
    }

    return 0;
}

// Declare locked data structures and locked variables
lqueue_t *web_lqueue;
lqueue_t *url_lqueue;
lhashtable_t *url_lhash;

int next_depth = 0;
pthread_mutex_t next_depth_lock;

int page_depth = 0;
pthread_mutex_t page_depth_lock;

int id = 1;
pthread_mutex_t id_lock;

int pos = 0;
pthread_mutex_t pos_lock;

// next_depth_t next;
// next.depth = 0;
// pthread_mutex_init(&(next.lock), NULL);

// page_depth_t curr_page;
// curr_page.depth = 0;
// pthread_mutex_init(&(curr_page.lock), NULL);

// id_t id;
// id.id = 1;
// pthread_mutex_init(&(id.lock), NULL);

// pos_t pos;
// pos.pos = 0;
// pthread_mutex_init(&(pos.lock), NULL);

long max_depth;
char *page_dir;

//------------------------------- crawl ---------------------------------
// Description:   
// Inputs:        
// Outputs:       
//------------------------------------------------------------------------
void* crawl(void *arg) {
    int32_t success;
    webpage_t *page;
    bool url_flag;
    char *result;                   // scanned url
    // Go thru all urls until max depth
    while ( page_depth <= max_depth && ( page = (webpage_t*)lqget(web_lqueue) ) != NULL ) {        
        // Assign the right depth
        pthread_mutex_lock(&page_depth_lock);
        page_depth = webpage_getDepth(page);
        pthread_mutex_unlock(&page_depth_lock);

        pthread_mutex_lock(&next_depth_lock);
        next_depth = page_depth + 1;
        pthread_mutex_unlock(&next_depth_lock);

        if ( page_depth > max_depth ) {
            break;
        }

        // Checking that the fetch worked correctly
        while( !webpage_fetch( page ) ) {
            bag_delete(page);
            // Grab front page in the queue
            page = (webpage_t*)lqget(web_lqueue);

            // Assign the right depth
            pthread_mutex_lock(&page_depth_lock);
            page_depth = webpage_getDepth(page);
            pthread_mutex_unlock(&page_depth_lock);

            pthread_mutex_lock(&next_depth_lock);
            next_depth = page_depth + 1;
            pthread_mutex_unlock(&next_depth_lock);
        }

        // Save the fetched webpage
        pagesave(page, id, page_dir);

        if ( next_depth <= max_depth ) {
            // Crawl the html code for urls
            pthread_mutex_lock(&pos_lock);
            while ( ( pos = webpage_getNextURL( page, pos, &result ) ) > 0 ) {
                pthread_mutex_unlock(&pos_lock);
                // Add url to queue
                success = url_to_queue(url_lqueue, result);
                if ( success != 0 ) {
                    printf("Second exit.\n");
                    exit(EXIT_FAILURE);
                }

                // Check if the url is internal
                if ( IsInternalURL( result ) ) {
                    // Check if the url is in the hashtable
                    success = url_to_hash(url_lhash, &search_url, result, (const char*)result, &url_flag);
                    if ( success != 0 ) {
                        printf("Third exit.\n");
                        exit(EXIT_FAILURE);
                    }

                    // If url is not in hashtable add webpage to the queue
                    if ( url_flag ) {
                        success = web_to_queue(web_lqueue, result, next_depth, NULL);
                        if ( success != 0 ) {
                            printf("Fourth exit.\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            }
        }
        bag_delete(page);
        pthread_mutex_lock(&id_lock);
        id += 1;
        pthread_mutex_unlock(&id_lock);

        pthread_mutex_lock(&pos_lock);
        pos = 0;
        pthread_mutex_unlock(&pos_lock);
    }
    bag_delete(page);

    int ret = 0;
    pthread_exit(&ret);
}

int main(int argc, char** argv){
    clock_t start, end;
    start = clock();
    // Check the inputs from the user
    int32_t correct_arg = check_arg(argc, argv);
    if ( correct_arg != 0 ){
        exit(EXIT_FAILURE);
    }

    // Assign arguments from command line
    char *seed_url = argv[1];
    page_dir = argv[2];
    max_depth = strtoul(argv[3], NULL, 0);
    long threads = strtoul(argv[4], NULL, 0);

    pthread_mutex_init(&next_depth_lock, NULL);
    pthread_mutex_init(&page_depth_lock, NULL);
    pthread_mutex_init(&id_lock, NULL);
    pthread_mutex_init(&pos_lock, NULL);
    
    pthread_t threads_arr[threads];

    // Initialize lqueues and lhash
    web_lqueue = lqopen();
    url_lqueue = lqopen();
    url_lhash = lhopen(H_SIZE);


    bool url_flag = false;          // check for url duplicate
    // Add url to the hashtable
    int32_t success = url_to_hash(url_lhash, &search_url, seed_url, (const char*)seed_url, &url_flag);
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    // Add webpage to the queue
    success = web_to_queue(web_lqueue, seed_url, next_depth, NULL);
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    // Create threads
    int i;
    for ( i = 0; i < threads; i++) {
        pthread_create(&(threads_arr[i]), NULL, crawl, NULL);
    }

    // Join threads
    for ( i = 0; i < threads; i++) {
        pthread_join(threads_arr[i], NULL);
    }

    char *url = (char*)lqget(url_lqueue);
    // Loop thru the entire queue to free the url memory
    while ( url != NULL ) {
        free(url);
        url = (char*)lqget(url_lqueue);
    }
    free(url);

    // Print urls in the queue    
    print_web_queue(web_lqueue);

    pthread_mutex_destroy(&next_depth_lock);
    pthread_mutex_destroy(&page_depth_lock);
    pthread_mutex_destroy(&id_lock);
    pthread_mutex_destroy(&pos_lock);

    // Free memory
    lhclose(url_lhash);
    lqclose(web_lqueue);
    lqclose(url_lqueue);

    end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nRUN TIME: %lf\n", cpu_time_used);


    exit(EXIT_SUCCESS);
}
