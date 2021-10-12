/* test_hash_apply.c --- 
 * 
 * 
* Author: Sergio E. Bobadilla Javier Esteban Garth Verdeflor
 * Created: Tue Oct 05 09:22:29 2021 (-0400)
 * Version: 1.0
 * 
 * Description: Test happly.
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

void age(void *element) {
    person_t *cp = (person_t*)element;
    cp->age = cp->age + 10;
}

int main(void){
	hashtable_t *htp = hopen(10);

	person_t person1 = make_person("Javi", 23, 101.1);
  person_t person2 = make_person("Serfio", 22, 310.0);
  person_t person3 = make_person("Girth", 21, 405.8);
	person_t person4 = make_person("Girth", 11, 99.9);
	
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
   
 
	int expected_javi_age = person1.age + 10;
	int expected_serg_age = person2.age + 10;
	int expected_garth_age = person3.age + 10;
	int expected_collision_age = person4.age + 10;

	// Test apply in 4 elements
	happly(htp, &age);

	if (person1.age != expected_javi_age) {
		printf("Test Failed.\n");
		printf("Expected: %d / Actual: %d",
					 expected_javi_age, person1.age);
		exit(EXIT_FAILURE);
	}

	if (person2.age != expected_serg_age) {
    printf("Test Failed.\n");
    printf("Expected: %d / Actual: %d",
           expected_serg_age, person2.age);
    exit(EXIT_FAILURE);
  }

	if (person3.age != expected_garth_age) {
    printf("Test Failed.\n");
    printf("Expected: %d / Actual: %d",
           expected_garth_age, person3.age);
    exit(EXIT_FAILURE);
  }

	if (person4.age != expected_collision_age) {
    printf("Test Failed.\n");
    printf("Expected: %d / Actual: %d",
           expected_collision_age, person4.age);
    exit(EXIT_FAILURE);
  }
	exit(EXIT_SUCCESS);
}
