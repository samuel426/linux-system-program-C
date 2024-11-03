#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int sum1 = 0; // 첫 번째 스레드에서 계산된 합계
int sum2 = 0; // 두 번째 스레드에서 계산된 합계

/*===============================================================
[Function Name] : void* sum_first_half(void* arg)
[Description]  : 1부터 50까지의 합을 계산하는 스레드 함수
[Input]        : Nothing
[Output]       : Nothing (합계는 전역 변수 sum1에 저장됨)
[Calls]        : pthread_exit
[Given]        : Nothing
[Returns]      : NULL
==================================================================*/
void* sum_first_half(void* arg) {
    for (int i = 1; i <= 50; i++) {
        sum1 += i;
    }
    printf("Thread 1 finished: sum of 1 to 50 is %d\n", sum1);
    pthread_exit(NULL);
}

/*===============================================================
[Function Name] : void* sum_second_half(void* arg)
[Description]  : 51부터 100까지의 합을 계산하는 스레드 함수
[Input]        : Nothing
[Output]       : Nothing (합계는 전역 변수 sum2에 저장됨)
[Calls]        : pthread_exit
[Given]        : Nothing
[Returns]      : NULL
==================================================================*/
void* sum_second_half(void* arg) {
    for (int i = 51; i <= 100; i++) {
        sum2 += i;
    }
    printf("Thread 2 finished: sum of 51 to 100 is %d\n", sum2);
    pthread_exit(NULL);
}

/*===============================================================
[Program Name] : tadd
[Description]  : 두 개의 스레드를 생성하여 1부터 100까지의 합을 구함.
[Input]        : Nothing
[Output]       : 각 스레드에서 계산한 합과 전체 합을 출력함.
[Calls]        : pthread_create, pthread_join
[특기사항]     : 스레드 생성과 종료를 위해 pthread 라이브러리 사용.
==================================================================*/
int main() {
    pthread_t tid1, tid2;

    // 첫 번째 스레드 생성: 1~50 합계 계산
    if (pthread_create(&tid1, NULL, sum_first_half, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }

    // 두 번째 스레드 생성: 51~100 합계 계산
    if (pthread_create(&tid2, NULL, sum_second_half, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }

    // 두 스레드가 종료될 때까지 기다림
    if (pthread_join(tid1, NULL) != 0) {
        perror("pthread_join");
        exit(1);
    }
    if (pthread_join(tid2, NULL) != 0) {
        perror("pthread_join");
        exit(1);
    }

    // 두 합계 출력
    printf("Total sum from 1 to 100 is %d\n", sum1 + sum2);
    
    return 0;
}

