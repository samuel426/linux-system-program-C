
/*===============================================================
[Program Name] : shm.c
[Description]  :
    - 공유 메모리(System V Shared Memory)를 생성하고 연결하여 사용.
    - 프로그램의 데이터 섹션, 힙 영역, 스택 영역, 공유 메모리의 메모리 주소를 출력.
[Input]        :
    없음 (프로그램 실행 시 동작)
[Output]       :
    - 데이터 섹션, 힙, 스택, 공유 메모리의 메모리 주소 출력.
[Calls]        :
    - shmget(), shmat(), shmdt(), shmctl(), malloc(), perror(), exit()
[특기사항]     :
    - 공유 메모리는 `IPC_PRIVATE` 키를 사용하여 생성.
    - 메모리 영역 주소 범위를 출력하여 메모리 구조를 확인.
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

#define ARRAY_SIZE 40000         // Array 크기
#define MALLOC_SIZE 100000       // 동적 할당 크기
#define SHM_SIZE 100000          // 공유 메모리 크기
#define SHM_MODE (SHM_R | SHM_W) // 공유 메모리 읽기/쓰기 모드

char Array[ARRAY_SIZE]; // 데이터 섹션에 위치한 정적 배열

int main() {
    int shmid;          // 공유 메모리 식별자
    char *ptr, *shmptr; // 동적 메모리 및 공유 메모리 포인터

    // 힙 영역에서 메모리 동적 할당
    if ((ptr = (char *)malloc(MALLOC_SIZE)) == NULL) {
        perror("malloc"); // malloc 실패 시 에러 출력
        exit(1);
    }

    // 공유 메모리 생성
    if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE)) < 0) {
        perror("shmget"); // 공유 메모리 생성 실패 시 에러 출력
        exit(1);
    }

    // 공유 메모리 연결
    if ((shmptr = shmat(shmid, 0, 0)) == (void *)-1) {
        perror("shmat"); // 공유 메모리 연결 실패 시 에러 출력
        exit(1);
    }

    // 메모리 주소 출력
    printf("Array[] from %p to %p\n", &Array[0], &Array[ARRAY_SIZE]);
    printf("Stack around %p\n", &shmid);
    printf("Malloced from %p to %p\n", ptr, ptr + MALLOC_SIZE);
    printf("Shared memory attached from %p to %p\n", shmptr, shmptr + SHM_SIZE);

    // 공유 메모리 분리
    if (shmdt(shmptr) < 0) {
        perror("shmdt"); // 공유 메모리 분리 실패 시 에러 출력
        exit(1);
    }

    // 공유 메모리 제거
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        perror("shmctl"); // 공유 메모리 제거 실패 시 에러 출력
        exit(1);
    }

    return 0;
}

