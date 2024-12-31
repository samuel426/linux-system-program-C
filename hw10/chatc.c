/*===============================================================
[Program Name] : chatc.c
[Description]  : 
    - 채팅 클라이언트 프로그램.
    - 서버에 접속하여 ID를 전송한 후, 
      select() 시스템 콜을 이용하여 키보드 입력과 소켓 수신을 
      동시에 감시(multiplexing)한다.
[Input]        :
    - 프로그램 실행 시, 첫 번째 인자로 서버 IP 주소를 받음.
    - 표준 입력(STDIN_FILENO)을 통해 채팅 메시지 입력.
[Output]       :
    - 서버로 전송한 메시지 / 서버에서 수신한 메시지 화면에 출력.
[Calls]        :
    - ChatClient()
    - CloseClient(int signo)
    - main(int argc, char *argv[])
[특기사항]     :
    - Ctrl + C(SIGINT) 입력 시 CloseClient()를 통해 정상 종료.
    - 서버가 종료되면 recv()에서 0 반환 → 클라이언트 종료 처리.
==================================================================*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>  // exit() 사용을 위해 추가
#include <ctype.h>   // isdigit() 사용을 위해 추가
#include "chat.h"

/*===============================================================
[Definition] : 상수, 전역변수 등 정의
==================================================================*/
#define MAX_BUF 256

int  Sockfd;  // 서버와 통신할 소켓 식별자

/*===============================================================
[Function Name] : ChatClient()
[Description]   :
    - 사용자로부터 ID를 입력받아 서버에 전송.
    - select() 함수를 이용하여, 동시에
      1) 서버로부터 수신되는 메시지 감시
      2) 키보드로부터의 입력(채팅 메시지) 감시
    - 서버에서 받은 메시지는 화면에 출력.
    - 키보드 입력 받은 메시지는 서버로 전송.
[Input]         :
    - STDIN_FILENO(키보드 입력)
    - Sockfd(서버 소켓)
[Output]        :
    - 서버로부터 받은 메시지를 stdout으로 출력
    - 서버에 메시지 전송
[Call By]       :
    - main()에서 호출됨
[Calls]         :
    - send(), recv(), select() 등 소켓 관련 함수
[Given]         :
    - 전역변수 Sockfd
[Returns]       :
    - 없음 (void 함수)
==================================================================*/
void ChatClient(void)
{
    char    buf[MAX_BUF];
    int     count, n;
    fd_set  fdset;

    printf("Enter ID: ");
    fflush(stdout);
    fgets(buf, MAX_BUF, stdin);

    /* 
     * fgets()로 읽어온 문자열 끝에 개행이 포함되므로,
     * 이를 '\0'으로 교체하여 문자열을 정돈한다.
     */
    *strchr(buf, '\n') = '\0';

    // 입력받은 ID를 서버로 전송
    if (send(Sockfd, buf, strlen(buf)+1, 0) < 0) {
        perror("send");
        exit(1);
    }
    printf("Press ^C to exit\n");

    while (1) {
        FD_ZERO(&fdset);
        FD_SET(Sockfd, &fdset);
        FD_SET(STDIN_FILENO, &fdset);

        // select() 대기
        if ((count = select(Sockfd + 1, &fdset, NULL, NULL, NULL)) < 0) {
            perror("select");
            exit(1);
        }

        // count 횟수만큼 이벤트 처리
        while (count--) {
            // 소켓에서 읽을 데이터가 있는 경우
            if (FD_ISSET(Sockfd, &fdset)) {
                if ((n = recv(Sockfd, buf, MAX_BUF, 0)) < 0) {
                    perror("recv");
                    exit(1);
                }
                if (n == 0) {
                    // 서버 소켓이 닫힘(서버 종료)
                    fprintf(stderr, "Server terminated.....\n");
                    close(Sockfd);
                    exit(1);
                }
                // 서버로부터 받은 메시지를 화면에 출력
                printf("%s", buf);
            }
            // 키보드 입력(STDIN)이 있는 경우
            else if (FD_ISSET(STDIN_FILENO, &fdset)) {
                fgets(buf, MAX_BUF, stdin);
                if ((n = send(Sockfd, buf, strlen(buf)+1, 0)) < 0) {
                    perror("send");
                    exit(1);
                }
            }
        }
    }
}

/*===============================================================
[Function Name] : CloseClient(int signo)
[Description]   :
    - SIGINT(Ctrl + C) 시그널이 발생했을 때 호출되어 
      소켓을 닫고 클라이언트를 종료한다.
[Input]         :
    - int signo : 시그널 번호
[Output]        :
    - "Chat client terminated....." 메시지 출력
[Call By]       :
    - signal(SIGINT, CloseClient)에 의해 호출
[Calls]         :
    - close() 함수 (소켓 닫기)
[Given]         :
    - 전역변수 Sockfd
[Returns]       :
    - 없음(프로그램 종료)
==================================================================*/
void CloseClient(int signo)
{
    close(Sockfd);
    printf("\nChat client terminated.....\n");
    exit(0);
}

/*===============================================================
[Function Name] : main(int argc, char *argv[])
[Description]   :
    - 채팅 클라이언트의 시작점.
    - 서버 IP 주소(문자열)를 인자로 받아 소켓을 생성하고,
      해당 서버에 connect()로 연결.
    - SIGINT 시그널에 대한 핸들러로 CloseClient() 등록.
    - ChatClient() 함수를 통해 채팅 로직 수행.
[Input]         :
    - int argc, char *argv[]
    - argv[1]: 서버 IP주소(문자열)
[Output]        :
    - 사용법 안내(Usage) / 연결 성공, 종료 메시지
[Call By]       :
    - OS
[Calls]         :
    - socket(), connect(), signal(), ChatClient() 등
[Given]         :
    - 전역변수 Sockfd
[Returns]       :
    - int (프로그램 종료 코드)
==================================================================*/
int main(int argc, char *argv[])
{
    struct sockaddr_in  servAddr;
    struct hostent      *hp;

    // 인자 체크
    if (argc != 2) {
        fprintf(stderr, "Usage: %s ServerIPaddress\n", argv[0]);
        exit(1);
    }

    // 소켓 생성
    if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    // 서버 주소 구조체 초기화
    bzero((char *)&servAddr, sizeof(servAddr));
    servAddr.sin_family = PF_INET;
    servAddr.sin_port   = htons(SERV_TCP_PORT);

    // argv[1]이 IP 형태인지 호스트 이름인지 판별
    if (isdigit(argv[1][0])) {
        servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    }
    else {
        if ((hp = gethostbyname(argv[1])) == NULL) {
            fprintf(stderr, "Unknown host: %s\n", argv[1]);
            exit(1);
        }
        memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length);
    }

    // 서버에 연결
    if (connect(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        perror("connect");
        exit(1);
    }

    // SIGINT 핸들러 등록
    signal(SIGINT, CloseClient);

    // 채팅 클라이언트 로직 시작
    ChatClient();

    return 0;
}

