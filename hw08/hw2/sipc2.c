/*===============================================================
[Program Name] : sipc2.c
[Description]  : 
    - 공유 메모리를 사용하여 서버로 요청을 보내고, 응답을 수신하는 클라이언트.
    - 세마포어를 통해 동기화를 구현하여 busy waiting 제거.
[Input]        : 
    - 요청 메시지 (프로세스 ID를 포함).
[Output]       : 
    - 서버로부터 수신한 응답 메시지를 콘솔에 출력.
[Calls]        : 
    - shmget(), shmat(), shmdt(), semWait(), semPost(), perror(), exit()
[특기사항]     : 
    - System V 세마포어를 사용하여 동기화.
    - 공유 메모리와 세마포어는 `shm.h`와 `semlib.h`에 정의.
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include "shm.h"
#include "semlib.h"

int main() {
    int shmid, semid;       // 공유 메모리 식별자 및 세마포어 식별자
    char *ptr, *pData;      // 공유 메모리 데이터 포인터

    // 공유 메모리 생성 또는 연결
    if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0) {
        perror("shmget");
        exit(1);
    }

    // 공유 메모리 연결
    if ((ptr = shmat(shmid, 0, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // 세마포어 초기화
    if ((semid = semInit(SHM_KEY)) < 0) {
        fprintf(stderr, "semInit failure\n");
        exit(1);
    }

    pData = ptr; // 공유 메모리 전체를 데이터로 사용

    // 요청 메시지 작성
    sprintf(pData, "This is a request from %d.", getpid());
    printf("Sent a request.....");

    // 요청 완료 신호
    if (semPost(semid) < 0) {
        fprintf(stderr, "semPost failure\n");
        exit(1);
    }

    // 응답 대기
    if (semWait(semid) < 0) {
        fprintf(stderr, "semWait failure\n");
        exit(1);
    }

    // 응답 메시지 출력
    printf("Received reply: %s\n", pData);

    // 공유 메모리 분리
    if (shmdt(ptr) < 0) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}

