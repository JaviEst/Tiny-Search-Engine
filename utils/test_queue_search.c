/* test_queue_search.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Sat Oct 02 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test qsearch on an empty and non empty queues.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "queue.h"

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

bool search_age(void *elementp, const void* keyp) {
    person_t *person1 = (person_t*)elementp;
    person_t *key = (person_t*)keyp;

    if ( person1->age == key->age ) {
        return true;
    }
    return false;
}

int main(void){
    queue_t *qp = qopen();

    const person_t key = make_person("Stone", 21, 0.0);

    // Test search on an empty queue
    person_t *temp = (person_t*)qsearch(qp, &search_age, (void*)&key);
    if ( temp != NULL ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    person_t person1 = make_person("Ravi", 20, 101.1);
    person_t person2 = make_person("Serf", 21, 310.0);
    person_t person3 = make_person("Girth", 22, 405.8);

    // Add first person
    int32_t success = qput(qp, (void*)&person1);
    if ( success != 0 ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    // Add second person
    success = qput(qp, (void*)&person2);
    if ( success != 0 ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    // Add third person
    success = qput(qp, (void*)&person3);
    if ( success != 0 ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    person_t *serg = &person2;

    // Test on a non empty queue
    person_t *check = (person_t*)qsearch(qp, &search_age, (void*)&key);
    if ( check != serg ) {
        printf("Test Failed.\n");
        printf("Expected: %d / Actual: %d\n",
                serg->age, check->age);
        qclose(qp);
        exit(EXIT_FAILURE);
    }
    qclose(qp);
    exit(EXIT_SUCCESS);
}