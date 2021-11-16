/* test_lhash_apply.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Mon Nov 15 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test lhapply with a single thread and multiple threads.
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
const uint32_t HSIZE = 100;

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
// Description:   tests the lhapply method on the locked hashtable with a single thread
// Inputs:        none
// Outputs:       returns 0 if successful, nonzero otherwise
//---------------------------------------------------------------------------------
int test_single_thread() {

    lhashtable_t *lhtp = lhopen(HSIZE);

    lhapply(lhtp, &age);

	person_t person1 = make_person("Javi", 23, 101.1);
    person_t person2 = make_person("Serfio", 22, 310.0);
    person_t person3 = make_person("Girth", 21, 405.8);
    //person_t person4 = make_person("Girth", 11, 99.9);

    // Test put on an empty queue
    // Add first person
    int32_t success = lhput(lhtp, (void*)&person1, "Ravi", strlen("Ravi"));
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    // Test put on a non empty queue
    // Add second person
    success = lhput(lhtp, (void*)&person2, "Serf", strlen("Serf"));
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    // Add third person
    success = lhput(lhtp, (void*)&person3, "Girth", strlen("Girth"));
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    // // Add fourth person
    // success = lhput(lhtp, (void*)&person4, "Girth", strlen("Girth"));
    // if ( success != 0 ) {
    //     printf("Test Failed\n");
    //     return success;
    // }

    // Expected ages
    int expected_javi_age = person1.age + 1;
    int expected_serg_age = person2.age + 1;
    int expected_garth_age = person3.age + 1;
    //int expected_collision_age = person4.age + 10;

    // Test apply on 4 elements
	lhapply(lhtp, &age);

    // Test get on a non empty queue
    if ( person1.age != expected_javi_age ) {
        printf("Test Failed.\n");
        printf("Expected: %d / Actual: %d\n",
                expected_javi_age, person1.age);
        return -1;
    }

    if ( person2.age != expected_serg_age ) {
        printf("Test Failed.\n");
        printf("Expected: %d / Actual: %d\n",
                expected_serg_age, person2.age);
        return -1;
    }

    if ( person3.age != expected_garth_age ) {
        printf("Test Failed.\n");
        printf("Expected: %d / Actual: %d\n",
                expected_garth_age, person3.age);
        return -1;
    }

    // if ( person4.age != expected_collision_age ) {
    //     printf("Test 4 Failed.\n");
    //     printf("Expected: %d / Actual: %d\n",
    //             expected_collision_age, person3.age);
    //     return -1;
    // }

    lhclose(lhtp);
    return 0;
}

// Global locked queue for the multiple threads test
lhashtable_t *global_lhtp;

void *test_apply(void *arg) {
    int ret;

    // Apply three different functions to locked queue
    lhapply(global_lhtp, &name);
    lhapply(global_lhtp, &age);
    lhapply(global_lhtp, &rate);

    ret = 0;
    pthread_exit(&ret);
}

//------------------------- test_multiple_threads_lqapply ---------------------------
// Description:   tests the lhapply method on the locked hashtable with multiple threads
// Inputs:        none
// Outputs:       returns 0 if successful, nonzero otherwise
//---------------------------------------------------------------------------------
int test_multiple_threads_lhapply() {
    pthread_t pid1;
    void *ret1;
    pthread_t pid2;
    void *ret2;

    global_lhtp = lhopen(HSIZE);

    person_t person1 = make_person("Javi", 23, 101.1);
    person_t person2 = make_person("Serfio", 22, 310.0);
    person_t person3 = make_person("Girth", 21, 405.8);
    //person_t person4 = make_person("Girth", 11, 99.9);

    lhapply(global_lhtp, &age);

    // Test put on an empty hashtable
    // Add first person
    int32_t success = lhput(global_lhtp, (void*)&person1, "Ravi", strlen("Ravi"));
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    // Test put on a non empty hashtable
    // Add second person
    success = lhput(global_lhtp, (void*)&person2, "Serf", strlen("Serf"));
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    // Add third person
    success = lhput(global_lhtp, (void*)&person3, "Girth", strlen("Girth"));
    if ( success != 0 ) {
        printf("Test Failed\n");
        return success;
    }

    // // Add fourth person
    // success = lhput(global_lhtp, (void*)&person4, "Girth", strlen("Girth"));
    // if ( success != 0 ) {
    //     printf("Test Failed\n");
    //     return success;
    // }

    // Test lhapply with multiple threads    
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

    lhclose(global_lhtp);
    return 0;
}

int main(void){
    int success;

    // Test the lhapply method on a locked hashtable with a single thread
    success = test_single_thread();
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    // Test the lhapply method on a locked hashtable with multiple threads
    success = test_multiple_threads_lhapply();
    if ( success != 0 ) {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
} 
