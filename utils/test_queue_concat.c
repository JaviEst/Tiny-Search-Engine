/* test_queue_concat.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Sat Oct 02 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test qconcat on empty and non empty queues.
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
    queue_t *q1p = qopen();
    queue_t *q2p = qopen();

    person_t person1 = make_person("Ravi", 23, 101.1);
    person_t person2 = make_person("Serf", 22, 310.0);
    person_t person3 = make_person("Girth", 22, 405.8);

    // Test concat 2 non empty queues
    // Add first person
    int32_t success = qput(q1p, (void*)&person1);
    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

    // Add second person
    success = qput(q1p, (void*)&person2);
    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

    // Add third person
    success = qput(q2p, (void*)&person3);
    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

    qconcat(q1p, q2p);

    person_t *javi;
    person_t *serg;
    person_t *garth;

    // Test get after concat
    javi = (person_t*)qget(q1p);
    if ( javi != &person1 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person1.name, person1.age, person1.rate,
                javi->name, javi->age, javi->rate);
        exit(EXIT_FAILURE);
    }

   serg = (person_t*)qget(q1p);
    if ( serg != &person2 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person2.name, person2.age, person2.rate,
                serg->name, serg->age, serg->rate);
        exit(EXIT_FAILURE);
    }

    garth = (person_t*)qget(q1p);
    if ( garth != &person3 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person3.name, person3.age, person3.rate,
                garth->name, garth->age, garth->rate);
        exit(EXIT_FAILURE);
    }

    qclose(q1p);

    queue_t *q3p = qopen();
    queue_t *q4p = qopen();

    // Test concat 1 non empty queue into an empty queue
    // Add first person
    success = qput(q4p, (void*)&person1);
    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

    // Add second person
    success = qput(q4p, (void*)&person2);
    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

    qconcat(q3p, q4p);

    // Test get after concat
    javi = (person_t*)qget(q3p);
    if ( javi != &person1 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person1.name, person1.age, person1.rate,
                javi->name, javi->age, javi->rate);
        exit(EXIT_FAILURE);
    }

   serg = (person_t*)qget(q3p);
    if ( serg != &person2 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person2.name, person2.age, person2.rate,
                serg->name, serg->age, serg->rate);
        exit(EXIT_FAILURE);
    }

    qclose(q3p);
    exit(EXIT_SUCCESS);
}