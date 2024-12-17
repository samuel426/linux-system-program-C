/*===============================================================
[Program Name] : udps.c
[Description]  : 
    - UDP 서버를 구현하여 클라이언트로부터 요청 메시지를 받고, 이에 대한 응답을 전송한다.
[Input]        : 
    - 클라이언트의 요청 메시지
    - 서버는 클라이언트의 주소와 포트를 파악하여 응답을 전송
[Output]       : 
    - 클라이언트로 응답 메시지 전송
    - "Received request: <메시지>....." 및 "Replied."를 콘솔에 출력
[Calls]        : 
    - socket(), bind(), recvfrom(), sendto(), close(), signal()
[특기사항]     : 
    - 서버 종료를 위해 SIGINT 시그널(Ctrl+C) 처리
    - "udp.h" 파일에 MsgType과 SERV_UDP_PORT 등의 정의가 필요
    - 서버는 INADDR_ANY를 사용하여 모든 인터페이스에서 연결을 수락
    - UDP는 비연결형 프로토콜이므로 클라이언트의 주소 정보를 사용하여 응답
================================================================*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "udp.h"

int Sockfd; // 서버 소켓 파일 디스크립터

/*===============================================================
[Function Name] : CloseServer
[Description]   : 
    - 서버 소켓을 닫고 종료 메시지를 출력한 후 프로그램을 종료한다.
[Input]         : 
    - 없음
[Output]        : 
    - 콘솔에 "UDP Server exit....." 메시지 출력
[Call By]       : 
    - 시그널 핸들러(SIGINT)에 의해 호출
[Calls]         : 
    - close(), printf(), exit()
[Given]         : 
    - 글로벌 변수 Sockfd가 유효한 소켓 디스크립터를 가리키고 있다고 가정
[Returns]       : 
    - 없음 (프로그램 종료)
================================================================*/
void CloseServer()
{
    close(Sockfd); // 서버 소켓 닫기
    printf("\nUDP Server exit.....\n");
    exit(0); // 프로그램 종료
}

int main(int argc, char *argv[])
{
    int                 cliAddrLen, n; // 클라이언트 주소 길이, 읽은 바이트 수
    struct sockaddr_in  cliAddr, servAddr; // 클라이언트 및 서버 주소 구조체
    MsgType             msg; // 메시지 구조체 (udp.h에서 정의됨)

    // SIGINT 시그널 처리 등록 (Ctrl+C 시 CloseServer 호출)
    signal(SIGINT, CloseServer);

    // UDP 소켓 생성
    if ((Sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    // 서버 주소 구조체 초기화 및 설정
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 인터페이스에서 연결 수락
    servAddr.sin_port = htons(SERV_UDP_PORT);    // 서버 포트 번호 설정

    // 소켓과 주소 바인딩
    if (bind(Sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)  {
        perror("bind");
        exit(1);
    }

    printf("UDP Server started.....\n");

    cliAddrLen = sizeof(cliAddr);
    while (1)  {
        // 클라이언트로부터 메시지 수신
        if ((n = recvfrom(Sockfd, (char *)&msg, sizeof(msg), 
                    0, (struct sockaddr *)&cliAddr, &cliAddrLen)) < 0)  {
            perror("recvfrom");
            exit(1);
        }
        printf("Received request: %s.....", msg.data);

        // 응답 메시지 작성
        msg.type = MSG_REPLY;
        sprintf(msg.data, "This is a reply from %d.", getpid());

        // 클라이언트에게 응답 메시지 전송
        if (sendto(Sockfd, (char *)&msg, sizeof(msg),
                    0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0)  {
            perror("sendto");
            exit(1);
        }
        printf("Replied.\n");
    }
}

