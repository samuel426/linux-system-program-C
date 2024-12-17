/*===============================================================
[Program Name] : udpc.c
[Description]  : 
    - UDP 클라이언트를 구현하여 서버에 요청 메시지를 전송하고, 
      서버로부터의 응답을 수신하여 출력한다.
[Input]        : 
    - 서버의 호스트 주소 (SERV_HOST_ADDR)
    - 서버의 포트 번호 (SERV_UDP_PORT)
    - 클라이언트 프로세스 ID를 포함한 요청 메시지
[Output]       : 
    - 서버로부터 받은 응답 메시지
    - "Sent a request....." 및 "Received reply: <메시지>"를 콘솔에 출력
[Calls]        : 
    - socket(), sendto(), recvfrom(), close()
[특기사항]     : 
    - "udp.h" 파일에 MsgType, SERV_HOST_ADDR, SERV_UDP_PORT 등의 정의가 필요
    - 클라이언트는 서버에 UDP 패킷을 전송하고 응답을 기다림
    - UDP는 비연결형 프로토콜이므로 응답을 받을 때 서버의 주소 정보를 사용
================================================================*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "udp.h"

int main(int argc, char *argv[]) 
{
    int                 sockfd, n, peerAddrLen; // 소켓 파일 디스크립터, 읽은 바이트 수, 피어 주소 길이
    struct sockaddr_in  servAddr, peerAddr;      // 서버 및 피어 주소 구조체
    MsgType             msg;                     // 메시지 구조체 (udp.h에서 정의됨)

    // UDP 소켓 생성
    if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // 서버 주소 구조체 초기화 및 설정
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR); // 서버 호스트 주소 설정
    servAddr.sin_port = htons(SERV_UDP_PORT);             // 서버 포트 번호 설정

    // 요청 메시지 작성
    msg.type = MSG_REQUEST;
    sprintf(msg.data, "This is a request from %d.", getpid());

    // 서버로 요청 메시지 전송
    if (sendto(sockfd, (char *)&msg, sizeof(msg), 
            0, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)  {
        perror("sendto");
        exit(1);
    }
    printf("Sent a request.....");
        
    // 피어 주소 길이 초기화
    peerAddrLen = sizeof(peerAddr);

    // 서버로부터 응답 메시지 수신
    if ((n = recvfrom(sockfd, (char *)&msg, sizeof(msg),
                0, (struct sockaddr *)&peerAddr, &peerAddrLen)) < 0)  {
        perror("recvfrom");
        exit(1);
    }
    printf("Received reply: %s\n", msg.data);

    // 소켓 닫기
    close(sockfd);
}

