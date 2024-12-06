/*===============================================================
[Program Name] : consumer_s.c
[Description]  : 
    - 공유 메모리와 세마포어를 이용하여 생산자/소비자 문제를 해결하는 소비자 프로그램.
    - 공유 버퍼에서 데이터를 소비하며, 세마포어로 동기화를 관리.
[Input]        :
    공유 메모리 및 세마포어 (SHM_KEY, EMPTY_SEM_KEY, FULL_SEM_KEY, MUTEX_SEM_KEY)  
    int shmid;              // 공유 메모리 식별자
    int emptySemid;         // empty 세마포어 식별자
    int fullSemid;          // full 세마포어 식별자
    int mutexSemid;         // mutex 세마포어 식별자
[Output]       :
    소비된 데이터 정보 출력
    프로그램 종료 시 버퍼 상태 및 소비된 아이템 개수 출력
[Calls]        :
    semInit(int key), semInitValue(int semid, int value), 
    semWait(int semid), semPost(int semid)
[특기사항]     : 
    - 공유 메모리와 세마포어 초기화 과정 중 에러 발생 시 프로그램 종료.
    - 버퍼의 크기(MAX_BUF)는 프로덕션 코드에서 정의된 상수값.
    - 소비자 프로세스는 난수를 이용해 소비 시간 간격을 랜덤화함.
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

    // 공유 메모리 초기화
    if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  {
        perror("shmget");
        exit(1);
    }
    if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *) -1)  {
        perror("shmat");
        exit(1);
    }

    // 세마포어 초기화
    if ((emptySemid = semInit(EMPTY_SEM_KEY)) < 0)  {
        fprintf(stderr, "semInit failure\n");
        exit(1);
    }
    if ((fullSemid = semInit(FULL_SEM_KEY)) < 0)  {
        fprintf(stderr, "semInit failure\n");
        exit(1);
    }
    if ((mutexSemid = semInit(MUTEX_SEM_KEY)) < 0)  {
        fprintf(stderr, "semInit failure\n");
        exit(1);
    }

    // 세마포어 초기값 설정
    if (semInitValue(emptySemid, MAX_BUF) < 0)  {
        fprintf(stderr, "semInitValue failure\n");
        exit(1);
    }
    if (semInitValue(fullSemid, 0) < 0)  {
        fprintf(stderr, "semInitValue failure\n");
        exit(1);
    }
    if (semInitValue(mutexSemid, 1) < 0)  {
        fprintf(stderr, "semInitValue failure\n");
        exit(1);
    }

    srand(0x9999);
    for (i = 0 ; i < NLOOPS ; i++)  {
        // full 세마포어 대기
        if (semWait(fullSemid) < 0)  {
            fprintf(stderr, "semWait failure\n");
            exit(1);
        }
        // mutex 세마포어 대기
        if (semWait(mutexSemid) < 0)  {
            fprintf(stderr, "semWait failure\n");
            exit(1);
        }

        // 소비자 작업 수행
        printf("Consumer: Consuming an item.....\n");
        data = pBuf->buf[pBuf->out].data;
        pBuf->out = (pBuf->out + 1) % MAX_BUF; // 순환 버퍼 처리
        pBuf->counter--;

        // mutex 세마포어 해제
        if (semPost(mutexSemid) < 0)  {
            fprintf(stderr, "semPost failure\n");
            exit(1);
        }
        // empty 세마포어 해제
        if (semPost(emptySemid) < 0)  {
            fprintf(stderr, "semPost failure\n");
            exit(1);
        }

        // 작업 간 랜덤 대기
        usleep((rand() % 100) * 10000);
    }

    // 최종 결과 출력
    printf("Consumer: Consumed %d items.....\n", i);
    printf("Consumer: %d items in buffer.....\n", pBuf->counter);
}

