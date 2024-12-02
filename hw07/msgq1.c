/*===============================================================
[Program Name] : msgq1.c
[Description]  : 
    - 메시지 큐(System V IPC)를 사용하여 클라이언트 요청을 처리하고 응답을 보내는 프로그램.
    - 메시지 큐 생성, 메시지 수신 및 전송, 메시지 큐 제거를 포함.
[Input]        : 
    MsgType msg; // 메시지 큐를 통해 수신한 요청 메시지 구조체
[Output]       : 
    - 메시지 큐를 통해 응답 메시지를 전송
    - 수신 및 전송된 메시지를 콘솔에 출력
[Calls]        : 
    - msgget(), msgrcv(), msgsnd(), msgctl(), perror(), exit()
[특기사항]     : 
    - 메시지 큐 키는 `MSG_KEY`로 정의(0x8000 + MY_ID).
    - 메시지 큐 생성 시 `MSG_MODE`를 사용하여 읽기/쓰기 권한 설정.
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg.h"
#include <stdlib.h>

#define MSG_MODE (0600 | IPC_CREAT) // 메시지 큐 생성 모드

int main() {
    int msqid, n;       // 메시지 큐 ID와 메시지 수신 바이트 수
    MsgType msg;        // 메시지 구조체

    // 메시지 큐 생성 또는 연결
    if ((msqid = msgget(MSG_KEY, MSG_MODE)) < 0) {
        perror("msgget"); // 메시지 큐 생성 실패 시 에러 출력
        exit(1);
    }

    // 메시지 큐에서 메시지 수신
    if ((n = msgrcv(msqid, (char *)&msg, sizeof(msg), 0, 0)) < 0) {
        perror("msgrcv"); // 메시지 수신 실패 시 에러 출력
        exit(1);
    }
    printf("Received request: %s.....", msg.data);

    // 응답 메시지 설정 및 전송
    msg.type = MSG_REPLY; // 응답 메시지 타입 설정
    sprintf(msg.data, "This is a reply from %d.", getpid()); // 응답 메시지 내용 작성
    if (msgsnd(msqid, (char *)&msg, sizeof(msg), 0) < 0) {
        perror("msgsnd"); // 메시지 전송 실패 시 에러 출력
        exit(1);
    }
    printf("Replied.\n");

    sleep(1); // 메시지 큐 삭제 전 대기

    // 메시지 큐 제거
    if (msgctl(msqid, IPC_RMID, 0) < 0) {
        perror("msgctl"); // 메시지 큐 제거 실패 시 에러 출력
        exit(1);
    }

    return 0;
}

