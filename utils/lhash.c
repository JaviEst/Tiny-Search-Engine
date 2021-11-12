 /* lhash.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Wed Nov 10 09:22:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Implements a generic locked hash table as an indexed set of queues.
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "hash.h"

typedef struct lhashtable {
    uint32_t size;
    hashtable_t *htp;
    pthread_mutex_t lock;
} lhashtable_t;

//---------------------------- lhopen ------------------------------
// Description:   creates a locked empty hash table
// Inputs:        size of locked hash table
// Outputs:       a locked hash table with initial size hsize
//------------------------------------------------------------------
lhashtable_t *lhopen(uint32_t hsize) {
  lhashtable_t *empty_lhash = malloc(sizeof(lhashtable_t));
  
  // Initialize mutex
  pthread_mutex_init(&empty_lhash->lock, NULL);
  empty_lhash->size = hsize;
  empty_lhash->htp = hopen(hsize);  

  return empty_lhash;
}

//---------------------------- lhclose -----------------------------
// Description:   closes a locked hash table
// Inputs:        a pointer to a locked hash table
// Outputs:       deallocates locked hash table and frees memory
//------------------------------------------------------------------
void lhclose(lhashtable_t *lhtp) {
  // Destroy the mutex
  pthread_mutex_destroy(&lhtp->lock);
  hclose(lhtp->htp);
  free(lhtp);
}

//---------------------------- lhput -------------------------------
// Description:   puts an entry into a locked hash table
// Inputs:        locked hash table pointer, element pointer, 
//                key pointer, and key length
// Outputs:       returns 0 if successful, nonzero otherwise
//------------------------------------------------------------------
int32_t lhput(lhashtable_t *lhtp, void *ep, const char *key, int keylen) {
  // Wait for ownership of locked hashtable
  pthread_mutex_lock(&lhtp->lock);
  int32_t success = hput(lhtp->htp, ep, key, keylen);

  // Give up ownership of locked hashtable
  pthread_mutex_unlock(&lhtp->lock);
  return success;
}

//---------------------------- lhapply -----------------------------
// Description:   apply a function to every entry in locked hash table
// Inputs:        locked hash table pointer and arbitrary function 
// Outputs:       apply a function to every entry in locked hash table
//------------------------------------------------------------------
void lhapply(lhashtable_t *lhtp, void (*fn)(void* ep)) {
  // Wait for ownership of locked hashtable
  pthread_mutex_lock(&lhtp->lock);
  happly(lhtp->htp, fn);

  // Give up ownership of locked hashtable
  pthread_mutex_unlock(&lhtp->lock);
}

//---------------------------- lhsearch --------------------------------
// Description:   search hash table using a supplied boolean function
// Inputs:        a search function, a hast table pointer, a key pointer
//                and a key length
// Outputs:       returns a pointer to the entry, or NULL if not found
//----------------------------------------------------------------------
void *lhsearch(lhashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, int32_t keylen) {
  // Wait for ownership of locked hashtable
  pthread_mutex_lock(&lhtp->lock);
  void *elementp = hsearch(lhtp->htp, searchfn, key, keylen);

  // Give up ownership of locked hashtable
  pthread_mutex_unlock(&lhtp->lock);
  return elementp;
}

//---------------------------- lhremove --------------------------------
// Description:   search locked hash table using supplied boolean 
//                function (as in qsearch), removes element from queue
// Inputs:        search function, locked hash table pointer, key pointer,
//                and key length
// Outputs:       returns a pointer to entry, or NULL if not found
//----------------------------------------------------------------------
void *lhremove(lhashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, int32_t keylen) {
  // Wait for ownership of locked hashtable
  pthread_mutex_lock(&lhtp->lock);
  void *elementp = hremove(lhtp->htp, searchfn, key, keylen);

  // Give up ownership of locked hashtable
  pthread_mutex_unlock(&lhtp->lock);
  return elementp;
}