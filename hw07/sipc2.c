/*===============================================================
[Program Name] : sipc2.c
[Description]  : 
    - 공유 메모리를 사용하여 서버로 요청을 보내고, 응답을 수신하는 클라이언트 프로그램.
    - 요청 메시지를 공유 메모리에 저장하고 상태 값을 업데이트.
    - 서버의 응답을 공유 메모리에서 읽어 콘솔에 출력.
[Input]        : 
    MsgType msg; // 공유 메모리를 통해 전송된 요청 메시지
[Output]       : 
    - 공유 메모리를 통해 수신된 응답 메시지.
    - 응답 메시지를 콘솔에 출력.
[Calls]        : 
    - shmget(), shmat(), shmdt(), perror(), exit()
[특기사항]     : 
    - 공유 메모리 키와 크기는 `shm.h`에 정의(`SHM_KEY`, `SHM_SIZE`).
    - 공유 메모리의 첫 번째 정수 값을 사용해 요청 및 응답 상태를 제어.
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"
#include <stdlib.h>

int main() {
    int shmid;          // 공유 메모리 식별자
    char *ptr, *pData;  // 공유 메모리 데이터 포인터
    int *pInt;          // 공유 메모리 상태 확인용 정수 포인터

    /* SHM_KEY, SHM_SIZE, SHM_MODE in shm.h */
    // 공유 메모리 생성 또는 연결
    if ((shmid = shmget(SHM_KEY, SHM_SIZE, SHM_MODE)) < 0) {
        perror("shmget"); // 공유 메모리 생성 실패 시 에러 출력
        exit(1);
    }

    // 공유 메모리 연결
    if ((ptr = shmat(shmid, 0, 0)) == (void *)-1) {
        perror("shmat"); // 공유 메모리 연결 실패 시 에러 출력
        exit(1);
    }

    pInt = (int *)ptr;           // 상태 값 저장 위치
    pData = ptr + sizeof(int);   // 메시지 저장 위치

    // 요청 메시지 작성 및 상태 업데이트
    sprintf(pData, "This is a request from %d.", getpid()); // 요청 메시지 작성
    *pInt = 1; // 요청 상태 값 설정
    printf("Sent a request.....");

    // 서버의 응답 대기
    while ((*pInt) == 1)
        ; // 상태 값이 변경될 때까지 대기

    // 응답 메시지 출력
    printf("Received reply: %s\n", pData);

    /* Detach shared memory */
    // 공유 메모리 분리
    if (shmdt(ptr) < 0) {
        perror("shmdt"); // 공유 메모리 분리 실패 시 에러 출력
        exit(1);
    }

    return 0;
}

