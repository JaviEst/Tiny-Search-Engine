/* test_lhash_open_close.c --- 
 * 
 * 
* Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thur Nov 11 09:22:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test lhopen and lhclose.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "lhash.h"


int main(void){
    lhashtable_t *lhtp = lhopen(100);

    if ( lhtp == NULL ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    lhclose(lhtp);
    exit(EXIT_SUCCESS);
}