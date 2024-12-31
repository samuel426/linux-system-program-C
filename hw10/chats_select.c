/*===============================================================
[Program Name] : chats_select.c
[Description]  : 
    - 다중 클라이언트 연결을 select()를 통해 하나의 프로세스/스레드에서 처리.
    - 클라이언트에게서 받은 메시지를 다른 클라이언트들에게 브로드캐스트.
[Input]        :
    (코드 내 SERV_TCP_PORT 사용 / 별도 인자 없음)
[Output]       :
    - 각 클라이언트 로그인/로그아웃
    - 채팅 메시지 송수신
[Calls]        : 
    int main(int argc, char *argv[])
    void BroadcastMessage(int sender, char *msg)
    void CloseServer(int signo)
[특기사항]     :
    - 스레드 사용 없이 select()로 I/O multiplexing
    - SIGINT(Ctrl + C)로 서버 종료
==================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "chat.h"


#define MAX_CLIENT       5
#define MAX_ID           32
#define MAX_BUF          256

/*===============================================================
[Structure]     : ClientType
[Description]   : 
    - 클라이언트 상태 정보를 저장하기 위한 구조체
[Fields]        :
    int  sockfd     : 클라이언트 소켓 디스크립터
    int  inUse      : 사용 여부(0: 미사용, 1: 사용)
    char uid[MAX_ID]: 클라이언트 ID(문자열)
==================================================================*/
typedef struct {
    int  sockfd;    
    int  inUse;     
    char uid[MAX_ID];
} ClientType;

ClientType Client[MAX_CLIENT];
int        Sockfd;  // 서버 소켓 식별자

/*===============================================================
[Function Name] : BroadcastMessage(int sender, char *msg)
[Description]   : 
    - sender 클라이언트가 보낸 메시지를 모든 다른 클라이언트에게 전송
[Input]         : 
    int sender   - 메시지를 보낸 클라이언트 인덱스
    char *msg    - 전송할 메시지
[Output]        : 해당 메시지를 다른 클라이언트에게 send
[Call By]       : main() 루프 내에서 recv() 후 호출
[Calls]         : send()
[Given]         : 전역변수 Client[]
[Returns]       : 없음
==================================================================*/
void BroadcastMessage(int sender, char *msg)
{
    char buf[MAX_BUF + MAX_ID];

    // "ID> 메시지" 형태로 만들기
    sprintf(buf, "%s> %s", Client[sender].uid, msg);

    for (int i = 0; i < MAX_CLIENT; i++) {
        if (Client[i].inUse && (i != sender)) {
            if (send(Client[i].sockfd, buf, strlen(buf)+1, 0) < 0) {
                perror("send");
                close(Client[i].sockfd);
                Client[i].inUse = 0;
            }
        }
    }
}

/*===============================================================
[Function Name] : CloseServer(int signo)
[Description]   : 
    - SIGINT(Ctrl + C) 발생 시 서버 소켓 및 모든 클라이언트 소켓을 닫고 종료
[Input]         : 
    int signo    - 시그널 번호
[Output]        : 서버 종료 메시지
[Call By]       : signal(SIGINT, CloseServer)
[Calls]         : close()
[Given]         : 전역변수 Sockfd, Client[]
[Returns]       : 없음(프로그램 종료)
==================================================================*/
void CloseServer(int signo)
{
    printf("\nTerminating server...\n");
    close(Sockfd);

    for (int i = 0; i < MAX_CLIENT; i++) {
        if (Client[i].inUse) {
            close(Client[i].sockfd);
        }
    }
    exit(0);
}

/*===============================================================
[Function Name] : main(int argc, char *argv[])
[Description]   : 
    - 서버 소켓 생성, bind, listen 후 select()를 사용해 
      여러 클라이언트를 동시에 처리.
    - 새 클라이언트 접속 시 uid를 수신해 Client 배열에 저장.
    - 메시지 수신 시 BroadcastMessage() 통해 다른 클라이언트에게 전송.
[Input]         : 
    int argc, char *argv[] - (사용 안 함)
[Output]        : 
    - 서버 시작/종료 메시지, 클라이언트 연결/메시지
[Call By]       : OS
[Calls]         : 
    CloseServer(), BroadcastMessage(), select(), accept(), recv(), send()
[Given]         : 전역변수 Sockfd, Client[]
[Returns]       : int (프로그램 종료 상태)
==================================================================*/
int main(int argc, char *argv[])
{
    struct sockaddr_in servAddr, cliAddr;
    socklen_t          cliAddrLen;
    int                newSockfd, maxFd, n;
    fd_set             readFds;
    char               buf[MAX_BUF];

    signal(SIGINT, CloseServer);

    // 서버 소켓 생성
    if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    int one = 1;
    if (setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family      = PF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port        = htons(SERV_TCP_PORT);

    if (bind(Sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(Sockfd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Select-based Chat Server started...\n");

    // Client[] 배열 초기화
    for(int i = 0; i < MAX_CLIENT; i++) {
        Client[i].sockfd = -1;
        Client[i].inUse  = 0;
        memset(Client[i].uid, 0, MAX_ID);
    }

    // 메인 루프
    while (1) {
        FD_ZERO(&readFds);
        FD_SET(Sockfd, &readFds);

        // 가장 큰 파일 디스크립터 번호 찾기
        maxFd = Sockfd;

        // 사용 중인 클라이언트 소켓을 readFds에 추가 & maxFd 갱신
        for(int i = 0; i < MAX_CLIENT; i++) {
            if (Client[i].inUse) {
                FD_SET(Client[i].sockfd, &readFds);
                if (Client[i].sockfd > maxFd)
                    maxFd = Client[i].sockfd;
            }
        }

        // select() 대기
        int sel = select(maxFd + 1, &readFds, NULL, NULL, NULL);
        if (sel < 0) {
            perror("select");
            exit(1);
        }

        // 1) 새로운 클라이언트 연결 확인
        if (FD_ISSET(Sockfd, &readFds)) {
            cliAddrLen = sizeof(cliAddr);
            newSockfd  = accept(Sockfd, (struct sockaddr *)&cliAddr, &cliAddrLen);
            if (newSockfd < 0) {
                perror("accept");
                continue;
            }

            // 빈 자리 찾기
            int i;
            for (i = 0; i < MAX_CLIENT; i++) {
                if (!Client[i].inUse) {
                    Client[i].sockfd = newSockfd;
                    Client[i].inUse  = 1;

                    // 클라이언트 uid 수신
                    if ((n = recv(newSockfd, Client[i].uid, MAX_ID, 0)) <= 0) {
                        // 수신 실패(즉시 끊어짐)
                        close(newSockfd);
                        Client[i].inUse = 0;
                    } else {
                        printf("Client %d connected with ID: %s\n", i, Client[i].uid);
                    }
                    break;
                }
            }

            if (i == MAX_CLIENT) {
                // 자리가 없는 경우
                char *msg = "Server is full\n";
                send(newSockfd, msg, strlen(msg)+1, 0);
                close(newSockfd);
            }
        }

        // 2) 기존 클라이언트 소켓에서의 데이터 수신
        for(int i = 0; i < MAX_CLIENT; i++) {
            if (Client[i].inUse && FD_ISSET(Client[i].sockfd, &readFds)) {
                // recv()로 데이터 수신
                n = recv(Client[i].sockfd, buf, MAX_BUF, 0);
                if (n <= 0) {
                    // 연결 종료
                    printf("Client %d (ID: %s) disconnected.\n", i, Client[i].uid);
                    close(Client[i].sockfd);
                    Client[i].inUse = 0;
                }
                else {
                    // 디버그: 서버가 받은 내용을 확인
                    // (이 줄을 통해 서버가 정말 메시지를 받는지 확인 가능)
                    printf("[DEBUG] Received from client %d (%s): %s\n", i, Client[i].uid, buf);

                    // 받은 메시지 브로드캐스트
                    BroadcastMessage(i, buf);
                }
            }
        }
    }

    return 0;  // 일반적으로 도달 X
}

