/* test_lqueue_search_remove.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thur Nov 14 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test lqsearch and lqremove with a single thread and multiple threads.
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

//----------------------------- search_name --------------------------------
// Description:   searches for a person by name
// Inputs:        element pointer and const key
// Outputs:       returns true if found, false otherwise
//---------------------------------------------------------------------------------
bool search_name(void *elementp, const void* keyp) {
    person_t *person = (person_t*)elementp;
    keyp = (char*)keyp;

    if ( strcmp(person->name, keyp) == 0 ) {
        return true;
    }
    return false;
}

//----------------------------- test_single_thread --------------------------------
// Description:   tests the lqsearch and lqremove methods of the locked queue with a single thread
// Inputs:        none
// Outputs:       returns 0 if successful, nonzero otherwise
//---------------------------------------------------------------------------------
int test_single_thread() {
    lqueue_t *lqp = lqopen();
    
    person_t person1 = make_person("Ravi", 23, 101.1);
    person_t person2 = make_person("Serf", 22, 310.0);
    person_t person3 = make_person("Girth", 22, 405.8);

    person_t *get_emp = (person_t*)lqget(lqp);
    // Test get on an empty queue
    if ( get_emp != NULL ) {
        printf("Test Failed\n");
        return -1;
    }

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

    person_t *javi;
    person_t *serg;
    person_t *garth;
    char* p1_name = "Ravi";
    char* p2_name = "Serf";
    char* p3_name = "Girth";


    // Test get on a non empty queue
    javi = (person_t*)lqsearch(lqp, &search_name, (void*)p1_name);
    if ( javi != &person1 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person1.name, person1.age, person1.rate,
                javi->name, javi->age, javi->rate);
        return -1;
    }

    javi = (person_t*)lqremove(lqp, &search_name, (void*)p1_name);
    if ( javi != &person1 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person1.name, person1.age, person1.rate,
                javi->name, javi->age, javi->rate);
        return -1;
    }

    serg = (person_t*)lqsearch(lqp, &search_name, (void*)p2_name);
    if ( serg != &person2 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person2.name, person2.age, person2.rate,
                serg->name, serg->age, serg->rate);
        return -1;
    }

    serg = (person_t*)lqremove(lqp, &search_name, (void*)p2_name);
    if ( serg != &person2 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person2.name, person2.age, person2.rate,
                serg->name, serg->age, serg->rate);
        return -1;
    }

    garth = (person_t*)lqsearch(lqp, &search_name, (void*)p3_name);
    if ( garth != &person3 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person3.name, person3.age, person3.rate,
                garth->name, garth->age, garth->rate);
        return -1;
    }

    garth = (person_t*)lqremove(lqp, &search_name, (void*)p3_name);
    if ( garth != &person3 ) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person3.name, person3.age, person3.rate,
                garth->name, garth->age, garth->rate);
        return -1;
    }

    lqclose(lqp);
    return 0;
}

// Global locked queue for the multiple threads test
lqueue_t *global_lqp;

void *test_search(void *arg) {
    int ret;

    person_t *person_key = (person_t*)arg;
    person_t *person;

    person = (person_t*)lqsearch(global_lqp, &search_name, (void*)person_key->name);
    if ( strcmp(person->name, person_key->name) != 0) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person_key->name, person_key->age, person_key->rate,
                person->name, person->age, person->rate);

        ret = -1;
        pthread_exit(&ret);
    }
    ret = 0;
    pthread_exit(&ret);
}

//------------------------- test_multiple_threads_lqsearch ------------------------
// Description:   tests lqsearch of the locked queue with multiple threads
// Inputs:        none
// Outputs:       returns 0 if successful, nonzero otherwise
//---------------------------------------------------------------------------------
int test_multiple_threads_lqsearch() {
    pthread_t pid1;
    void *ret1;
    pthread_t pid2;
    void *ret2;

    global_lqp = lqopen();

    person_t person1 = make_person("Ravi", 23, 101.1);
    person_t person2 = make_person("Serf", 22, 310.0);
    person_t person3 = make_person("Girth", 22, 405.8);

    // Test put on an empty queue
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

    // Test lqsearch with multiple threads
    if (pthread_create(&pid1, NULL, test_search, &person3) != 0) {
        printf("Test Failed.\n");
        perror("pthread_create() error");
        return -1;
    }

    if (pthread_create(&pid2, NULL, test_search, &person2) != 0) {
        printf("Test Failed.\n");
        perror("pthread_create() error");
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

void *test_remove(void *arg) {
    int ret;

    person_t *person_key = (person_t*)arg;
    person_t *person;

    person = (person_t*)lqremove(global_lqp, &search_name, (void*)person_key->name);
    if ( strcmp(person->name, person_key->name) != 0) {
        printf("Test Failed.\n");
        printf("Expected: %s & %d & %lf / Actual: %s & %d & %lf\n",
                person_key->name, person_key->age, person_key->rate,
                person->name, person->age, person->rate);

        ret = -1;
        pthread_exit(&ret);
    }
    ret = 0;
    pthread_exit(&ret);
}

//------------------------- test_multiple_threads_lqremove ------------------------
// Description:   tests lqremove of the locked queue with multiple threads
// Inputs:        none
// Outputs:       returns 0 if successful, nonzero otherwise
//---------------------------------------------------------------------------------
int test_multiple_threads_lqremove() {
    pthread_t pid1;
    void *ret1;
    pthread_t pid2;
    void *ret2;

    global_lqp = lqopen();

    person_t person1 = make_person("Ravi", 23, 101.1);
    person_t person2 = make_person("Serf", 22, 310.0);
    person_t person3 = make_person("Girth", 22, 405.8);

    // Test put on an empty queue
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

    // Test lqsearch with multiple threads
    if (pthread_create(&pid1, NULL, test_remove, &person3) != 0) {
        printf("Test Failed.\n");
        perror("pthread_create() error");
        return -1;
    }

    if (pthread_create(&pid2, NULL, test_remove, &person2) != 0) {
        printf("Test Failed.\n");
        perror("pthread_create() error");
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

    // Test the lqsearch and lqremove methods of a locked queue with a single thread
    success = test_single_thread();
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    // Test the lqsearch method of a locked queue with multiple thread
    success = test_multiple_threads_lqsearch();
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    // Test the lqremove method of a locked queue with multiple thread
    success = test_multiple_threads_lqremove();
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}