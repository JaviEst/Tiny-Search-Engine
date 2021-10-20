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
#include <errno.h>
#include <webpage.h>

// Global
char *URL = "https://thayer.github.io/engs50/";
char *PAGES_PATH = "../pages/";
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
    webpage_delete((void*)page);
}

//---------------------------- pagesave ----------------------------------
// Description:   saves a fetched page
// Inputs:        pointer to a webpage, filename designator, and
//                directory path/name
// Outputs:       content of id should have the following
//                    (1) URL page was fetched from 
//                    (2) depth assigned to webpage
//                    (3) length of HTML associated with page
//                    (4) HTML associated with page
//------------------------------------------------------------------------
int32_t pagesave(webpage_t *pagep, int id, char *dirname) {
    char *url = webpage_getURL(pagep);
    char *html = webpage_getHTML(pagep);
    int html_len = webpage_getHTMLlen(pagep);
    int depth = webpage_getDepth(pagep);
    // int i;

    // Create name of file
    char filepath[100] = {'\0'};
    sprintf(filepath, "%s%d", dirname, id);

    // Create a file and write to it 
    FILE *fp;
    fp = fopen(filepath, "w");

    // Check if file has been created/opened
    if ( fp == NULL ) {
        return 1;
    }

    fprintf(fp, "%s\n%d\n%d\n%s\n", url, depth, html_len, html);
    fclose(fp);

    // // Print first 50 characters of html
    // for (i=0;i<50; i++) {
    //     fputc(*(html+i), fp);
    // }

    return 0;
}

int main(){
    // Create new webpage, queue, and hash
    webpage_t *page = webpage_new(URL, DEPTH, NULL);

    // Checking that the fetch worked correctly
    if( !webpage_fetch( page ) ) {
        exit(EXIT_FAILURE);
    }

    // Get html code
    char *html = webpage_getHTML(page);
    fprintf(stdout, "Found html: %s\n", html);

    // Save the fetched webpage
    pagesave(page, 1, PAGES_PATH);

    // Free memory
    bag_delete(page);

    exit(EXIT_SUCCESS);
}
