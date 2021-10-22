/* test_pageio_load.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thur Oct 21 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Tests pageload.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <pageio.h>
#include <webpage.h>


int main(void){
    int id = 1;
    char *orig_dirname = "../pages/";

    // Test pageload 
    webpage_t *orig_page = pageload(id, orig_dirname);
    if ( orig_page == NULL ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    char *orig_html = webpage_getHTML(orig_page);

    // Save the page just loaded in a different location
    char *dirname = "../indexer/";
    int32_t success = pagesave(orig_page, id, dirname);
    if ( success != 0 ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    // Load the saved page
    webpage_t *page = pageload(id, dirname);
    if ( page == NULL ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    char *html = webpage_getHTML(page);

    // Test both html are the same
    if ( strcmp(orig_html, html) != 0 ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }
    webpage_delete(page);
    webpage_delete(orig_page);
    exit(EXIT_SUCCESS);
}