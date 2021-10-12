/* test_hash_put.c --- 
 * 
 * 
* Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Tue Oct 05 09:22:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test hput:
 *                  - Test put one / hashing by name (no collision)
 *                  - Test put three / hashing by name (three collisions)
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "hash.h"

#define MAX 100

typedef struct person {
    char name[MAX];
    int age;
    double rate;
} person_t;

person_t make_person(const char *name, int age, double rate) {
    person_t p;
    strcpy(p.name, name);
    p.age = age;
    p.rate = rate;
    return p;
}

// Test without collisions: two keys, two hash indexes
int test_put_one(hashtable_t *htp){
    person_t person1 = make_person("Ravi", 23, 101.1);
    person_t person2 = make_person("Serf", 22, 310.0);

    int32_t result = hput(htp, (void*)&person1, "Ravi", sizeof("Ravi"));
    if ( result != 0 ) {
        return 1;
    }

    result = hput(htp, (void*)&person2, "Serf", sizeof("Serf"));
    if ( result != 0 ) {
        return 1;
    }
    return 0;
}

// Test with collisions: three identical keys one hash index
int test_put_three(hashtable_t *htp){
    person_t person1 = make_person("Girth", 23, 101.1);
    person_t person2 = make_person("Girth", 22, 310.0);
    person_t person3 = make_person("Girth", 22, 405.8);

    int32_t result = hput(htp, (void*)&person1, "Girth", sizeof("Girth"));
    if ( result != 0 ) {
        return 1;
    }

    result = hput(htp, (void*)&person2, "Girth", sizeof("Girth"));
    if ( result != 0 ) {
        return 1;
    }

    result = hput(htp, (void*)&person3, "Girth", sizeof("Girth"));
    if ( result != 0 ) {
        return 1;
    }

    return 0;
}

int main(void){
    hashtable_t *htp1 = hopen(10);
    hashtable_t *htp2 = hopen(50);
    
    // Testing putting one element into the hashtable
    int success = test_put_one(htp1);
    if ( success != 0 ) {
        printf("Test put one person failed.\n");
        exit(EXIT_FAILURE);
    }

    // Testing putting three element into the hashtable
    success = test_put_three(htp2);
    if ( success != 0 ) {
        printf("Test put three people failed.\n");
        exit(EXIT_FAILURE);
    }

    hclose(htp1);
    hclose(htp2);

    exit(EXIT_SUCCESS);
}