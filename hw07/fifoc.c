/*===============================================================
[Program Name] : fifoc.c
[Description]  : 
    - 클라이언트가 서버(FIFO)를 통해 요청을 보내고 응답을 받는 프로그램.
    - 클라이언트는 자신의 임시 FIFO를 생성하여 서버와 통신.
[Input]        : 
    MsgType msg; // 서버로 보낼 요청 메시지 구조체
[Output]       : 
    - 서버로 메시지를 전송하고, 서버의 응답을 콘솔에 출력.
[Calls]        : 
    - mkfifo(), open(), write(), read(), close(), remove()
[특기사항]     : 
    - 클라이언트는 자신의 PID 기반으로 임시 FIFO를 생성.
    - 서버와의 통신 후 임시 FIFO를 삭제하여 정리.
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "fifo.h"

int main() {
    char fname[MAX_FIFO_NAME]; // 클라이언트 임시 FIFO 이름
    int fd, sfd, n;            // FIFO 파일 디스크립터
    MsgType msg;               // 메시지 구조체

    // 클라이언트 임시 FIFO 생성
    sprintf(fname, ".fifo%d", getpid()); // FIFO 이름: ".fifo<클라이언트 PID>"
    if (mkfifo(fname, 0600) < 0) {
        perror("mkfifo"); // FIFO 생성 실패 시 에러 출력
        exit(1);
    }

    // 클라이언트의 FIFO 열기
    if ((fd = open(fname, O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    // 서버의 FIFO 열기
    if ((sfd = open(SERV_FIFO, O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    // 메시지 초기화 및 서버로 요청 전송
    strcpy(msg.returnFifo, fname);                  // 클라이언트의 임시 FIFO 이름 저장
    sprintf(msg.data, "This is a request from %d.", getpid()); // 요청 메시지 작성
    write(sfd, (char *)&msg, sizeof(msg));          // 서버 FIFO로 메시지 전송
    printf("Sent a request.....");

    // 서버로부터 응답 읽기
    if ((n = read(fd, (char *)&msg, sizeof(msg))) < 0) {
        perror("read");
        exit(1);
    }

    // 응답 메시지 출력
    printf("Received reply: %s\n", msg.data);

    // FIFO 파일 닫기
    close(fd);
    close(sfd);

    // 임시 FIFO 삭제
    if (remove(fname) < 0) {
        perror("remove");
        exit(1);
    }

    return 0;
}

