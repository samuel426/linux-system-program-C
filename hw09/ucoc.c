/*===============================================================
[Program Name] : ucoc.c
[Description]  : 
    - UNIX 도메인 소켓을 이용한 연결 지향형 클라이언트를 구현하여 
      서버에 요청 메시지를 전송하고 응답을 수신한다.
[Input]        : 
    - 서버의 UNIX 도메인 소켓 경로 (UNIX_STR_PATH)
    - 클라이언트 프로세스 ID를 포함한 요청 메시지
[Output]       : 
    - 서버로부터 받은 응답 메시지
    - "Sent a request....." 및 "Received reply: <메시지>"를 콘솔에 출력
[Calls]        : 
    - socket(), connect(), write(), read(), close()
[특기사항]     : 
    - "unix.h" 파일에 MsgType, UNIX_STR_PATH 등의 정의가 필요
    - 클라이언트는 서버의 UNIX 도메인 소켓에 연결을 시도
    - 연결 성공 시 요청 메시지를 전송하고, 서버로부터 응답을 기다림
    - 응답을 받은 후 소켓을 닫고 프로그램을 종료
===============================================================*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include "unix.h"

int main(int argc, char *argv[]) 
{
    int                 sockfd, n, servAddrLen; // 소켓 파일 디스크립터, 읽은 바이트 수, 서버 주소 길이
    struct sockaddr_un  servAddr;                // 서버 주소 구조체
    MsgType             msg;                     // 메시지 구조체 (unix.h에서 정의됨)

    // UNIX 도메인 소켓 생성
    if ((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // 서버 주소 구조체 초기화 및 설정
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sun_family = PF_UNIX;
    strcpy(servAddr.sun_path, UNIX_STR_PATH); // 서버 소켓 경로 설정
    servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

    // 서버에 연결 시도
    if (connect(sockfd, (struct sockaddr *) &servAddr, servAddrLen) < 0)  {
        perror("connect");
        exit(1);
    }

    // 요청 메시지 작성
    msg.type = MSG_REQUEST;
    sprintf(msg.data, "This is a request from %d.", getpid());

    // 서버로 요청 메시지 전송
    if (write(sockfd, (char *)&msg, sizeof(msg)) < 0)  {
        perror("write");
        exit(1);
    }
    printf("Sent a request.....");

    // 서버로부터 응답 메시지 수신
    if ((n = read(sockfd, (char *)&msg, sizeof(msg))) < 0)  {
        perror("read");
        exit(1);
    }
    printf("Received reply: %s\n", msg.data);

    // 소켓 닫기
    close(sockfd);
}

