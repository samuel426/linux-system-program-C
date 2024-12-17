/*===============================================================
[Program Name] : select_t.c
[Description]  : 
    - 다중 소켓(TCP, UDP, UNIX 도메인 연결 지향형, UNIX 도메인 비연결형)을 생성하고 관리하여
      클라이언트의 요청을 처리하는 스레드 기반 서버를 구현한다.
    - select 시스템 호출을 사용하여 다중 소켓을 모니터링하고, 이벤트가 발생한 소켓에 대해
      새로운 스레드를 생성하여 요청을 전담 처리한다.
    - SIGINT 시그널을 처리하여 서버 종료 시 모든 소켓을 닫고 소켓 파일을 삭제한다.
[Input]        : 
    - 클라이언트의 요청 메시지
    - 서버는 클라이언트의 주소 정보를 사용하여 응답을 전송
[Output]       : 
    - 클라이언트로 응답 메시지 전송
    - "Server daemon started....." 및 각 요청 처리 상태 메시지를 콘솔에 출력
[Calls]        : 
    - socket(), bind(), listen(), accept(), recvfrom(), sendto(), read(), write(), close(), signal(), remove(), select(), pthread_create(), perror(), exit(), strcpy(), strlen(), memcpy()
[특기사항]     : 
    - "select.h" 파일에 MsgType, SERV_TCP_PORT, SERV_UDP_PORT, UNIX_STR_PATH, UNIX_DG_PATH 등의 정의가 필요
    - 스레드 안전성을 위해 동기화가 필요한 경우 추가 구현 필요
    - 각 클라이언트 요청을 처리하기 위해 새로운 스레드를 생성하여 독립적으로 처리
    - 오류 발생 시 perror를 통해 에러 메시지를 출력하고 프로그램을 종료
===============================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include "select.h"

// 글로벌 변수
int TcpSockfd;
int UdpSockfd;
int UcoSockfd;
int UclSockfd;

// 자식 스레드에서 사용할 데이터 구조체
typedef struct {
    int sockfd;
    struct sockaddr_in cliAddr;
} TcpClientData;

typedef struct {
    int sockfd;
    struct sockaddr_in cliAddr;
} UdpClientData;

typedef struct {
    int sockfd;
    struct sockaddr_un cliAddr;
} UnixStreamClientData;

typedef struct {
    int sockfd;
    struct sockaddr_un cliAddr;
} UnixDatagramClientData;

// 서버 종료 시 소켓을 닫고 소켓 파일을 삭제한 후 종료
void CloseServer() {
    close(TcpSockfd);
    close(UdpSockfd);
    close(UcoSockfd);
    close(UclSockfd);
    if (remove(UNIX_STR_PATH) < 0)  {
        perror("remove");
    }
    if (remove(UNIX_DG_PATH) < 0)  {
        perror("remove");
    }

    printf("\nServer daemon exit.....\n");
    exit(0);
}

// 스레드에서 실행될 TCP 요청 처리 함수
void* HandleTcpClient(void* arg) {
    TcpClientData* data = (TcpClientData*)arg;
    int newSockfd = data->sockfd;
    MsgType msg;

    // 클라이언트로부터 메시지 읽기
    if (read(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
        perror("read");
        close(newSockfd);
        free(data);
        pthread_exit(NULL);
    }
    printf("Received TCP request: %s.....\n", msg.data);

    // 응답 메시지 작성
    msg.type = MSG_REPLY;
    sprintf(msg.data, "This is a reply from %d.", getpid());

    // 클라이언트에게 응답 메시지 전송
    if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
        perror("write");
        close(newSockfd);
        free(data);
        pthread_exit(NULL);
    }
    printf("Replied to TCP client.\n");

    close(newSockfd);
    free(data);
    pthread_exit(NULL);
}

// 스레드에서 실행될 UDP 요청 처리 함수
void* HandleUdpClient(void* arg) {
    UdpClientData* data = (UdpClientData*)arg;
    int sockfd = data->sockfd;
    struct sockaddr_in cliAddr = data->cliAddr;
    socklen_t cliAddrLen = sizeof(cliAddr);
    MsgType msg;

    // 클라이언트로부터 메시지 수신
    if (recvfrom(sockfd, (char *)&msg, sizeof(msg), 
                0, (struct sockaddr *)&cliAddr, &cliAddrLen) < 0)  {
        perror("recvfrom");
        free(data);
        pthread_exit(NULL);
    }
    printf("Received UDP request: %s.....\n", msg.data);

    // 응답 메시지 작성
    msg.type = MSG_REPLY;
    sprintf(msg.data, "This is a reply from %d.", getpid());

    // 클라이언트에게 응답 메시지 전송
    if (sendto(sockfd, (char *)&msg, sizeof(msg),
                0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0)  {
        perror("sendto");
        free(data);
        pthread_exit(NULL);
    }
    printf("Replied to UDP client.\n");

    free(data);
    pthread_exit(NULL);
}

// 스레드에서 실행될 UNIX 도메인 연결 지향형 소켓 요청 처리 함수
void* HandleUnixStreamClient(void* arg) {
    UnixStreamClientData* data = (UnixStreamClientData*)arg;
    int newSockfd = data->sockfd;
    MsgType msg;

    // 클라이언트로부터 메시지 읽기
    if (read(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
        perror("read");
        close(newSockfd);
        free(data);
        pthread_exit(NULL);
    }
    printf("Received UNIX-domain CO request: %s.....\n", msg.data);

    // 응답 메시지 작성
    msg.type = MSG_REPLY;
    sprintf(msg.data, "This is a reply from %d.", getpid());

    // 클라이언트에게 응답 메시지 전송
    if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
        perror("write");
        close(newSockfd);
        free(data);
        pthread_exit(NULL);
    }
    printf("Replied to UNIX-domain CO client.\n");

    close(newSockfd);
    free(data);
    pthread_exit(NULL);
}

// 스레드에서 실행될 UNIX 도메인 비연결형 소켓 요청 처리 함수
void* HandleUnixDatagramClient(void* arg) {
    UnixDatagramClientData* data = (UnixDatagramClientData*)arg;
    int sockfd = data->sockfd;
    struct sockaddr_un cliAddr = data->cliAddr;
    socklen_t cliAddrLen = sizeof(cliAddr);
    MsgType msg;

    // 클라이언트로부터 메시지 수신
    if (recvfrom(sockfd, (char *)&msg, sizeof(msg), 
                0, (struct sockaddr *)&cliAddr, &cliAddrLen) < 0)  {
        perror("recvfrom");
        free(data);
        pthread_exit(NULL);
    }
    printf("Received UNIX-domain CL request: %s.....\n", msg.data);

    // 응답 메시지 작성
    msg.type = MSG_REPLY;
    sprintf(msg.data, "This is a reply from %d.", getpid());

    // 클라이언트에게 응답 메시지 전송
    if (sendto(sockfd, (char *)&msg, sizeof(msg),
                0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0)  {
        perror("sendto");
        free(data);
        pthread_exit(NULL);
    }
    printf("Replied to UNIX-domain CL client.\n");

    free(data);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    fd_set fdvar;
    int     maxfd;
    int     count;

    // SIGINT 시그널 처리 등록 (Ctrl+C 시 CloseServer 호출)
    signal(SIGINT, CloseServer);

    // 소켓 생성 및 설정 함수 호출
    // (함수는 원래 select.c에서 사용된 것과 동일)
    // MakeTcpSocket(), MakeUdpSocket(), MakeUcoSocket(), MakeUclSocket()
    // 아래는 select.c의 Make 함수들을 그대로 사용함

    // TCP 소켓 생성
    if ((TcpSockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in servAddr_tcp;
    bzero((char *)&servAddr_tcp, sizeof(servAddr_tcp));
    servAddr_tcp.sin_family = PF_INET;
    servAddr_tcp.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr_tcp.sin_port = htons(SERV_TCP_PORT);

    if (bind(TcpSockfd, (struct sockaddr *) &servAddr_tcp, sizeof(servAddr_tcp)) < 0)  {
        perror("bind");
        close(TcpSockfd);
        exit(1);
    }

    listen(TcpSockfd, 5);

    // UDP 소켓 생성
    if ((UdpSockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in servAddr_udp;
    bzero((char *)&servAddr_udp, sizeof(servAddr_udp));
    servAddr_udp.sin_family = PF_INET;
    servAddr_udp.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr_udp.sin_port = htons(SERV_UDP_PORT);

    if (bind(UdpSockfd, (struct sockaddr *) &servAddr_udp, sizeof(servAddr_udp)) < 0)  {
        perror("bind");
        close(UdpSockfd);
        exit(1);
    }

    // UNIX 도메인 연결 지향형 소켓 생성
    if ((UcoSockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    struct sockaddr_un servAddr_unco;
    bzero((char *)&servAddr_unco, sizeof(servAddr_unco));
    servAddr_unco.sun_family = PF_UNIX;
    strcpy(servAddr_unco.sun_path, UNIX_STR_PATH);
    int servAddrLen_unco = strlen(servAddr_unco.sun_path) + sizeof(servAddr_unco.sun_family);

    if (bind(UcoSockfd, (struct sockaddr *) &servAddr_unco, servAddrLen_unco) < 0)  {
        perror("bind");
        close(UcoSockfd);
        exit(1);
    }

    listen(UcoSockfd, 5);

    // UNIX 도메인 비연결형 소켓 생성
    if ((UclSockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)  {
        perror("socket");
        exit(1);
    }

    struct sockaddr_un servAddr_uncl;
    bzero((char *)&servAddr_uncl, sizeof(servAddr_uncl));
    servAddr_uncl.sun_family = PF_UNIX;
    strcpy(servAddr_uncl.sun_path, UNIX_DG_PATH);
    int servAddrLen_uncl = strlen(servAddr_uncl.sun_path) + sizeof(servAddr_uncl.sun_family);

    if (bind(UclSockfd, (struct sockaddr *) &servAddr_uncl, servAddrLen_uncl) < 0)  {
        perror("bind");
        close(UclSockfd);
        exit(1);
    }

    printf("Server daemon started.....\n");

    while (1)  {
        FD_ZERO(&fdvar);
        FD_SET(TcpSockfd, &fdvar);
        FD_SET(UdpSockfd, &fdvar);
        FD_SET(UcoSockfd, &fdvar);
        FD_SET(UclSockfd, &fdvar);

        // 최대 파일 디스크립터 계산
        maxfd = TcpSockfd;
        if (UdpSockfd > maxfd) maxfd = UdpSockfd;
        if (UcoSockfd > maxfd) maxfd = UcoSockfd;
        if (UclSockfd > maxfd) maxfd = UclSockfd;

        // select 호출: 파일 디스크립터 집합 중 이벤트가 발생한 것을 감지
        if ((count = select(maxfd + 1, &fdvar, NULL, NULL, NULL)) < 0)  {
            perror("select");
            exit(1);
        }

        // 이벤트가 발생한 파일 디스크립터에 대해 요청 처리
        if (FD_ISSET(TcpSockfd, &fdvar))  {
            // TCP 클라이언트 연결 수락
            struct sockaddr_in cliAddr_tcp;
            socklen_t cliAddrLen_tcp = sizeof(cliAddr_tcp);
            int newSockfd = accept(TcpSockfd, (struct sockaddr *) &cliAddr_tcp, &cliAddrLen_tcp);
            if (newSockfd < 0)  {
                perror("accept");
                continue;
            }

            // 클라이언트 데이터를 저장할 구조체 할당
            TcpClientData* clientData = malloc(sizeof(TcpClientData));
            if (clientData == NULL) {
                perror("malloc");
                close(newSockfd);
                continue;
            }
            clientData->sockfd = newSockfd;
            memcpy(&clientData->cliAddr, &cliAddr_tcp, sizeof(cliAddr_tcp));

            // 새로운 스레드 생성하여 클라이언트 요청 처리
            pthread_t tid;
            if (pthread_create(&tid, NULL, HandleTcpClient, (void*)clientData) != 0) {
                perror("pthread_create");
                close(newSockfd);
                free(clientData);
                continue;
            }

            // 스레드를 분리하여 자원 자동 해제
            pthread_detach(tid);
        }

        if (FD_ISSET(UdpSockfd, &fdvar))  {
            // UDP 클라이언트 요청 처리
            struct sockaddr_in cliAddr_udp;
            socklen_t cliAddrLen_udp = sizeof(cliAddr_udp);
            MsgType msg_udp;

            // 클라이언트로부터 메시지 수신
            if (recvfrom(UdpSockfd, (char *)&msg_udp, sizeof(msg_udp), 
                        0, (struct sockaddr *)&cliAddr_udp, &cliAddrLen_udp) < 0)  {
                perror("recvfrom");
                continue;
            }
            printf("Received UDP request: %s.....\n", msg_udp.data);

            // 응답 메시지 작성
            msg_udp.type = MSG_REPLY;
            sprintf(msg_udp.data, "This is a reply from %d.", getpid());

            // 클라이언트에게 응답 메시지 전송
            if (sendto(UdpSockfd, (char *)&msg_udp, sizeof(msg_udp),
                        0, (struct sockaddr *)&cliAddr_udp, cliAddrLen_udp) < 0)  {
                perror("sendto");
                continue;
            }
            printf("Replied to UDP client.\n");
        }

        if (FD_ISSET(UcoSockfd, &fdvar))  {
            // UNIX 도메인 연결 지향형 클라이언트 연결 수락
            struct sockaddr_un cliAddr_unco;
            socklen_t cliAddrLen_unco = sizeof(cliAddr_unco);
            int newSockfd = accept(UcoSockfd, (struct sockaddr *) &cliAddr_unco, &cliAddrLen_unco);
            if (newSockfd < 0)  {
                perror("accept");
                continue;
            }

            // 클라이언트 데이터를 저장할 구조체 할당
            UnixStreamClientData* clientData = malloc(sizeof(UnixStreamClientData));
            if (clientData == NULL) {
                perror("malloc");
                close(newSockfd);
                continue;
            }
            clientData->sockfd = newSockfd;
            memcpy(&clientData->cliAddr, &cliAddr_unco, sizeof(cliAddr_unco));

            // 새로운 스레드 생성하여 클라이언트 요청 처리
            pthread_t tid;
            if (pthread_create(&tid, NULL, HandleUnixStreamClient, (void*)clientData) != 0) {
                perror("pthread_create");
                close(newSockfd);
                free(clientData);
                continue;
            }

            // 스레드를 분리하여 자원 자동 해제
            pthread_detach(tid);
        }

        if (FD_ISSET(UclSockfd, &fdvar))  {
            // UNIX 도메인 비연결형 클라이언트 요청 처리
            struct sockaddr_un cliAddr_uncl;
            socklen_t cliAddrLen_uncl = sizeof(cliAddr_uncl);
            MsgType msg_uncl;

            // 클라이언트로부터 메시지 수신
            if (recvfrom(UclSockfd, (char *)&msg_uncl, sizeof(msg_uncl), 
                        0, (struct sockaddr *)&cliAddr_uncl, &cliAddrLen_uncl) < 0)  {
                perror("recvfrom");
                continue;
            }
            printf("Received UNIX-domain CL request: %s.....\n", msg_uncl.data);

            // 응답 메시지 작성
            msg_uncl.type = MSG_REPLY;
            sprintf(msg_uncl.data, "This is a reply from %d.", getpid());

            // 클라이언트에게 응답 메시지 전송
            if (sendto(UclSockfd, (char *)&msg_uncl, sizeof(msg_uncl),
                        0, (struct sockaddr *)&cliAddr_uncl, cliAddrLen_uncl) < 0)  {
                perror("sendto");
                continue;
            }
            printf("Replied to UNIX-domain CL client.\n");
        }
    }

    return 0;
}

