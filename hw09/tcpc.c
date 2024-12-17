/*===============================================================
[Program Name] : tcpc.c
[Description]  : 
    - TCP 클라이언트를 구현하여 서버에 연결하고, 요청 메시지를 전송한 후 응답을 수신한다.
[Input]        : 
    - 서버의 호스트 주소 (SERV_HOST_ADDR)
    - 서버의 포트 번호 (SERV_TCP_PORT)
    - 클라이언트 프로세스 ID를 포함한 요청 메시지
[Output]       : 
    - 서버로부터 받은 응답 메시지
    - "Sent a request....." 및 "Received reply: <메시지>"를 콘솔에 출력
[Calls]        : 
    - socket(), connect(), write(), read(), close()
[특기사항]     : 
    - "tcp.h" 파일에 MsgType, SERV_HOST_ADDR, SERV_TCP_PORT 등의 정의가 필요
    - 클라이언트는 서버에 연결을 시도하며, 연결 실패 시 오류 메시지를 출력하고 종료
    - 요청 및 응답 메시지는 MsgType 구조체를 사용하여 전송
================================================================*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcp.h"

int main(int argc, char *argv[]) 
{
    int                 sockfd, n;
    struct sockaddr_in  servAddr;
    MsgType             msg;

    // 소켓 생성
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // 서버 주소 구조체 초기화 및 설정
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR); // 서버 호스트 주소 설정
    servAddr.sin_port = htons(SERV_TCP_PORT);             // 서버 포트 번호 설정

    // 서버에 연결 시도
    if (connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
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

