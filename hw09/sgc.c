/*===============================================================
[Program Name] : sgc.c
[Description]  : 
    - TCP 클라이언트를 구현하여 서버에 연결하고, 
      헤더(HeaderType)와 메시지(MsgType)를 함께 전송한 후 응답을 수신한다.
[Input]        : 
    - 서버의 IP 주소 (SERV_HOST_ADDR)
    - 서버의 포트 번호 (SERV_TCP_PORT)
    - 클라이언트 프로세스 ID를 포함한 요청 메시지
[Output]       : 
    - 서버로부터 받은 응답 메시지와 헤더 정보
    - "Sent a request....." 및 "Received reply: <메시지>(<헤더 정보>)"를 콘솔에 출력
[Calls]        : 
    - socket(), connect(), writev(), readv(), close(), perror(), exit(), strcpy(), sprintf(), memcpy()
[특기사항]     : 
    - "sg.h" 파일에 MsgType, HeaderType, SERV_TCP_PORT, SERV_HOST_ADDR 등의 정의가 필요
    - 헤더와 메시지를 함께 전송하기 위해 writev와 readv 함수를 사용
    - 다중 버퍼 전송을 통해 데이터 구조의 효율적인 전송을 구현
    - 오류 발생 시 perror를 통해 에러 메시지를 출력하고 프로그램을 종료
===============================================================*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "sg.h"

int main(int argc, char *argv[]) 
{
    int                 sockfd, n;                  // 소켓 파일 디스크립터, 읽은 바이트 수
    struct sockaddr_in  servAddr;                   // 서버 주소 구조체
    MsgType             msg;                        // 메시지 구조체 (sg.h에서 정의됨)
    HeaderType          hdr;                        // 헤더 구조체 (sg.h에서 정의됨)
    struct iovec        iov[2];                     // 다중 버퍼를 위한 구조체 배열

    // TCP 소켓 생성
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // 서버 주소 구조체 초기화 및 설정
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR); // 서버 IP 주소 설정
    servAddr.sin_port = htons(SERV_TCP_PORT);             // 서버 포트 번호 설정

    // 서버에 연결 시도
    if (connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
        perror("connect");
        exit(1);
    }

    // 헤더와 메시지 작성
    strcpy(hdr.info, "REQST");                                // 헤더 정보 설정
    msg.type = MSG_REQUEST;                                   // 메시지 타입 설정
    sprintf(msg.data, "This is a request from %d.", getpid()); // 메시지 데이터 설정

    // 다중 버퍼 설정: 헤더와 메시지를 각각의 iovec에 할당
    iov[0].iov_base = (char *)&hdr;
    iov[0].iov_len = sizeof(hdr);
    iov[1].iov_base = (char *)&msg;
    iov[1].iov_len = sizeof(msg);

    // 헤더와 메시지를 함께 전송
    if (writev(sockfd, iov, 2) < 0)  {
        perror("writev");
        exit(1);
    }
    printf("Sent a request.....");

    // 응답을 위한 다중 버퍼 설정
    if ((n = readv(sockfd, iov, 2)) < 0)  {
        perror("readv");
        exit(1);
    }
    printf("Received reply: %s(%s)\n", msg.data, hdr.info);

    // 소켓 닫기
    close(sockfd);
}

