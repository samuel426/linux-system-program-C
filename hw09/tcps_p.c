/*===============================================================
[Program Name] : tcps_p.c
[Description]  : 
    - TCP 서버를 구현하여 클라이언트와 연결을 수락한 후,
      새로운 프로세스를 생성하여 클라이언트 요청을 전담 처리한다.
    - 부모 프로세스는 계속해서 새로운 클라이언트의 연결을 수락할 수 있다.
[Input]        : 
    - 클라이언트의 요청 메시지
    - 서버는 클라이언트의 주소 정보를 사용하여 응답을 전송
[Output]       : 
    - 클라이언트로 응답 메시지 전송
    - "TCP Server started.....", "Received request: ...", "Replied." 등의 상태 메시지를 콘솔에 출력
[Calls]        : 
    - socket(), bind(), listen(), accept(), fork(), read(), write(), close(), perror(), exit(), strcpy(), sprintf()
[특기사항]     : 
    - `tcp.h` 파일에 MsgType, SERV_TCP_PORT 등의 정의가 필요
    - SIGCHLD 시그널을 처리하여 종료된 자식 프로세스의 상태를 정리
    - 오류 발생 시 perror를 통해 에러 메시지를 출력하고 프로그램을 종료
===============================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "tcp.h"

// 글로벌 변수 (필요에 따라 조정 가능)
int Sockfd;

// 자식 프로세스 종료 시 부모 프로세스가 자식의 종료 상태를 수집할 수 있도록 SIGCHLD 시그널 핸들러 등록
void sigchld_handler(int signo) {
    // waitpid를 사용하여 종료된 자식 프로세스를 비동기적으로 수집
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// 서버 종료 시 소켓을 닫고 종료 메시지를 출력
void CloseServer() {
    close(Sockfd);
    printf("\nTCP Server exit.....\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    int                 newSockfd, cliAddrLen, n;
    struct sockaddr_in  cliAddr, servAddr;
    MsgType             msg;
    pid_t               pid;

    // SIGINT 시그널 핸들러 등록 (Ctrl+C 시 CloseServer 호출)
    signal(SIGINT, CloseServer);
    // SIGCHLD 시그널 핸들러 등록 (자식 프로세스 종료 시 처리)
    signal(SIGCHLD, sigchld_handler);

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
        close(Sockfd);
        exit(1);
    }

    // 연결 요청 대기 상태로 설정
    listen(Sockfd, 5);

    printf("TCP Server started.....\n");

    cliAddrLen = sizeof(cliAddr);
    while (1)  {
        // 클라이언트 연결 수락
        newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, &cliAddrLen);
        if (newSockfd < 0)  {
            perror("accept");
            continue; // 오류 발생 시 루프를 계속해서 다음 클라이언트 수락 시도
        }

        // 새로운 프로세스 생성
        pid = fork();
        if (pid < 0) {
            perror("fork");
            close(newSockfd);
            continue; // 프로세스 생성 실패 시 소켓을 닫고 루프를 계속
        }

        if (pid == 0) { // 자식 프로세스
            close(Sockfd); // 자식 프로세스는 원본 소켓을 닫음

            // 클라이언트로부터 메시지 읽기
            if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  {
                perror("read");
                close(newSockfd);
                exit(1);
            }
            printf("Received request: %s.....\n", msg.data);

            // 응답 메시지 작성
            msg.type = MSG_REPLY;
            sprintf(msg.data, "This is a reply from %d.", getpid());

            // 클라이언트에게 응답 메시지 전송
            if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
                perror("write");
                close(newSockfd);
                exit(1);
            }
            printf("Replied.\n");

            close(newSockfd); // 클라이언트 소켓 닫기
            exit(0); // 자식 프로세스 종료
        }
        else { // 부모 프로세스
            close(newSockfd); // 부모는 클라이언트 소켓을 닫고, 루프를 통해 다음 클라이언트 수락
        }
    }

    return 0;
}

