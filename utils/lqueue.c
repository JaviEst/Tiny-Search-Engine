/* lqueue.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Wed Nov 10 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Implements a locked queue.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"


typedef struct lqueue {
    queue_t qp;
    // mutex
} lqueue_t;


//---------------------------- lqopen ------------------------------
// Description:   create an empty queue
// Inputs:        nothing
// Outputs:       returns pointer to an empty queue
//------------------------------------------------------------------
lqueue_t* lqopen(void) {
    
}

//---------------------------- lqclose -----------------------------
// Description:   deallocate a queue, frees everything in it
// Inputs:        queue pointer
// Outputs:       deallocate queue, free memory
//------------------------------------------------------------------
void lqclose(lqueue_t *qp) {
    
}

//---------------------------- lqput -------------------------------
// Description:   put element at the end of the queue
// Inputs:        queue pointer and element to add to the queue
// Outputs:       returns 0 is successful; nonzero otherwise
//------------------------------------------------------------------
int32_t lqput(lqueue_t *qp, void *elementp) {
    
}

//---------------------------- lqget -------------------------------
// Description:   get the first element from queue, 
//                removing it from the queue
// Inputs:        queue pointer 
// Outputs:       pointer to first element of the queue
//                remove first element from the queue
//------------------------------------------------------------------
void* lqget(lqueue_t *qp) {    
    
}

//---------------------------- qlapply -------------------------------
// Description:   apply a function to every element of the queue
// Inputs:        function to apply to every element in the queue
//                queue pointer
// Outputs:       apply a function to every element of the queue
//-------------------------------------------------------------------
void lqapply(lqueue_t *qp, void (*fn)(void* elementp)) {
    
}

//---------------------------- lqsearch -------------------------------
// Description:   search a queue using a supplied boolean function
// Inputs:        a search function, queue pointer, and key
// Outputs:       returns a pointer to an element, or NULL if not found
//--------------------------------------------------------------------
void* lqsearch(lqueue_t *qp, 
                bool (*searchfn)(void* elementp,const void* keyp),
				const void* skeyp) {
    
}

//---------------------------- lqremove ------------------------------
// Description:   search a queue using a supplied boolean function 
//                (as in qsearch), removes the element from the queue
// Inputs:        a search function, queue pointer, and key
// Outputs:       returns a pointer to the element, or NULL if not found
//--------------------------------------------------------------------
void* lqremove(lqueue_t *qp,
                bool (*searchfn)(void* elementp,const void* keyp),
                const void* skeyp) {
    
}

//---------------------------- lqconcat ------------------------------
// Description:   concatenates elements of q2 into q1
// Inputs:        pointers to both queues
// Outputs:       q2 is deallocated
//--------------------------------------------------------------------
void lqconcat(lqueue_t *q1p, lqueue_t *q2p) {
    
}