/* test_queue_open_close.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Sat Oct 02 11:33:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test qopen and qclose.
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
    
    person_t person1 = make_person("Javier", 22, 100.0);
    person_t person2 = make_person("Sergio", 22, 99.9);
    person_t person3 = make_person("Garth", 22, 120.22);

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

    qclose(qp);
    exit(EXIT_SUCCESS);
}