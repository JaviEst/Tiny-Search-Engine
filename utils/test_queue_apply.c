/* test_queue_apply.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Sat Oct 02 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test qapply on an empty and non empty queues.
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

void age(void *element) {
    person_t *cp = (person_t*)element;
    cp->age = cp->age + 1;
}

int main(void){
    queue_t *qp = qopen();

    qapply(qp, &age);
    
    person_t person1 = make_person("Ravi", 1, 101.1);
    person_t person2 = make_person("Serf", 2, 310.0);
    person_t person3 = make_person("Girth", 3, 405.8);

    // Add first person
    int32_t success = qput(qp, (void*)&person1);
    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

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

    // Expected ages
    int expected_javi_age = person1.age + 1;
    int expected_serg_age = person2.age + 1;
    int expected_garth_age = person3.age + 1;

    // Test qapply on a non empty function
    qapply(qp, &age);

    person_t *javi;
    person_t *serg;
    person_t *garth;

    // Test apply on a non empty queue
    javi = (person_t*)qget(qp);
    if ( javi->age != expected_javi_age ) {
        printf("Test Failed.\n");
        printf("Expected: %d / Actual: %d\n",
                expected_javi_age, javi->age);
        exit(EXIT_FAILURE);
    }

    serg = (person_t*)qget(qp);
    if ( serg->age != expected_serg_age ) {
        printf("Test Failed.\n");
        printf("Expected: %d / Actual: %d\n",
                expected_serg_age, serg->age);
        exit(EXIT_FAILURE);
    }

    garth = (person_t*)qget(qp);
    if ( garth->age != expected_garth_age ) {
        printf("Test Failed.\n");
        printf("Expected: %d / Actual: %d\n",
                expected_garth_age, garth->age);
        exit(EXIT_FAILURE);
    }

    qclose(qp);
    exit(EXIT_SUCCESS);
}