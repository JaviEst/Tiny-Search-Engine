/* test_get_list_non_empty.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thu Sep 30 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test get for a non empty list.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"

int main(void){
    // Add one car to the list
    car_t *cp = malloc(sizeof(car_t));
    strcpy(cp->plate, "BMW4");
    cp->price = 69000.12;
    cp->year = 2022;

    lput(cp);
    free(cp);

    // Add second car to the list
    car_t *tp = malloc(sizeof(car_t));
    strcpy(tp->plate, "ROADSTER");
    tp->price = 77777.12;
    tp->year = 2030;

    lput(tp);

    // Get car at the beginning of the list
    car_t *carp = lget();

    if ( carp != tp ) {
        printf("Test Failed. Expected: %s & %lf & %d / Actual: %s & %lf & %d\n",
                tp->plate, tp->price, tp->year, carp->plate, carp->price, carp->year);
        free(tp);
        exit(EXIT_FAILURE);
    }
    free(tp);
    exit(EXIT_SUCCESS);
}