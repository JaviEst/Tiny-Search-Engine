/* test_remove_list_start.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thu Sep 30 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test remove for a non empty list.
 *              Where the plate is at the start of the list.
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
    car_t cp;
    strcpy(cp.plate, "Sergio");
    cp.price = 100000.12;
    cp.year = 2022;

    // Add second car to the list
    car_t tp;
    strcpy(tp.plate, "Garth");
    tp.price = 70000;
    tp.year = 2030;

    // Add third car to the list
    car_t mp;
    strcpy(mp.plate, "Javier");
    mp.price = 10111.33;
    mp.year = 2010;

    lput(&cp);
    lput(&tp);
    lput(&mp);

    car_t *sp = lremove("Garth");

    // Use get to make sure the list only has Javi and Serg
    car_t *car1p = lget();
    car_t *car2p = lget();
    car_t *car3p = lget();

    if ( car1p != &mp ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    if ( car2p != &cp ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    if ( car3p != NULL ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    if ( sp != &tp ) {
        printf("Test Failed. Expected: %s & %lf & %d / Actual: %s & %lf & %d\n",
                tp.plate, tp.price, tp.year, sp->plate, sp->price, sp->year);
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}