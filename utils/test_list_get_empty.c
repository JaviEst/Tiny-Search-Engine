/* test_get_list_empty.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thu Sep 30 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test get for an empty list.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"

int main(void){
    car_t *cp = lget();

    if ( cp != NULL ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}