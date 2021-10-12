 /* hash.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Tue Oct 05 09:22:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Implements a generic hash table as an indexed set of queues.
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "queue.h"

typedef struct hashtable {
    uint32_t size;
    queue_t **table;
} hashtable_t;

/* 
 * SuperFastHash() -- produces a number between 0 and the tablesize-1.
 * 
 * The following (rather complicated) code, has been taken from Paul
 * Hsieh's website under the terms of the BSD license. It's a hash
 * function used all over the place nowadays, including Google Sparse
 * Hash.
 */
#define get16bits(d) (*((const uint16_t *) (d)))

static uint32_t SuperFastHash (const char *data,int len,uint32_t tablesize) {
  uint32_t hash = len, tmp;
  int rem;
  
  if (len <= 0 || data == NULL)
		return 0;
  rem = len & 3;
  len >>= 2;
  /* Main loop */
  for (;len > 0; len--) {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2*sizeof (uint16_t);
    hash  += hash >> 11;
  }
  /* Handle end cases */
  switch (rem) {
  case 3: hash += get16bits (data);
    hash ^= hash << 16;
    hash ^= data[sizeof (uint16_t)] << 18;
    hash += hash >> 11;
    break;
  case 2: hash += get16bits (data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1: hash += *data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }
  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;
  return hash % tablesize;
}

//---------------------------- hopen -------------------------------
// Description:   creates an empty hash table
// Inputs:        size of hash table
// Outputs:       a hash table with initial size hsize
//------------------------------------------------------------------
hashtable_t *hopen(uint32_t hsize) {
  // First allocate memory for hash table data structure
  hashtable_t *empty_hash = malloc(sizeof(hashtable_t));
  // Then allocate memory for table (queue)
  empty_hash->table = (queue_t**)malloc(sizeof(queue_t*)*hsize);
  empty_hash->size = hsize;
  
  // Create a queue at each index of the empty hash table
  for (int i=0; i<hsize; i++) {
    empty_hash->table[i] = qopen();
  }
  return empty_hash;
}

//---------------------------- hclose ------------------------------
// Description:   closes a hash table
// Inputs:        a pointer to a hash table
// Outputs:       deallocate hash table, free memory
//------------------------------------------------------------------
void hclose(hashtable_t *htp) {
  // Close queue at each index of the given hash table
  for (int i=0; i<htp->size; i++) {
    queue_t* queue = htp->table[i];
    qclose(queue);
  }

  // First deallocate memory for table (queue)
  free(htp->table);
  // Then deallocate memory for hash table data structure
  free(htp);
}

//---------------------------- hput --------------------------------
// Description:   puts an entry into a hash table
// Inputs:        hash table pointer, element pointer, key pointer
//                and key length
// Outputs:       returns 0 if successful; nonzero otherwise
//------------------------------------------------------------------
int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen) {
  // Use SuperFastHash to generate table index
  uint32_t index = SuperFastHash(key, keylen, htp->size);

  // Check for index out of bounds
  if (index < 0 || index >= htp->size ) {
    printf("Index out of bounds.\n");
    return 1;
  }

  // Input data at generated index
  int32_t qsuccess = qput(htp->table[index], ep);
  return qsuccess;
}

//---------------------------- happly ------------------------------
// Description:   apply a function to every entry in hash table
// Inputs:        hash table pointer and an arbitrary function 
// Outputs:       apply a function to every entry in hash table
//------------------------------------------------------------------
void happly(hashtable_t *htp, void (*fn)(void* ep)) {
  // Loop through the hash table
  for ( int i=0; i<htp->size; i++ ) {
    qapply(htp->table[i], fn);
  } 
}

//---------------------------- hsearch ---------------------------------
// Description:   search a hash table using a supplied boolean function
// Inputs:        a search function, a hast table pointer, a key pointer
//                and a key length
// Outputs:       returns a pointer to the entry, or NULL if not found
//----------------------------------------------------------------------
void *hsearch(hashtable_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, int32_t keylen) {
  // Find index of the given key
  uint32_t index = SuperFastHash(key, keylen, htp->size);

  queue_t* qp = htp->table[index];
  void* element = qsearch(qp, searchfn, key);
  return element;
}

//---------------------------- hremove ---------------------------------
// Description:   search a hash table using a supplied boolean function
//                (as in qsearch), removes the element from the queue
// Inputs:        a search function, a hast table pointer, a key pointer
//                and a key length
// Outputs:       returns a pointer to the entry, or NULL if not found
//----------------------------------------------------------------------
void *hremove(hashtable_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, int32_t keylen) {
  // Find index of the given key
  uint32_t index = SuperFastHash(key, keylen, htp->size);

  queue_t* qp = htp->table[index];
  void* element = qremove(qp, searchfn, key);
  return element;
}