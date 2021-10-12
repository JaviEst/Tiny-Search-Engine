/* test_put_list_empty.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thu Sep 30 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test put for an empty list.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"

int main(void){
    car_t *cp = malloc(sizeof(car_t));
    strcpy(cp->plate, "BMW4");
    cp->price = 69000.12;
    cp->year = 2022;

    int success = lput(cp);
    free(cp);

    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}