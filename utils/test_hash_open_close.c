/* test_hash_open_close.c --- 
 * 
 * 
* Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Tue Oct 05 09:22:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test hopen and hclose.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "hash.h"


int main(void){
    hashtable_t *htp = hopen(100);

    if ( htp == NULL ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    hclose(htp);
    exit(EXIT_SUCCESS);
}