/*===============================================================
[Program Name] : producer.c
[Description]  : 
    - 생산자 프로세스가 공유 메모리를 사용하여 데이터를 생성하고 버퍼에 추가.
    - 공유 메모리를 직접 접근하여 동기화 없이 버퍼 상태를 확인하며 작업 수행.
[Input]        :
    공유 메모리
    int shmid;                // 공유 메모리 식별자
    BoundedBufferType *pBuf;  // 공유 버퍼 구조체 포인터
[Output]       :
    생성된 데이터 상태 출력, 버퍼 상태 출력.
[Calls]        :
    shmget(), shmat(), shmctl()
[특기사항]     : 
    - 생산자는 버퍼가 꽉 차면 대기.
    - 동기화를 사용하지 않아 상태 확인 시 충돌 가능성이 있음.
==================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "prodcons.h"

int main()
{
    BoundedBufferType *pBuf;
    int shmid, i, data;

    // 공유 메모리 생성 및 연결
    if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    srand(0x8888);
    for (i = 0; i < NLOOPS; i++) {
        // 버퍼가 꽉 차면 대기
        if (pBuf->counter == MAX_BUF) {
            printf("Producer: Buffer full. Waiting.....\n");
            while (pBuf->counter == MAX_BUF)
                ;
        }

        // 데이터 생성 및 버퍼에 추가
        printf("Producer: Producing an item.....\n");
        data = (rand() % 100) * 10000;
        pBuf->buf[pBuf->in].data = data;
        pBuf->in = (pBuf->in + 1) % MAX_BUF;
        pBuf->counter++;

        usleep(data);
    }

    printf("Producer: Produced %d items.....\n", i);

    sleep(2);
    printf("Producer: %d items in buffer.....\n", pBuf->counter);

    // 공유 메모리 해제
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        perror("shmctl");
        exit(1);
    }

    return 0;
}

