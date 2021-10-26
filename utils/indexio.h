#pragma once
/* 
 * indexio.h --- saving and loading indexer files
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Tue Oct 26 10:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: ...
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <hash.h>

/*
 * indexsave -- save the hashtable entries to a file called indexnm
 *
 * returns: 0 for success; nonzero otherwise
 *
 * The suggested format for the file is:
 *
 *   <word> <docID1> <count1> <docID2> <count2> ....<docIDN> <countN> 
 *
 */
int32_t indexsave(hashtable_t *htp, char *filepath);

/* 
 * indexload -- loads the file called indexnm into a hashtable
 *
 * returns: non-NULL for success; NULL otherwise
 */
hashtable_t *indexload(char *filepath);
