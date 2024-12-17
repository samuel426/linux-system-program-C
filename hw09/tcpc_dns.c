/*===============================================================
[Program Name] : tcpc_dns.c
[Description]  : 
    - TCP 클라이언트를 구현하여 DNS를 사용하여 서버의 호스트 이름을 해석하고, 
      서버에 요청 메시지를 전송한 후 응답을 수신한다.
[Input]        : 
    - 명령줄 인수로 서버의 IP 주소 또는 호스트 이름을 입력받음
    - 클라이언트 프로세스 ID를 포함한 요청 메시지
[Output]       : 
    - 서버로부터 받은 응답 메시지
    - "Sent a request....." 및 "Received reply: <메시지>"를 콘솔에 출력
[Calls]        : 
    - socket(), connect(), write(), read(), close(), gethostbyname(), inet_addr(), memcpy()
[특기사항]     : 
    - "tcp.h" 파일에 MsgType과 SERV_TCP_PORT 등의 정의가 필요
    - 서버의 호스트 이름을 입력받아 IP 주소로 변환할 수 있음
    - IP 주소가 아닌 호스트 이름을 입력받을 경우 DNS를 통해 해석
    - 오류 발생 시 적절한 메시지를 출력하고 프로그램을 종료
===============================================================*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "tcp.h"

int main(int argc, char *argv[])
{
    int                 sockfd, n;                 // 소켓 파일 디스크립터, 읽은 바이트 수
    struct sockaddr_in  servAddr;                  // 서버 주소 구조체
    MsgType             msg;                       // 메시지 구조체 (tcp.h에서 정의됨)
    struct hostent      *hp;                       // 호스트 엔티티 구조체

    // 명령줄 인수가 정확히 2개가 아닐 경우 사용법 출력 후 종료
    if (argc != 2)  {
        fprintf(stderr, "Usage: %s IPaddress\n", argv[0]);
        exit(1);
    }

    // TCP 소켓 생성
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // 서버 주소 구조체 초기화 및 설정
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_port = htons(SERV_TCP_PORT); // 서버 포트 번호 설정

    // 입력된 인수가 숫자로 시작하면 IP 주소로 간주하고 변환
    if (isdigit(argv[1][0]))  {
        servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    }
    else  {
        // 호스트 이름을 IP 주소로 해석
        if ((hp = gethostbyname(argv[1])) == NULL)  {
            fprintf(stderr, "Unknown host: %s\n", argv[1]);
            exit(1);
        }
        memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length);
    }

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

