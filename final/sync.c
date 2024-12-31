#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

#define NLOOPS 5

sem_t sem;

static int Pfd1[1], Pdf[2];

void* parent_thread(void* arg) {
	printf("Parent: Tell to child\n");
	printf("Parent: Wait for child to tell\n");
}

void* child_thread(void* arg) {
}



int main() {
	
	pthread_t parent, child;

	sem_init(&sem, 0, 0);

	pthread_create(&parent, NULL, parent_thread, NULL);
        pthread_create(&child, NULL, child_thread, NULL);

	

	pthread_join(parent, NULL);
	pthread_join(child, NULL);

	sem_destroy(&sem);

}
