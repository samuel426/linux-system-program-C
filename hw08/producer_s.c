/*===============================================================
[Program Name] : producer_s.c
[Description]  : 
    - 생산자 프로세스가 공유 메모리를 사용하여 데이터를 생성하고 버퍼에 추가.
    - 세마포어를 통해 동기화 및 상호 배제를 구현.
[Input]        :
    공유 메모리 및 세마포어
    int emptySemid; // 버퍼의 빈 공간을 관리하는 세마포어
    int fullSemid;  // 버퍼에 데이터가 있는 공간을 관리하는 세마포어
    int mutexSemid; // 버퍼 접근 동기화를 위한 뮤텍스 세마포어
[Output]       :
    생성된 데이터 상태 출력, 버퍼 상태 출력.
[Calls]        :
    shmget(), shmat(), semInit(), semWait(), semPost(), semDestroy(), shmctl()
[특기사항]     : 
    - 생산자는 버퍼가 꽉 차면 대기.
    - 세마포어와 공유 메모리가 초기화되어 있어야 정상 동작.
==================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semlib.h"
#include "prodcons.h"

int main()
{
    BoundedBufferType *pBuf;
    int shmid, i, data;
    int emptySemid, fullSemid, mutexSemid;

    // 공유 메모리 생성 및 연결
    if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // 세마포어 초기화
    if ((emptySemid = semInit(EMPTY_SEM_KEY)) < 0) {
        fprintf(stderr, "semInit failure\n");
        exit(1);
    }
    if ((fullSemid = semInit(FULL_SEM_KEY)) < 0) {
        fprintf(stderr, "semInit failure\n");
        exit(1);
    }
    if ((mutexSemid = semInit(MUTEX_SEM_KEY)) < 0) {
        fprintf(stderr, "semInit failure\n");
        exit(1);
    }

    srand(0x8888);
    for (i = 0; i < NLOOPS; i++) {
        // 빈 공간 세마포어 대기
        if (semWait(emptySemid) < 0) {
            fprintf(stderr, "semWait failure\n");
            exit(1);
        }
        // 뮤텍스 세마포어 대기
        if (semWait(mutexSemid) < 0) {
            fprintf(stderr, "semWait failure\n");
            exit(1);
        }

        // 데이터 생성 및 버퍼에 추가
        printf("Producer: Producing an item.....\n");
        data = (rand() % 100) * 10000;
        pBuf->buf[pBuf->in].data = data;
        pBuf->in = (pBuf->in + 1) % MAX_BUF;
        pBuf->counter++;

        // 뮤텍스 세마포어 해제
        if (semPost(mutexSemid) < 0) {
            fprintf(stderr, "semPost failure\n");
            exit(1);
        }
        // 데이터 공간 세마포어 신호
        if (semPost(fullSemid) < 0) {
            fprintf(stderr, "semPost failure\n");
            exit(1);
        }

        usleep(data);
    }

    printf("Producer: Produced %d items.....\n", i);

    sleep(2);
    printf("Producer: %d items in buffer.....\n", pBuf->counter);

    // 세마포어 및 공유 메모리 해제
    if (semDestroy(emptySemid) < 0) {
        fprintf(stderr, "semDestroy failure\n");
    }
    if (semDestroy(fullSemid) < 0) {
        fprintf(stderr, "semDestroy failure\n");
    }
    if (semDestroy(mutexSemid) < 0) {
        fprintf(stderr, "semDestroy failure\n");
    }
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        perror("shmctl");
        exit(1);
    }

    return 0;
}

