/*===============================================================
[Program Name] : prodcons_m.c
[Description]  : 
    - 생산자와 소비자가 공유 버퍼를 통해 데이터를 생성하고 소비하는 문제를 해결.
    - 조건 변수와 뮤텍스를 이용하여 동기화 구현.
[Input]        :
    pthread_cond_t NotFull;   // 버퍼가 꽉 차지 않았음을 알리는 조건 변수
    pthread_cond_t NotEmpty;  // 버퍼가 비어 있지 않음을 알리는 조건 변수
    pthread_mutex_t Mutex;    // 공유 자원을 보호하는 뮤텍스
[Output]       :
    생산 및 소비 상태 출력, 프로그램 종료 시 버퍼 상태 출력.
[Calls]        :
    pthread_create(), pthread_join(), pthread_cond_init(), 
    pthread_mutex_init(), pthread_cond_wait(), pthread_cond_signal()
[특기사항]     : 
    - 생산자는 버퍼가 꽉 차면 대기, 소비자는 버퍼가 비면 대기.
    - 공유 자원에 접근 시 뮤텍스를 통해 상호 배제 보장.
==================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "prodcons.h"

BoundedBufferType Buf;
pthread_cond_t NotFull;
pthread_cond_t NotEmpty;
pthread_mutex_t Mutex;

/*===============================================================
[Function Name] : void ThreadUsleep(int usecs)
[Description]   : 
    - 지정된 시간만큼 현재 스레드를 대기 상태로 유지.
[Input]         :
    int usecs;    // 대기 시간 (마이크로초 단위)
[Output]        :
    Nothing
[Call By]       :
    Producer(void *dummy), Consumer(void *dummy)
[Calls]         :
    pthread_cond_init(), pthread_mutex_init(), 
    pthread_cond_timedwait(), pthread_cond_destroy(), 
    pthread_mutex_destroy()
[Given]         :
    usecs 값은 0보다 커야 하며, 대기 시간 범위를 초과하지 않음.
[Returns]       :
    Nothing
==================================================================*/
void ThreadUsleep(int usecs)
{
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    struct timespec ts;
    struct timeval tv;

    if (pthread_cond_init(&cond, NULL) < 0) {
        perror("pthread_cond_init");
        pthread_exit(NULL);
    }
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("pthread_mutex_init");
        pthread_exit(NULL);
    }

    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + usecs / 1000000;
    ts.tv_nsec = (tv.tv_usec + (usecs % 1000000)) * 1000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }

    if (pthread_mutex_lock(&mutex) < 0) {
        perror("pthread_mutex_lock");
        pthread_exit(NULL);
    }
    if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0) {
        perror("pthread_cond_timedwait");
        pthread_exit(NULL);
    }

    if (pthread_cond_destroy(&cond) < 0) {
        perror("pthread_cond_destroy");
        pthread_exit(NULL);
    }
    if (pthread_mutex_destroy(&mutex) < 0) {
        perror("pthread_mutex_destroy");
        pthread_exit(NULL);
    }
}

/*===============================================================
[Function Name] : void Producer(void *dummy)
[Description]   : 
    - 데이터를 생성하고 공유 버퍼에 저장.
    - 버퍼가 꽉 차면 대기, 공간이 생기면 데이터 추가.
[Input]         :
    Nothing
[Output]        :
    데이터 생성 상태 및 버퍼 상태 출력.
[Call By]       :
    Main()
[Calls]         :
    pthread_mutex_lock(), pthread_cond_wait(), pthread_cond_signal(), 
    ThreadUsleep()
[Given]         :
    버퍼는 초기화되어 있어야 하며, 뮤텍스와 조건 변수가 유효해야 함.
[Returns]       :
    Nothing
==================================================================*/
void Producer(void *dummy)
{
    int i, data;

    printf("Producer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        if (pthread_mutex_lock(&Mutex) < 0) {
            perror("pthread_mutex_lock");
            pthread_exit(NULL);
        }

        while (Buf.counter == MAX_BUF) {
            if (pthread_cond_wait(&NotFull, &Mutex)) {
                perror("pthread_cond_wait");
                pthread_exit(NULL);
            }
        }

        printf("Producer: Producing an item.....\n");
        data = (rand() % 100) * 10000;
        Buf.buf[Buf.in].data = data;
        Buf.in = (Buf.in + 1) % MAX_BUF;
        Buf.counter++;

        if (pthread_cond_signal(&NotEmpty) < 0) {
            perror("pthread_cond_signal");
            pthread_exit(NULL);
        }
        if (pthread_mutex_unlock(&Mutex) < 0) {
            perror("pthread_mutex_unlock");
            pthread_exit(NULL);
        }

        ThreadUsleep(data);
    }

    printf("Producer: Produced %d items.....\n", i);
    printf("Producer: %d items in buffer.....\n", Buf.counter);

    pthread_exit(NULL);
}

/*===============================================================
[Function Name] : void Consumer(void *dummy)
[Description]   : 
    - 공유 버퍼에서 데이터를 소비.
    - 버퍼가 비어 있으면 대기, 데이터가 생기면 소비.
[Input]         :
    Nothing
[Output]        :
    데이터 소비 상태 및 버퍼 상태 출력.
[Call By]       :
    Main()
[Calls]         :
    pthread_mutex_lock(), pthread_cond_wait(), pthread_cond_signal(), 
    ThreadUsleep()
[Given]         :
    버퍼는 초기화되어 있어야 하며, 뮤텍스와 조건 변수가 유효해야 함.
[Returns]       :
    Nothing
==================================================================*/
void Consumer(void *dummy)
{
    int i, data;

    printf("Consumer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        if (pthread_mutex_lock(&Mutex)) {
            perror("pthread_mutex_lock");
            pthread_exit(NULL);
        }

        while (Buf.counter == 0) {
            if (pthread_cond_wait(&NotEmpty, &Mutex)) {
                perror("pthread_cond_wait");
                pthread_exit(NULL);
            }
        }

        printf("Consumer: Consuming an item.....\n");
        data = Buf.buf[Buf.out].data;
        Buf.out = (Buf.out + 1) % MAX_BUF;
        Buf.counter--;

        if (pthread_cond_signal(&NotFull) < 0) {
            perror("pthread_cond_signal");
            pthread_exit(NULL);
        }
        if (pthread_mutex_unlock(&Mutex) < 0) {
            perror("pthread_mutex_unlock");
            pthread_exit(NULL);
        }

        ThreadUsleep((rand() % 100) * 10000);
    }

    printf("Consumer: Consumed %d items.....\n", i);
    printf("Consumer: %d items in buffer.....\n", Buf.counter);

    pthread_exit(NULL);
}

int main()
{
    pthread_t tid1, tid2;

    srand(0x8888);

    if (pthread_cond_init(&NotFull, NULL) < 0) {
        perror("pthread_cond_init");
        pthread_exit(NULL);
    }
    if (pthread_cond_init(&NotEmpty, NULL) < 0) {
        perror("pthread_cond_init");
        pthread_exit(NULL);
    }
    if (pthread_mutex_init(&Mutex, NULL) < 0) {
        perror("pthread_mutex_init");
        pthread_exit(NULL);
    }

    if (pthread_create(&tid1, NULL, (void *)Producer, (void *)NULL) < 0) {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_create(&tid2, NULL, (void *)Consumer, (void *)NULL) < 0) {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_join(tid1, NULL) < 0) {
        perror("pthread_join");
        exit(1);
    }
    if (pthread_join(tid2, NULL) < 0) {
        perror("pthread_join");
        exit(1);
    }

    printf("Main    : %d items in buffer.....\n", Buf.counter);

    if (pthread_cond_destroy(&NotFull) < 0) {
        perror("pthread_cond_destroy");
    }
    if (pthread_cond_destroy(&NotEmpty) < 0) {
        perror("pthread_cond_destroy");
    }
    if (pthread_mutex_destroy(&Mutex) < 0) {
        perror("pthread_mutex_destroy");
    }
}

