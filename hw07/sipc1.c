/*===============================================================
[Program Name] : sipc1.c
[Description]  : 
    - 공유 메모리를 사용하여 프로세스 간의 통신을 구현.
    - 클라이언트의 요청을 공유 메모리를 통해 수신하고, 응답을 공유 메모리에 저장.
[Input]        : 
    - 공유 메모리에서 읽어오는 요청 메시지.
    - 공유 메모리의 정수 값을 통해 요청의 상태를 확인.
[Output]       : 
    - 공유 메모리에 응답 메시지를 기록.
    - 요청과 응답 내용을 콘솔에 출력.
[Calls]        : 
    - shmget(), shmat(), shmdt(), shmctl(), perror(), exit()
[특기사항]     : 
    - 공유 메모리 키와 크기는 `shm.h`에 정의(`SHM_KEY`, `SHM_SIZE`).
    - 공유 메모리를 통해 요청 상태를 확인하기 위해 정수 값을 사용.
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

    pInt = (int *)ptr; // 공유 메모리의 첫 번째 부분을 상태 확인용 정수로 사용
    while ((*pInt) == 0) // 클라이언트 요청 대기
        ;

    pData = ptr + sizeof(int); // 공유 메모리의 두 번째 부분을 데이터로 사용
    printf("Received request: %s.....", pData);

    // 응답 메시지 작성 및 상태 초기화
    sprintf(pData, "This is a reply from %d.", getpid()); // 응답 메시지 작성
    *pInt = 0; // 상태 초기화
    printf("Replied.\n");

    sleep(1); // 응답 전송 후 대기

    /* Detach shared memory */
    // 공유 메모리 분리
    if (shmdt(ptr) < 0) {
        perror("shmdt"); // 공유 메모리 분리 실패 시 에러 출력
        exit(1);
    }

    /* Remove shared memory */
    // 공유 메모리 제거
    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        perror("shmctl"); // 공유 메모리 제거 실패 시 에러 출력
        exit(1);
    }

    return 0;
}

