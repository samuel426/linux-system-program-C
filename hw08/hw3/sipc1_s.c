/*===============================================================
[Program Name] : sipc1.c
[Description]  :
    - 멀티스레드 환경에서 클라이언트 요청을 처리하고 응답을 생성.
    - 전역 변수를 사용해 메시지를 주고받으며, System V 세마포어를 통해 동기화.
    - SIGINT 신호(Ctrl+C)를 처리하여 서버 종료 시 리소스를 안전하게 정리.
[Input]        :
    - 전역 변수 `message`에 저장된 클라이언트 요청.
[Output]       :
    - 전역 변수 `message`에 저장된 응답 메시지.
    - 요청 및 응답 내용을 콘솔에 출력.
[Calls]        :
    - semInit(), semWait(), semPost(), semDestroy(), signal()
[특기사항]     :
    - System V 세마포어를 사용하여 요청-응답의 동기화를 관리.
    - SIGINT 신호를 처리하여 서버 종료 시 세마포어를 정리.
==================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>   // getpid
#include <sys/ipc.h>
#include <sys/sem.h>
#include "shm.h"

#define BUFFER_SIZE 1024

char message[BUFFER_SIZE]; // 메시지를 저장하는 전역 변수
int semid = -1;            // 세마포어 식별자
int running = 1;           // 서버 실행 상태 플래그

// 세마포어 연산을 위한 설정
struct sembuf sem_op = {0, -1, SEM_UNDO}; // 세마포어 기다리기

// SIGINT 신호 핸들러
void handle_sigint(int sig) {
    printf("\nServer: Received SIGINT. Shutting down...\n");
    running = 0;
}

void cleanup() {
    // 세마포어 제거
    if (semid != -1) {
        if (semctl(semid, 0, IPC_RMID) < 0) {
            perror("semctl");
        } else {
            printf("Semaphore destroyed.\n");
        }
    }
}

void *serverThread(void *arg) {
    while (running) {
        // 요청 대기
        if (semop(semid, &sem_op, 1) < 0) {
            if (!running) break; // SIGINT 발생 시 루프 종료
            perror("semop");
            cleanup();
            exit(1);
        }

        if (!running) break; // 실행 상태 확인

        printf("Server: Received request: %s\n", message);

        // 응답 작성
        snprintf(message, BUFFER_SIZE, "Reply from server: %d", getpid());

        // 응답 완료 신호
        sem_op.sem_op = 1; // 세마포어 증가
        if (semop(semid, &sem_op, 1) < 0) {
            perror("semop");
            cleanup();
            exit(1);
        }
    }

    return NULL;
}

int main() {
    pthread_t tid;

    // SIGINT 신호 핸들러 설정
    signal(SIGINT, handle_sigint);

    // 세마포어 초기화
    key_t key = SHM_KEY;  // SHM_KEY는 공유 메모리 키와 동일한 키를 사용
    if ((semid = semget(key, 1, IPC_CREAT | 0666)) < 0) {
        perror("semget");
        exit(1);
    }

    // 세마포어 값 초기화
    if (semctl(semid, 0, SETVAL, 0) < 0) {
        perror("semctl");
        exit(1);
    }

    // 서버 스레드 생성
    if (pthread_create(&tid, NULL, serverThread, NULL) != 0) {
        perror("pthread_create");
        cleanup();
        exit(1);
    }

    // 서버 종료 대기
    pthread_join(tid, NULL);

    cleanup(); // 리소스 정리
    return 0;
}

