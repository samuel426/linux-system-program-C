/*===============================================================
[Program Name] : prodcons_t.c
[Description]  : 
    - 생산자와 소비자 문제를 해결하기 위한 단순 구현.
    - 생산자와 소비자가 공유 버퍼를 사용하여 데이터를 생성하고 소비.
    - 동기화 없이 버퍼 상태를 반복 검사하며 대기.
[Input]        :
    BoundedBufferType Buf;  // 공유 버퍼
    pthread_t tid1, tid2;   // 생산자 및 소비자 스레드
[Output]       :
    생산 및 소비 상태 출력, 프로그램 종료 시 버퍼 상태 출력.
[Calls]        :
    pthread_create(), pthread_join(), ThreadUsleep()
[특기사항]     : 
    - 동기화 없이 상태를 검사하며 대기하는 방식으로 구현.
    - 생산자는 버퍼가 꽉 차면 대기, 소비자는 버퍼가 비면 대기.
==================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "prodcons.h"

BoundedBufferType Buf;

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
    ThreadUsleep()
[Given]         :
    공유 버퍼는 초기화되어 있어야 함.
[Returns]       :
    Nothing
==================================================================*/
void Producer(void *dummy)
{
    int i, data;

    printf("Producer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        if (Buf.counter == MAX_BUF) {
            printf("Producer: Buffer full. Waiting.....\n");
            while (Buf.counter == MAX_BUF)
                ;
        }

        printf("Producer: Producing an item.....\n");
        data = (rand() % 100) * 10000;
        Buf.buf[Buf.in].data = data;
        Buf.in = (Buf.in + 1) % MAX_BUF;
        Buf.counter++;

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
    ThreadUsleep()
[Given]         :
    공유 버퍼는 초기화되어 있어야 함.
[Returns]       :
    Nothing
==================================================================*/
void Consumer(void *dummy)
{
    int i, data;

    printf("Consumer: Start.....\n");

    for (i = 0; i < NLOOPS; i++) {
        if (Buf.counter == 0) {
            printf("Consumer: Buffer empty. Waiting.....\n");
            while (Buf.counter == 0)
                ;
        }

        printf("Consumer: Consuming an item.....\n");
        data = Buf.buf[Buf.out].data;
        Buf.out = (Buf.out + 1) % MAX_BUF;
        Buf.counter--;

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
}

