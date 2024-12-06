/*===============================================================
[Program Name] : prodcons_s.c
[Description]  : 
    - 생산자와 소비자 문제를 세마포어를 사용하여 해결.
    - 생산자는 공유 버퍼가 꽉 차면 대기하고, 소비자는 버퍼가 비면 대기.
    - 세마포어를 통해 공유 자원의 동기화 및 상호 배제를 보장.
[Input]        :
    sem_t EmptySem;  // 버퍼에 빈 공간의 수를 관리하는 세마포어
    sem_t FullSem;   // 버퍼에 데이터가 있는 공간의 수를 관리하는 세마포어
    sem_t MutexSem;  // 버퍼 접근 동기화를 위한 뮤텍스 세마포어
[Output]       :
    생산 및 소비 상태 출력, 프로그램 종료 시 버퍼 상태 출력.
[Calls]        :
    pthread_create(), pthread_join(), sem_init(), sem_wait(), 
    sem_post(), sem_destroy(), ThreadUsleep()
[특기사항]     : 
    - 생산자는 버퍼의 빈 공간(EmptySem)을 확인 후 데이터 생성.
    - 소비자는 버퍼의 데이터(FullSem)를 확인 후 데이터 소비.
    - MutexSem을 사용하여 공유 자원의 동시 접근 방지.
==================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "semlib2.h"
#include "prodcons.h"

BoundedBufferType Buf;
sem_t EmptySem, FullSem, MutexSem;

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
    - 버퍼가 꽉 차면 대기하며, 공간이 생기면 데이터 추가.
[Input]         :
    Nothing
[Output]        :
    데이터 생성 상태 및 버퍼 상태 출력.
[Call By]       :
    Main()
[Calls]         :
    sem_wait(), sem_post(), ThreadUsleep()
[Given]         :
    세마포어와 공유 버퍼는 초기화되어 있어야 함.
[Returns]       :
    Nothing
==================================================================*/
void Producer(void *dummy)
{
    int i, data;

    printf("Producer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        if (sem_wait(&EmptySem) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }
        if (sem_wait(&MutexSem) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }

        printf("Producer: Producing an item.....\n");
        data = (rand() % 100) * 10000;
        Buf.buf[Buf.in].data = data;
        Buf.in = (Buf.in + 1) % MAX_BUF;
        Buf.counter++;

        if (sem_post(&MutexSem) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }
        if (sem_post(&FullSem) < 0) {
            perror("sem_post");
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
    - 버퍼가 비어 있으면 대기하며, 데이터가 생기면 소비.
[Input]         :
    Nothing
[Output]        :
    데이터 소비 상태 및 버퍼 상태 출력.
[Call By]       :
    Main()
[Calls]         :
    sem_wait(), sem_post(), ThreadUsleep()
[Given]         :
    세마포어와 공유 버퍼는 초기화되어 있어야 함.
[Returns]       :
    Nothing
==================================================================*/
void Consumer(void *dummy)
{
    int i, data;

    printf("Consumer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        if (sem_wait(&FullSem) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }
        if (sem_wait(&MutexSem) < 0) {
            perror("sem_wait");
            pthread_exit(NULL);
        }

        printf("Consumer: Consuming an item.....\n");
        data = Buf.buf[Buf.out].data;
        Buf.out = (Buf.out + 1) % MAX_BUF;
        Buf.counter--;

        if (sem_post(&MutexSem) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }
        if (sem_post(&EmptySem) < 0) {
            perror("sem_post");
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

    if (sem_init(&EmptySem, 0, MAX_BUF) < 0) {
        perror("sem_init");
        exit(1);
    }
    if (sem_init(&FullSem, 0, 0) < 0) {
        perror("sem_init");
        exit(1);
    }
    if (sem_init(&MutexSem, 0, 1) < 0) {
        perror("sem_init");
        exit(1);
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

    if (sem_destroy(&EmptySem) < 0) {
        perror("sem_destroy");
    }
    if (sem_destroy(&FullSem) < 0) {
        perror("sem_destroy");
    }
    if (sem_destroy(&MutexSem) < 0) {
        perror("sem_destroy");
    }
}

