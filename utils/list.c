/* list.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thu Sep 30 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Implements a linked list of cars.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"

static car_t *front = NULL;

//---------------------------- lput -------------------------------
// Description:   place a car at the beginning of the list
// Inputs:        pointer to a car (pointer can be NULL)
// Outputs:       returns 0 if successful; nonzero otherwise
//-----------------------------------------------------------------
int32_t lput(car_t *cp) {
    // If given car is NULL exit unsuccessfully
    if ( cp == NULL ) {
        return 1;
    }

    // If empty list point to the given car 
    if ( front == NULL ) {
        front = cp;
        cp->next = NULL;
        return 0;
    }

    // If non-empty list... 
    cp->next = front;       // new car points to previous
    front = cp;             // front points to new car
    return 0;
}



//---------------------------- lget -------------------------------
// Description:   remove and return the first car in the list
// Inputs:        takes no arguments
// Outputs:       return NULL if the list is empty, return the
//                first car in the list
//-----------------------------------------------------------------
car_t *lget() {
	// If empty list return NULL
	if ( front == NULL ) {
		return NULL;
	}

	// Remove and return the first car
	car_t *front_tmp = front;
	front = front->next;
	return front_tmp;
}



//---------------------------- lapply -----------------------------
// Description:   apply a function to every car in the list
// Inputs:        ...
// Outputs:       ...
//-----------------------------------------------------------------
void lapply(void (*fn)(car_t *cp)) {
    // If empty list return NULL
    if ( front == NULL ) {
        return;
    }

    car_t *p;
    // Loop thru the entire list
    for ( p=front; p!=NULL; p=p->next ) {
        fn(p);
    }
}



//---------------------------- lremove ----------------------------
// Description:   find, remove, and return any car with designated plate
// Inputs:        pointer to a plate (pointer can be NULL)
// Outputs:       return NULL if not present
//-----------------------------------------------------------------
car_t *lremove(char *platep) {
    // If empty list return NULL
    if ( front == NULL ) {
        return NULL;
    }

    car_t *cp;      // current car
    car_t *pp;      // previous car

    // If the given plate corresponds to the first car in the list
    // call lget to get and remove the car
    if ( strcmp(front->plate, platep) == 0 ) {
        cp = lget();
    }
    else {
        pp = front;
        // Loop thru the list starting at the second car
        for ( cp=front->next; cp!=NULL; cp=cp->next ) {
            // If the plate matches the given plate
            if ( strcmp(cp->plate, platep) == 0 ) {
                // Remove and return the car from the list
                pp->next = cp->next;
                break;
            }
        }
    }
    return cp;
}
