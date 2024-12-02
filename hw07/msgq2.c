/*===============================================================
[Program Name] : msgq2.c
[Description]  : 
    - 메시지 큐(System V IPC)를 사용하여 서버에 요청을 보내고, 응답을 수신하는 클라이언트 프로그램.
    - 메시지 큐에 요청 메시지를 전송하고, 응답 메시지를 수신.
[Input]        : 
    MsgType msg; // 클라이언트가 서버에 보낼 요청 메시지 구조체
[Output]       : 
    - 메시지 큐를 통해 응답 메시지를 수신.
    - 수신된 메시지를 콘솔에 출력.
[Calls]        : 
    - msgget(), msgsnd(), msgrcv(), perror(), exit()
[특기사항]     : 
    - 메시지 큐 키는 `MSG_KEY`로 정의(0x8000 + MY_ID).
    - 요청 메시지 타입은 `MSG_REQUEST`로 설정.
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg.h"
#include <stdlib.h>

#define MSG_MODE (0600) // 메시지 큐 권한 설정 (읽기/쓰기)

int main() {
    int msqid, n;       // 메시지 큐 ID와 메시지 전송/수신 바이트 수
    MsgType msg;        // 메시지 구조체

    // 메시지 큐에 연결
    if ((msqid = msgget(MSG_KEY, MSG_MODE)) < 0) {
        perror("msgget"); // 메시지 큐 연결 실패 시 에러 출력
        exit(1);
    }

    // 요청 메시지 작성 및 전송
    msg.type = MSG_REQUEST; // 요청 메시지 타입 설정
    sprintf(msg.data, "This is a request from %d.", getpid()); // 요청 메시지 작성
    if ((n = msgsnd(msqid, (char *)&msg, sizeof(msg), 0)) < 0) {
        perror("msgsnd"); // 메시지 전송 실패 시 에러 출력
        exit(1);
    }
    printf("Sent a request.....");

    // 응답 메시지 수신
    if ((n = msgrcv(msqid, (char *)&msg, sizeof(msg), 0, 0)) < 0) {
        perror("msgrcv"); // 메시지 수신 실패 시 에러 출력
        exit(1);
    }
    printf("Received reply: %s\n", msg.data);

    return 0;
}

