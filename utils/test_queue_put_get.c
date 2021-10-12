/* test_queue_put_get.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Sat Oct 02 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test qput and qget on an empty and non empty queue.
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

int main(void){
    queue_t *qp = qopen();
    
    person_t person1 = make_person("Ravi", 23, 101.1);
    person_t person2 = make_person("Serf", 22, 310.0);
    person_t person3 = make_person("Girth", 22, 405.8);

    person_t *get_emp = (person_t*)qget(qp);

    // Test get on an empty queue
    if ( get_emp != NULL ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

    // Test put on an empty queue
    // Add first person
    int32_t success = qput(qp, (void*)&person1);
    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

    // Test put on a non empty queue
    // Add second person
    success = qput(qp, (void*)&person2);
    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

    // Add third person
    success = qput(qp, (void*)&person3);
    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

    person_t *javi;
    person_t *serg;
    person_t *garth;

    // Test get on a non empty queue
    javi = (person_t*)qget(qp);
    if ( javi != &person1 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person1.name, person1.age, person1.rate,
                javi->name, javi->age, javi->rate);
        exit(EXIT_FAILURE);
    }

   serg = (person_t*)qget(qp);
    if ( serg != &person2 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person2.name, person2.age, person2.rate,
                serg->name, serg->age, serg->rate);
        exit(EXIT_FAILURE);
    }

    garth = (person_t*)qget(qp);
    if ( garth != &person3 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person3.name, person3.age, person3.rate,
                garth->name, garth->age, garth->rate);
        exit(EXIT_FAILURE);
    }

    qclose(qp);
    exit(EXIT_SUCCESS);
}