/*===============================================================
[Program Name] : sgs.c
[Description]  : 
    - Scatter/Gather TCP 서버를 구현하여 클라이언트의 요청을 
      헤더(HeaderType)와 메시지(MsgType)로 받아들이고, 
      응답을 동일한 구조로 전송한다.
[Input]        : 
    - 클라이언트의 요청 메시지와 헤더 정보
    - 서버는 클라이언트의 주소 정보를 사용하여 응답을 전송
[Output]       : 
    - 클라이언트로 응답 메시지와 헤더 정보 전송
    - "Scatter/Gather TCP Server started....." 및 각 요청 처리 상태 메시지를 콘솔에 출력
[Calls]        : 
    - socket(), bind(), listen(), accept(), readv(), writev(), close(), signal(), perror(), exit(), strcpy(), sprintf()
[특기사항]     : 
    - "sg.h" 파일에 MsgType, HeaderType, SERV_TCP_PORT 등의 정의가 필요
    - Scatter/Gather I/O를 사용하여 헤더와 메시지를 효율적으로 처리
    - 서버 종료를 위해 SIGINT 시그널(Ctrl+C) 처리
    - 다중 클라이언트 요청을 처리할 수 있도록 연결 지향형 소켓을 사용
    - 오류 발생 시 perror를 통해 에러 메시지를 출력하고 프로그램을 종료
===============================================================*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include "sg.h"


int	Sockfd; // 서버 소켓 파일 디스크립터

/*===============================================================
[Function Name] : CloseServer
[Description]   : 
    - 서버 소켓을 닫고 종료 메시지를 출력하며 프로그램을 종료한다.
[Input]         : 
    - 없음
[Output]        : 
    - "Scatter/Gather TCP Server exit....." 메시지 출력
[Call By]       : 
    - SIGINT 시그널 핸들러
[Calls]         : 
    - close(), printf(), exit()
[Given]         : 
    - 글로벌 변수 Sockfd가 유효한 소켓 디스크립터를 가리키고 있다고 가정
[Returns]       : 
    - 없음 (프로그램 종료)
===============================================================*/
void
CloseServer()
{
    close(Sockfd); // 서버 소켓 닫기

    printf("\nScatter/Gather TCP Server exit.....\n");
    exit(0); // 프로그램 종료
}

/*===============================================================
[Function Name] : main
[Description]   : 
    - 서버를 초기화하고 클라이언트의 요청을 처리하여 응답을 전송한다.
    - Scatter/Gather I/O를 사용하여 헤더와 메시지를 동시에 읽고 쓴다.
[Input]         : 
    - 없음
[Output]        : 
    - 클라이언트의 요청과 응답 상태를 콘솔에 출력
[Call By]       : 
    - 시스템에 의해 자동으로 호출
[Calls]         : 
    - signal(), socket(), bind(), listen(), accept(), readv(), writev(), close(), perror(), exit(), strcpy(), sprintf()
[Given]         : 
    - "sg.h" 파일에 MsgType, HeaderType, SERV_TCP_PORT 등이 정의되어 있어야 함
[Returns]       : 
    - 없음 (무한 루프 내에서 실행)
===============================================================*/
int main(int argc, char *argv[])
{
    int					newSockfd, cliAddrLen, n;
    struct sockaddr_in	cliAddr, servAddr;
    MsgType				msg;
    HeaderType			hdr;
    struct iovec		iov[2];

    // SIGINT 시그널 처리 등록 (Ctrl+C 시 CloseServer 호출)
    signal(SIGINT, CloseServer);

    // TCP 소켓 생성
    if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // 서버 주소 구조체 초기화 및 설정
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 인터페이스에서 연결 수락
    servAddr.sin_port = htons(SERV_TCP_PORT);    // 서버 포트 번호 설정

    // 소켓과 서버 주소 바인딩
    if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
        perror("bind");
        exit(1);
    }

    // 연결 요청 대기 상태로 설정
    listen(Sockfd, 5);

    printf("Scatter/Gather TCP Server started.....\n");

    cliAddrLen = sizeof(cliAddr);
    while (1)  {
        // 클라이언트 연결 수락
        newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen);
        if (newSockfd < 0)  {
            perror("accept");
            exit(1);
        }

        // 다중 버퍼 설정: 헤더와 메시지를 각각의 iovec에 할당
        iov[0].iov_base = (char *)&hdr;
        iov[0].iov_len = sizeof(hdr);
        iov[1].iov_base = (char *)&msg;
        iov[1].iov_len = sizeof(msg);

        // 클라이언트로부터 헤더와 메시지 읽기
        if ((n = readv(newSockfd, iov, 2)) < 0)  {
            perror("readv");
            exit(1);
        }
        printf("Received request: %s(%s).....", msg.data, hdr.info);

        // 응답 헤더와 메시지 작성
        strcpy(hdr.info, "REPLY");                         // 응답 헤더 정보 설정
        msg.type = MSG_REPLY;                              // 메시지 타입 설정
        sprintf(msg.data, "This is a reply from %d.", getpid()); // 응답 메시지 설정

        // 응답 헤더와 메시지를 클라이언트로 전송
        if (writev(newSockfd, iov, 2) < 0)  {
            perror("writev");
            exit(1);
        }
        printf("Replied.\n");

        // 클라이언트 소켓 닫기
        close(newSockfd);
    }
}

