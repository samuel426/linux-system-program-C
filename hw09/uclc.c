/*===============================================================
[Program Name] : uclc.c
[Description]  : 
    - UNIX 도메인 소켓을 이용한 비연결형 클라이언트를 구현하여 
      서버에 요청 메시지를 전송하고 응답을 수신한다.
[Input]        : 
    - 서버의 UNIX 도메인 소켓 경로 (UNIX_DG_PATH)
    - 클라이언트 프로세스 ID를 포함한 요청 메시지
[Output]       : 
    - 서버로부터 받은 응답 메시지
    - "Sent a request....." 및 "Received reply: <메시지>"를 콘솔에 출력
[Calls]        : 
    - socket(), bind(), sendto(), recvfrom(), write(), read(), close(), remove()
[특기사항]     : 
    - "unix.h" 파일에 MsgType과 UNIX_DG_PATH 등의 정의가 필요
    - 클라이언트는 고유한 소켓 경로를 생성하여 서버에 요청을 전송
    - 비연결형 프로토콜을 사용하여 데이터그램 단위로 통신
    - 통신 후 소켓 파일을 삭제하여 리소스 정리
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
    int                 sockfd, n, servAddrLen, myAddrLen, peerAddrLen; // 소켓 파일 디스크립터, 읽은 바이트 수, 서버 주소 길이, 내 주소 길이, 피어 주소 길이
    struct sockaddr_un  servAddr, myAddr, peerAddr; // 서버, 내, 피어 주소 구조체
    MsgType             msg; // 메시지 구조체 (unix.h에서 정의됨)

    // UNIX 도메인 소켓 생성
    if ((sockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // 클라이언트 주소 구조체 초기화 및 설정
    bzero((char *)&myAddr, sizeof(myAddr));
    myAddr.sun_family = PF_UNIX;
    sprintf(myAddr.sun_path, ".unix-%d", getpid()); // 고유한 소켓 파일 경로 생성
    myAddrLen = strlen(myAddr.sun_path) + sizeof(myAddr.sun_family);

    // 클라이언트 소켓 바인딩
    if (bind(sockfd, (struct sockaddr *)&myAddr, myAddrLen) < 0)  {
        perror("bind");
        exit(1);
    }

    // 서버 주소 구조체 초기화 및 설정
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sun_family = PF_UNIX;
    strcpy(servAddr.sun_path, UNIX_DG_PATH); // 서버 소켓 경로 설정
    servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

    // 요청 메시지 작성
    msg.type = MSG_REQUEST;
    sprintf(msg.data, "This is a request from %d.", getpid());

    // 서버로 요청 메시지 전송
    if (sendto(sockfd, (char *)&msg, sizeof(msg), 
            0, (struct sockaddr *)&servAddr, servAddrLen) < 0)  {
        perror("sendto");
        exit(1);
    }
    printf("Sent a request.....");

    // 서버로부터 응답 메시지 수신
    peerAddrLen = sizeof(peerAddr);
    if ((n = recvfrom(sockfd, (char *)&msg, sizeof(msg),
                0, (struct sockaddr *)&peerAddr, &peerAddrLen)) < 0)  {
        perror("recvfrom");
        exit(1);
    }
    printf("Received reply: %s\n", msg.data);

    // 소켓 닫기 및 소켓 파일 삭제
    close(sockfd);
    if (remove(myAddr.sun_path) < 0)  {
        perror("remove");
        exit(1);
    }
}

