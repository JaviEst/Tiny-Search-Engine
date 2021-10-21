 /* pageio.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thur Oct 21 10:33:29 2021 (-0400)
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
#include <webpage.h>

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

    return 0;
}