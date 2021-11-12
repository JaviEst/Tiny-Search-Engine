/* test_lqueue_open_close.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Thur Nov 11 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test lqopen and lqclose.
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

int main(void){
    // Test opening and closing an empty locked queue
    lqueue_t *empty_lqp = lqopen();
    lqclose(empty_lqp);

    // Test opening and closing a non empty locked queue
    lqueue_t *lqp = lqopen();
    
    person_t person1 = make_person("Javier", 22, 100.0);
    person_t person2 = make_person("Sergio", 22, 99.9);
    person_t person3 = make_person("Garth", 22, 120.22);

    // Add first person
    int32_t success = lqput(lqp, (void*)&person1);
    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

    // Add second person
    success = lqput(lqp, (void*)&person2);
    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

    // Add third person
    success = lqput(lqp, (void*)&person3);
    if ( success != 0 ) {
        printf("Test Failed\n");
        exit(EXIT_FAILURE);
    }

    lqclose(lqp);
    exit(EXIT_SUCCESS);
}