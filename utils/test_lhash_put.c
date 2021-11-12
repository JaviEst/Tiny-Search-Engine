/* test_lhash_put.c --- 
 * 
 * 
* Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thur Nov 11 09:22:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test hput with a single and multiple thread.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "lhash.h"

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

//----------------------------- test_single_thread --------------------------------
// Description:   tests the lhput method of the locked hashtable with a single thread
// Inputs:        none
// Outputs:       returns 0 if successful, nonzero otherwise
//---------------------------------------------------------------------------------
int test_single_thread() {
    lhashtable_t *lhtp = lhopen(10);
    
    person_t person1 = make_person("Girth", 23, 101.1);
    person_t person2 = make_person("Girth", 22, 310.0);
    person_t person3 = make_person("Girth", 22, 405.8);

    int32_t result = lhput(lhtp, (void*)&person1, "Girth", sizeof("Girth"));
    if ( result != 0 ) {
        return 1;
    }

    result = lhput(lhtp, (void*)&person2, "Girth", sizeof("Girth"));
    if ( result != 0 ) {
        return 1;
    }

    result = lhput(lhtp, (void*)&person3, "Girth", sizeof("Girth"));
    if ( result != 0 ) {
        return 1;
    }

    lhclose(lhtp);
    return 0;
}



// Global locked hashtable for the multiple threads test
lhashtable_t *global_lhtp;

void *test_put(void *arg) {
    int ret1;

    person_t *person = (person_t*)arg;

    // Add person
    int32_t success = lhput(global_lhtp, (void*)arg, person->name, strlen(person->name));
    if ( success != 0 ) {
        printf("Test Failed\n");
        ret1 = -1;
        pthread_exit(&ret1);
    }
    ret1 = 0;
    pthread_exit(&ret1);
}

//----------------------------- test_multiple_threads -----------------------------
// Description:   tests the lhput method of the locked hashtable with multiple threads
// Inputs:        none
// Outputs:       returns 0 if successful, nonzero otherwise
//---------------------------------------------------------------------------------
int test_multiple_threads() {
    pthread_t pid1;
    void *ret1;
    pthread_t pid2;
    void *ret2;

    global_lhtp = lhopen(10);

    person_t person1 = make_person("Ravi", 23, 101.1);
    person_t person2 = make_person("Serf", 22, 310.0);
    person_t person3 = make_person("Girth", 22, 405.8);

    // Test put on an empty queue
    // Add first person
    int32_t success = lhput(global_lhtp, (void*)&person1, person1.name, strlen(person1.name));
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    // Test lqput with multiple threads
    if (pthread_create(&pid1, NULL, test_put, &person3) != 0) {
        printf("Test Failed.\n");
        perror("pthread_create() error");
        return -1;
    }

    // Join child thread to parent thread
    if (pthread_join(pid1, &ret1) != 0) {
        perror("pthread_create() error");
        exit(3);
    }

    if (pthread_create(&pid2, NULL, test_put, &person2) != 0) {
        printf("Test Failed.\n");
        perror("pthread_create() error");
        return -1;
    }

    // Join child thread to parent thread
    if (pthread_join(pid2, &ret2) != 0) {
        perror("pthread_create() error");
        exit(3);
    }

    lhclose(global_lhtp);
    return 0;
}


int main(void){
    int success;

    // Test the lqput and lqget methods of a locked queue with a single thread
    success = test_single_thread();
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    // Test the lqget method of a locked queue with multiple thread
    success = test_multiple_threads();
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}