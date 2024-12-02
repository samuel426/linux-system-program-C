/*===============================================================
[Program Name] : mipc.c
[Description]  : 
    - 공유 메모리와 프로세스 간 동기화 기법을 사용하여 부모와 자식 프로세스 간의 협력적 카운터 증가 구현.
    - 부모와 자식은 교대로 공유 메모리에 값을 업데이트하며, 동기화를 위해 사용자 정의 함수(TELL_WAIT 등)를 사용.
[Input]        : 
    없음 (프로그램 실행 시 동작)
[Output]       : 
    - 부모와 자식 프로세스의 카운터 값 출력.
    - 공유 메모리 업데이트 및 동작 상태 출력.
[Calls]        : 
    - shmget(), shmat(), shmdt(), shmctl(), perror(), fork(), TELL_WAIT(), TELL_CHILD(), WAIT_PARENT(), WAIT_CHILD()
[특기사항]     : 
    - 공유 메모리는 `IPC_PRIVATE` 키를 사용해 생성.
    - 프로세스 간 동기화를 위해 synclib.h에서 정의된 함수 사용.
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include "synclib.h"

#define NLOOPS  10             // 루프 반복 횟수
#define SIZE    sizeof(long)   // 공유 메모리 크기 (long 타입 크기)

// Counter 업데이트 함수
/*===============================================================
[Function Name] : int update(long *ptr)
[Description]   : 
    - 공유 메모리에 저장된 카운터 값을 증가시키고 반환.
[Input]         : 
    long *ptr; // 공유 메모리 내 카운터 값의 포인터
[Output]        : 
    - 증가된 카운터 값 반환.
[Call By]       : 
    - main()
[Calls]         : 없음
[Given]         : 
    - 공유 메모리에 접근 가능한 포인터
[Returns]       : 
    - 증가된 카운터 값
==================================================================*/
int update(long *ptr) {
    return ((*ptr)++);
}

int main() {
    int shmid, i, counter;    // 공유 메모리 식별자, 루프 인덱스, 카운터 값
    pid_t pid;               // 프로세스 ID
    long *shared_mem;        // 공유 메모리 포인터

    // 공유 메모리 생성
    if ((shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | 0666)) == -1) {
        perror("shmget");
        exit(1);
    }

    // 공유 메모리에 연결
    if ((shared_mem = (long *)shmat(shmid, NULL, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // 초기화
    *shared_mem = 0;

    TELL_WAIT(); // 동기화를 위한 초기화
    if ((pid = fork()) < 0) { // 자식 프로세스 생성
        perror("fork");
        exit(1);
    } else if (pid > 0) { // 부모 프로세스
        for (i = 0; i < NLOOPS; i += 2) {
            if ((counter = update(shared_mem)) != i) {
                fprintf(stderr, "Counter mismatch\n");
                exit(1);
            }

            printf("Parent: counter=%d\n", counter);
            TELL_CHILD(); // 자식에게 신호 전달
            WAIT_CHILD(); // 자식의 신호 대기
        }

        // 공유 메모리 해제
        if (shmdt(shared_mem) == -1) {
            perror("shmdt");
            exit(1);
        }

        // 공유 메모리 제거
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl");
            exit(1);
        }
    } else { // 자식 프로세스
        for (i = 1; i < NLOOPS; i += 2) {
            WAIT_PARENT(); // 부모의 신호 대기
            if ((counter = update(shared_mem)) != i) {
                fprintf(stderr, "Counter mismatch\n");
                exit(1);
            }
            printf("Child : counter=%d\n", counter);
            TELL_PARENT(); // 부모에게 신호 전달
        }

        // 자식도 공유 메모리에서 분리
        if (shmdt(shared_mem) == -1) {
            perror("shmdt");
            exit(1);
        }
    }

    return 0;
}

