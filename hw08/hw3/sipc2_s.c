/*===============================================================
[Program Name] : sipc2.c
[Description]  :
    - 멀티스레드 환경에서 서버로 요청을 보내고 응답을 수신하는 클라이언트.
    - 전역 변수를 사용해 메시지를 주고받으며, 세마포어를 통해 동기화.
[Input]        :
    - 클라이언트 요청 메시지.
[Output]       :
    - 서버로부터 수신한 응답 메시지.
[Calls]        :
    - semWait(), semPost()
==================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // getpid
#include <sys/ipc.h>
#include <sys/sem.h>
#include "shm.h"

#define BUFFER_SIZE 1024

char message[BUFFER_SIZE];
int semid = -1;

struct sembuf sem_op = {0, -1, SEM_UNDO}; // 세마포어 대기 설정

int main() {
    // 세마포어 초기화 (서버와 동일한 세마포어 키 사용)
    key_t key = SHM_KEY;
    if ((semid = semget(key, 1, IPC_CREAT | 0666)) < 0) {
        perror("semget");
        exit(1);
    }

    // 요청 작성
    snprintf(message, BUFFER_SIZE, "Request from client: %d", getpid());
    printf("Client: Sent request: %s\n", message);

    // 요청 신호 전송
    sem_op.sem_op = 1; // 세마포어 증가
    if (semop(semid, &sem_op, 1) < 0) {
        perror("semop");
        exit(1);
    }

    // 응답 대기
    sem_op.sem_op = -1; // 세마포어 대기
    if (semop(semid, &sem_op, 1) < 0) {
        perror("semop");
        exit(1);
    }

    // 응답 출력
    printf("Client: Received reply: %s\n", message);

    // 응답 완료 신호
    sem_op.sem_op = 1; // 세마포어 증가
    if (semop(semid, &sem_op, 1) < 0) {
        perror("semop");
        exit(1);
    }

    return 0;
}

