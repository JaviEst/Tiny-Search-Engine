/* test_apply_list_empty.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thu Sep 30 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test apply for an empty list.
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
    lapply(&year);
    exit(EXIT_SUCCESS);
}