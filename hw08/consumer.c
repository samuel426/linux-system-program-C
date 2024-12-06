/*===============================================================
[Program Name] : consumer.c
[Description]  : 
    - 공유 메모리를 사용하여 생산자/소비자 문제를 해결하는 소비자 프로그램.
    - 공유 메모리에서 데이터를 읽고 소비하며, 버퍼가 비어있는 경우 대기.
[Input]        :
    공유 메모리 (SHM_KEY)
    int shmid;              // 공유 메모리 식별자
    BoundedBufferType *pBuf;// 공유 메모리를 가리키는 포인터
[Output]       :
    소비된 데이터 정보 출력
    프로그램 종료 시 버퍼 상태 및 소비된 아이템 개수 출력
[Calls]        :
    shmget(key_t key, size_t size, int shmflg), 
    shmat(int shmid, const void *shmaddr, int shmflg)
[특기사항]     : 
    - 공유 메모리가 미리 생성되어 있어야 하며, SHM_KEY 값은 고유해야 함.
    - 버퍼가 비어있을 경우 데이터를 소비할 수 없으므로 대기 상태 유지.
    - 작업 간 랜덤 대기 시간을 추가하여 처리 시간 분산.
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

    // 공유 메모리 초기화
    if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0)  {
        perror("shmget");
        exit(1);
    }
    if ((pBuf = (BoundedBufferType *)shmat(shmid, 0, 0)) == (void *) -1)  {
        perror("shmat");
        exit(1);
    }

    srand(0x9999);
    for (i = 0 ; i < NLOOPS ; i++)  {
        // 버퍼가 비어 있는 경우 대기
        if (pBuf->counter == 0)  {
            printf("Consumer: Buffer empty. Waiting.....\n");
            while (pBuf->counter == 0)
                ;
        }

        // 데이터 소비
        printf("Consumer: Consuming an item.....\n");
        data = pBuf->buf[pBuf->out].data;
        pBuf->out = (pBuf->out + 1) % MAX_BUF; // 순환 버퍼 처리
        pBuf->counter--;

        // 랜덤 대기 시간
        usleep((rand() % 100) * 10000);
    }

    // 최종 결과 출력
    printf("Consumer: Consumed %d items.....\n", i);
    printf("Consumer: %d items in buffer.....\n", pBuf->counter);
}

