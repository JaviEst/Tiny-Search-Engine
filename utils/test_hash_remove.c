/* test_hash_remove.c --- 
 * 
 * 
 * Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Tue Oct 05 09:22:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test hremove on a non empty hash table.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "hash.h"

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

bool search_name(void *elementp, const void* keyp) {
    char *person = (char*)elementp;
    char *key = (char*)keyp;

    if ( strcmp(person, key) == 0 ) {
        return true;
    }
    return false;
}

int main(void){
	hashtable_t *htp = hopen(10);

    person_t person1 = make_person("Javi", 23, 101.1);
    person_t person2 = make_person("Serfio", 30, 510.0);
    person_t person3 = make_person("Girth", 21, 405.8);
	person_t person4 = make_person("Girth", 11, 99.9);
    person_t person5 = make_person("Javi", 15, 69.9);
	
    int32_t result = hput(htp, (void*)&person1, "Javi", sizeof("Javi"));
    if ( result != 0 ) {
        printf("Test failed. \n");
        exit(EXIT_FAILURE);
    }
        
    result = hput(htp, (void*)&person2, "Serfio", sizeof("Serfio"));
    if ( result != 0 ) {
        printf("Test failed. \n");
        exit(EXIT_FAILURE);
    }
        
    result = hput(htp, (void*)&person3, "Girth", sizeof("Girth"));
    if ( result != 0 ) {
        printf("Test failed. \n");
        exit(EXIT_FAILURE);
    }

    result = hput(htp, (void*)&person4, "Girth", sizeof("Girth"));
    if ( result != 0 ) {
        printf("Test failed. \n");
        exit(EXIT_FAILURE);
    }

    result = hput(htp, (void*)&person5, "Javi", sizeof("Javi"));
    if ( result != 0 ) {
        printf("Test failed. \n");
        exit(EXIT_FAILURE);
    }

    // Test on a non empty hashtable
    person_t *check = (person_t*)hremove(htp, &search_name, "Serfio", sizeof("Serfio"));
	if ( check != &person2 ) {
        printf("Test Failed.\n");
        //printf("Check: %p", check);
        printf("Expected: %d & %lf / Actual: %d & %lf\n",
                person2.age, person2.rate, check->age, check->rate);
        hclose(htp);
        exit(EXIT_FAILURE);
    }

    hclose(htp);
	exit(EXIT_SUCCESS);
}