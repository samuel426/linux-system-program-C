#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int sum1 = 0;
int sum2 = 0;
int sum3 = 0;
int sum4 = 0;
int sum5 = 0;
int sum6 = 0;
int sum7 = 0;
int sum8 = 0;
int sum9 = 0;
int sum10 = 0;

void* sum_f(void* arg) {
	for (int i = 1; i <
}

int main() {
    pthread_t tid1, tid2;

    if (pthread_create(&tid1, NULL, sum_first_half, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_create(&tid2, NULL, sum_second_half, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_join(tid1, NULL) != 0) {
        perror("pthread_join");
        exit(1);
    }
    if (pthread_join(tid2, NULL) != 0) {
        perror("pthread_join");
        exit(1);
    }

    printf("Total sum from 1 to 100 is %d\n", sum1 + sum2);

    return 0;
}
