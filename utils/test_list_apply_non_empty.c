/* test_apply_list_non_empty.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thu Sep 30 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test apply for a non empty list.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"

void year(car_t *cp) {
    cp->year = cp->year + 1;
}

int main(void){
    // Add one car to the list
    car_t cp;
    strcpy(cp.plate, "BMW4");
    cp.year = 2022;

    // Add second car to the list
    car_t tp;
    strcpy(tp.plate, "ROADSTER");
    tp.year = 2030;

    lput(&cp);
    lput(&tp);

    int expected_year_cp = 2023;
    int expected_year_tp = 2031;

    // Apply function to list of cars
    lapply(&year);

    if( (cp.year != expected_year_cp) || (tp.year != expected_year_tp) ) {
        printf("Test Failed. Expected: %d & %d / Actual: %d & %d\n", 
                expected_year_cp, expected_year_tp, cp.year, tp.year);
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}