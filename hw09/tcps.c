/*===============================================================
[Program Name] : tcps.c
[Description]  : 
    - TCP 서버를 구현하여 클라이언트로부터 요청 메시지를 받고, 이에 대한 응답을 전송한다.
[Input]        : 
    - 클라이언트의 요청 메시지
    - 서버는 클라이언트의 연결 요청을 대기하며, 요청 메시지를 수신
[Output]       : 
    - 클라이언트로 응답 메시지를 전송
    - Received request와 Replied 메시지를 콘솔에 출력
[Calls]        : 
    - socket(), bind(), listen(), accept(), read(), write(), close()
[특기사항]     : 
    - 서버 종료를 위해 SIGINT 시그널(Ctrl+C) 처리
    - "tcp.h" 파일에 MsgType과 SERV_TCP_PORT 등의 정의가 필요
    - 서버는 INADDR_ANY를 사용하여 모든 인터페이스에서 연결을 수락
==================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "tcp.h"

int Sockfd; // 서버 소켓 파일 디스크립터

// SIGINT 시그널 처리 함수: 서버 종료
void CloseServer()
{
    close(Sockfd);
    printf("\nTCP Server exit.....\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    int newSockfd, cliAddrLen, n; // 클라이언트 소켓, 주소 길이, 읽은 바이트 수
    struct sockaddr_in cliAddr, servAddr; // 클라이언트 및 서버 주소 구조체
    MsgType msg; // 메시지 구조체 (tcp.h에서 정의됨)

    // SIGINT 시그널 처리 등록
    signal(SIGINT, CloseServer);

    // 소켓 생성
    if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // 서버 주소 구조체 초기화 및 설정
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 인터페이스에서 연결 수락
    servAddr.sin_port = htons(SERV_TCP_PORT);    // 서버 포트 번호 설정

    // 소켓과 주소 바인딩
    if (bind(Sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind");
        exit(1);
    }

    // 연결 요청 대기
    listen(Sockfd, 5);
    printf("TCP Server started.....\n");

    cliAddrLen = sizeof(cliAddr);
    while (1) {
        // 클라이언트 연결 수락
        newSockfd = accept(Sockfd, (struct sockaddr *)&cliAddr, &cliAddrLen);
        if (newSockfd < 0) {
            perror("accept");
            exit(1);
        }

        // 클라이언트로부터 메시지 읽기
        if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0) {
            perror("read");
            exit(1);
        }
        printf("Received request: %s.....", msg.data);

        // 응답 메시지 생성 및 전송
        msg.type = MSG_REPLY;
        sprintf(msg.data, "This is a reply from %d.", getpid());
        if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0) {
            perror("write");
            exit(1);
        }
        printf("Replied.\n");

        // 소켓 닫기
        usleep(10000); // 응답 후 짧은 대기
        close(newSockfd);
    }
}

