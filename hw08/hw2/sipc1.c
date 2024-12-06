/*===============================================================
[Program Name] : sipc1.c
[Description]  :
    - 공유 메모리를 사용하여 클라이언트의 요청을 처리하고, 응답을 전달하는 서버.
    - 세마포어를 통해 동기화를 구현하여 busy waiting 제거.
    - SIGINT 신호(Ctrl+C)를 처리하여 서버 종료를 지원.
[Input]        :
    - 공유 메모리에 저장된 요청 메시지.
[Output]       :
    - 공유 메모리에 응답 메시지 저장.
    - 요청 및 응답 내용을 콘솔에 출력.
[Calls]        :
    - shmget(), shmat(), shmdt(), shmctl(), semWait(), semPost(), semDestroy(), perror(), exit(), signal()
[특기사항]     :
    - System V 세마포어를 사용하여 동기화.
    - SIGINT 신호를 처리하여 서버를 안전하게 종료.
    - 공유 메모리와 세마포어는 `shm.h`와 `semlib.h`에 정의.
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <signal.h>
#include "shm.h"
#include "semlib.h"

int running = 1; // 서버 실행 상태 플래그
int shmid = -1, semid = -1; // 공유 메모리 및 세마포어 식별자
char *ptr = NULL; // 공유 메모리 포인터

// SIGINT 신호 핸들러
void handle_sigint(int sig) {
    printf("\nServer: Received SIGINT. Shutting down...\n");
    running = 0; // 실행 상태를 종료로 변경
}

void cleanup() {
    // 공유 메모리 분리 및 제거
    if (ptr != NULL) {
        if (shmdt(ptr) < 0) {
            perror("shmdt");
        } else {
            printf("Shared memory detached.\n");
        }
    }
    if (shmid != -1) {
        if (shmctl(shmid, IPC_RMID, NULL) < 0) {
            perror("shmctl");
        } else {
            printf("Shared memory removed.\n");
        }
    }

    // 세마포어 제거
    if (semid != -1) {
        if (semDestroy(semid) < 0) {
            perror("semDestroy");
        } else {
            printf("Semaphore destroyed.\n");
        }
    }
}

int main() {
    char *pData; // 공유 메모리 데이터 포인터

    // SIGINT 신호 핸들러 설정
    signal(SIGINT, handle_sigint);

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
        cleanup();
        exit(1);
    }

    pData = ptr; // 공유 메모리 전체를 데이터로 사용

    while (running) {
        // 요청 대기
        if (semWait(semid) < 0) {
            if (running == 0) break; // SIGINT 발생 시 루프 종료
            perror("semWait");
            cleanup();
            exit(1);
        }

        if (!running) break; // 실행 상태 확인

        printf("Received request: %s.....", pData);

        // 응답 메시지 작성
        sprintf(pData, "This is a reply from %d.", getpid());
        printf("Replied.\n");

        // 응답 완료 신호
        if (semPost(semid) < 0) {
            perror("semPost");
            cleanup();
            exit(1);
        }
    }

    printf("Server: Cleaning up resources...\n");
    cleanup();
    return 0;
}

