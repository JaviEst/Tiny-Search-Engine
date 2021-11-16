/* test_lqueue_apply.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Fri Nov 12 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test lqapply with a single thread and multiple threads.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "lqueue.h"

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

void name(void *element) {
    person_t *cp = (person_t*)element;
    strcpy(cp->name, "test");
}

void age(void *element) {
    person_t *cp = (person_t*)element;
    cp->age = cp->age + 1;
}

void rate(void *element) {
    person_t *cp = (person_t*)element;
    cp->rate = cp->rate + 1;
}

//----------------------------- test_single_thread --------------------------------
// Description:   tests the lqapply method on the locked queue with a single thread
// Inputs:        none
// Outputs:       returns 0 if successful, nonzero otherwise
//---------------------------------------------------------------------------------
int test_single_thread() {
    lqueue_t *lqp = lqopen();

    lqapply(lqp, &age);
    
    person_t person1 = make_person("Ravi", 23, 101.1);
    person_t person2 = make_person("Serf", 22, 310.0);
    person_t person3 = make_person("Girth", 22, 405.8);

    // Test put on an empty queue
    // Add first person
    int32_t success = lqput(lqp, (void*)&person1);
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    // Test put on a non empty queue
    // Add second person
    success = lqput(lqp, (void*)&person2);
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    // Add third person
    success = lqput(lqp, (void*)&person3);
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    // Expected ages
    int expected_javi_age = person1.age + 1;
    int expected_serg_age = person2.age + 1;
    int expected_garth_age = person3.age + 1;

     // Test qapply on a non empty queue
    lqapply(lqp, &age); 

    person_t *javi;
    person_t *serg;
    person_t *garth;

    // Test get on a non empty queue
    javi = (person_t*)lqget(lqp);
    if ( javi->age != expected_javi_age ) {
        printf("Test Failed.\n");
        printf("Expected: %d / Actual: %d\n",
                expected_javi_age, javi->age);
        return -1;
    }

   serg = (person_t*)lqget(lqp);
    if ( serg->age != expected_serg_age ) {
        printf("Test Failed.\n");
        printf("Expected: %d / Actual: %d\n",
                expected_serg_age, serg->age);
        return -1;
    }

    garth = (person_t*)lqget(lqp);
    if ( garth->age != expected_garth_age ) {
        printf("Test Failed.\n");
        printf("Expected: %d / Actual: %d\n",
                expected_garth_age, garth->age);
        return -1;
    }

    lqclose(lqp);
    return 0;
}

// Global locked queue for the multiple threads test
lqueue_t *global_lqp;

void *test_apply(void *arg) {
    int ret1;

    // Apply three different functions to locked queue
    lqapply(global_lqp, &name);
    lqapply(global_lqp, &age);
    lqapply(global_lqp, &rate);

    ret1 = 0;
    pthread_exit(&ret1);
}

//------------------------- test_multiple_threads_lqapply ---------------------------
// Description:   tests the lqapply method on the locked queue with multiple threads
// Inputs:        none
// Outputs:       returns 0 if successful, nonzero otherwise
//---------------------------------------------------------------------------------
int test_multiple_threads_lqapply() {
    pthread_t pid1;
    void *ret1;
    pthread_t pid2;
    void *ret2;

    global_lqp = lqopen();

    person_t person1 = make_person("Ravi", 23, 101.1);
    person_t person2 = make_person("Serf", 22, 310.0);
    person_t person3 = make_person("Girth", 22, 405.8);

    // Test apply on an empty queue
    lqapply(global_lqp, &age);

    // Add first person
    int32_t success = lqput(global_lqp, (void*)&person1);
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    // Test put on a non empty queue
    // Add second person
    success = lqput(global_lqp, (void*)&person2);
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    // Add third person
    success = lqput(global_lqp, (void*)&person3);
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    person_t *parent_person1;
    person_t *parent_person2;

    // Test lqapply with multiple threads    
    if (pthread_create(&pid1, NULL, test_apply, &person1) != 0) {
        printf("Test Failed.\n");
        perror("pthread_create() error");
        return -1;
    }

    if (pthread_create(&pid2, NULL, test_apply, &person2) != 0) {
        printf("Test Failed.\n");
        perror("pthread_create() error");
        return -1;
    }

    // Test get on a non empty queue
    parent_person1 = (person_t*)lqget(global_lqp);
    if ( parent_person1 != &person1 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person1.name, person1.age, person1.rate,
                parent_person1->name, parent_person1->age, parent_person1->rate);
        return -1;
    }

    parent_person2 = (person_t*)lqget(global_lqp);
    if ( parent_person2 != &person2 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person2.name, person2.age, person2.rate,
                parent_person2->name, parent_person2->age, parent_person2->rate);
        return -1;
    }

    // Join child thread to parent thread
    if (pthread_join(pid1, &ret1) != 0) {
        perror("pthread_create() error");
        exit(3);
    }

    // Join child thread to parent thread
    if (pthread_join(pid2, &ret2) != 0) {
        perror("pthread_create() error");
        exit(3);
    }

    lqclose(global_lqp);
    return 0;
}

int main(void){
    int success;

    // Test the lqapply method on a locked queue with a single thread
    success = test_single_thread();
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    // Test the lqapply method on a locked queue with multiple threads
    success = test_multiple_threads_lqapply();
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}