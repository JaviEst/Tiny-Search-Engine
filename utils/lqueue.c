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
    queue_t *qp;
    pthread_mutex_t lock;
} lqueue_t;

//---------------------------- lqopen ------------------------------
// Description:   create a locked empty queue
// Inputs:        nothing
// Outputs:       returns pointer to a locked empty queue
//------------------------------------------------------------------
lqueue_t* lqopen(void) {
    lqueue_t *empty_lq = malloc(sizeof(lqueue_t));

    // Initialize mutex
    pthread_mutex_init(&empty_lq->lock, NULL);
    empty_lq->qp = qopen();
    return empty_lq;
}

//---------------------------- lqclose -----------------------------
// Description:   deallocates a locked queue
// Inputs:        locked queue pointer
// Outputs:       deallocate locked queue and frees memory
//------------------------------------------------------------------
void lqclose(lqueue_t *lqp) {
    // Destroy the mutex
    pthread_mutex_destroy(&lqp->lock);
    qclose(lqp->qp);
}

//---------------------------- lqput -------------------------------
// Description:   put element at end of locked queue
// Inputs:        locked queue pointer and element to add to queue
// Outputs:       returns 0 is successful, nonzero otherwise
//------------------------------------------------------------------
int32_t lqput(lqueue_t *lqp, void *elementp) {
    // Wait for ownership of locked queue
    pthread_mutex_lock(&lqp->lock);
    int32_t success = qput(lqp->qp, elementp);

    // Give up ownership of locked queue
    pthread_mutex_unlock(&lqp->lock);
    return success;
}

//---------------------------- lqget -------------------------------
// Description:   get first element from locked queue, 
//                removing it from the queue
// Inputs:        locked queue pointer 
// Outputs:       pointer to first element of locked queue
//                (remove this element from locked queue)
//------------------------------------------------------------------
void* lqget(lqueue_t *lqp) {    
    // Wait for ownership of locked queue
    pthread_mutex_lock(&lqp->lock);
    void *elementp = qget(lqp->qp);

    // Give up ownership of locked queue
    pthread_mutex_unlock(&lqp->lock);
    return elementp;
}

//---------------------------- lqapply ------------------------------
// Description:   apply a function to every element of locked queue
// Inputs:        function to apply to every element in locked queue,
//                locked queue pointer
// Outputs:       apply a function to every element of locked queue
//-------------------------------------------------------------------
void lqapply(lqueue_t *lqp, void (*fn)(void* elementp)) {
    // Wait for ownership of locked queue
    pthread_mutex_lock(&lqp->lock);
    qapply(lqp->qp, fn);

    // Give up ownership of locked queue
    pthread_mutex_unlock(&lqp->lock);
}

//---------------------------- lqsearch -------------------------------
// Description:   search locked queue using supplied boolean function
// Inputs:        a search function, locked queue pointer, and key
// Outputs:       returns a pointer to an element, or NULL if not found
//--------------------------------------------------------------------
void* lqsearch(lqueue_t *lqp, 
                bool (*searchfn)(void* elementp,const void* keyp),
				const void* skeyp) {
    // Wait for ownership of locked queue
    pthread_mutex_lock(&lqp->lock);
    void *elementp = qsearch(lqp->qp, searchfn, skeyp);

    // Give up ownership of locked queue
    pthread_mutex_unlock(&lqp->lock);
    return elementp;
}

//---------------------------- lqremove ------------------------------
// Description:   search locked queue using supplied boolean function 
//                (as in qsearch), removes element from locked queue
// Inputs:        a search function, locked queue pointer, and key
// Outputs:       returns a pointer to the element, or NULL if not found
//--------------------------------------------------------------------
void* lqremove(lqueue_t *lqp,
                bool (*searchfn)(void* elementp,const void* keyp),
                const void* skeyp) {
    // Wait for ownership of locked queue
    pthread_mutex_lock(&lqp->lock);
    void *elementp = qremove(lqp->qp, searchfn, skeyp);

    // Give up ownership of locked queue
    pthread_mutex_unlock(&lqp->lock);
    return elementp;
}

//---------------------------- lqconcat ------------------------------
// Description:   concatenates elements of locked q2 int locked q1
// Inputs:        pointers to both locked queues
// Outputs:       locked q2 is deallocated
//--------------------------------------------------------------------
void lqconcat(lqueue_t *lq1p, lqueue_t *lq2p) {
    // Wait for ownership of locked queue
    pthread_mutex_lock(&lq1p->lock);
    pthread_mutex_lock(&lq2p->lock);
    qconcat(lq1p->qp, lq2p->qp);

    // Give up ownership of locked queue
    pthread_mutex_unlock(&lq1p->lock);
    pthread_mutex_unlock(&lq2p->lock);

    // Destroy lock and free memory of second queue
    lqclose(lq2p);
}