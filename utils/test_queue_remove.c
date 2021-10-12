/* test_queue_remove.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Sat Oct 02 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test qremove on an empty and non empty queues.
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

    // Test remove on an empty queue
    person_t *temp = (person_t*)qremove(qp, &search_age, (void*)&key);
    if ( temp != NULL ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    person_t person1 = make_person("Ravi", 21, 101.1);
    person_t person2 = make_person("Serf", 22, 310.0);
    person_t person3 = make_person("Girth", 23, 405.8);


    // Test remove on a queue with just one element
    // Add first person (element to be removed)
    int32_t success = qput(qp, (void*)&person1);
    if ( success != 0 ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    // Remove on a queue with just one element
    temp = (person_t*)qremove(qp, &search_age, (void*)&key);
    if ( temp != &person1 ) {
        printf("Test Failed.\n");
        printf("Expected: %d / Actual: %d\n",
                person1.age, temp->age);
        qclose(qp);
        exit(EXIT_FAILURE);
    }

    // Make sure the queue is empty after removing the element
    person_t *emp = (person_t*)qget(qp);
    if ( emp != NULL ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }
    
    // Test removing the last element from a multi element queue
    // Add first person
    success = qput(qp, (void*)&person2);
    if ( success != 0 ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    // Add second person
    success = qput(qp, (void*)&person3);
    if ( success != 0 ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    // Add third person (element to be removed)
    success = qput(qp, (void*)&person1);
    if ( success != 0 ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    // Remove last element of queue
    temp = (person_t*)qremove(qp, &search_age, (void*)&key);
    if ( temp != &person1 ) {
        printf("Test Failed.\n");
        printf("Expected: %d / Actual: %d\n",
                person1.age, temp->age);
        qclose(qp);
        exit(EXIT_FAILURE);
    }

    // Make sure the queue still has the other 2 elements
    person_t *serg = (person_t*)qget(qp);
    if ( serg != &person2 ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    person_t *garth = (person_t*)qget(qp);
    if ( garth != &person3 ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    emp = (person_t*)qget(qp);
    if ( emp != NULL ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }


    // Test removing a middle element from a multi element queue
    // Add first person
    success = qput(qp, (void*)&person2);
    if ( success != 0 ) {
        printf("Test Failed.\n");
        exit(EXIT_FAILURE);
    }

    // Add second person (element to be removed)
    success = qput(qp, (void*)&person1);
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

    // Remove middle element of queue
    temp = (person_t*)qremove(qp, &search_age, (void*)&key);
    if ( temp != &person1 ) {
        printf("Test Failed.13\n");
        printf("Expected: %d / Actual: %d\n",
                person1.age, temp->age);
        qclose(qp);
        exit(EXIT_FAILURE);
    }

    // Make sure the queue still has the other 2 elements
    serg = (person_t*)qget(qp);
    if ( serg != &person2 ) {
        printf("Test Failed.14\n");
        exit(EXIT_FAILURE);
    }

    garth = (person_t*)qget(qp);
    if ( garth != &person3 ) {
        printf("Test Failed.15\n");
        exit(EXIT_FAILURE);
    }

    emp = (person_t*)qget(qp);
    if ( emp != NULL ) {
        printf("Test Failed.16\n");
        exit(EXIT_FAILURE);
    }
    qclose(qp);
    exit(EXIT_SUCCESS);
}