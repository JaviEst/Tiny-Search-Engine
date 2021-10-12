/* test_remove_list_end.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thu Sep 30 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test remove for a non empty list.
 *              Where the plate is at the end of the list.
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

    car_t *sp = lremove("Sergio");

    if ( sp != &cp ) {
        printf("Test Failed. Expected: %s & %lf & %d / Actual: %s & %lf & %d\n",
                cp.plate, cp.price, cp.year, sp->plate, sp->price, sp->year);
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}