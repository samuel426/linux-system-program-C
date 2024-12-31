/*===============================================================
[Program Name] : chatc_multithread.c
[Description]  :
    - 채팅 클라이언트에서 select() 대신 스레드를 이용하여
      1) 키보드 입력 스레드, 2) 서버 메시지 수신 스레드 로 분리하여 동작.
[Input]        :
    실행 시 서버 IP주소 인자로 입력 (예: ./chatc_multithread 127.0.0.1)
[Output]       :
    채팅 메시지 송수신
[Calls]        :
    void *SendThread(void *arg)
    void *ReceiveThread(void *arg)
    CloseClient(int signo)
    main()
[특기사항]     :
    - POSIX pthreads 사용
==================================================================*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "chat.h"

#define MAX_BUF       256

int Sockfd;

/*===============================================================
[Function Name] : CloseClient(int signo)
[Description]   : 
    - SIGINT 처리 함수로, 소켓을 닫고 프로그램을 종료
[Input]         :
    int signo    - 시그널 번호
[Output]        : 종료 메시지
[Call By]       : signal(SIGINT, CloseClient)
[Calls]         : close()
[Given]         : 전역변수 Sockfd
[Returns]       : 없음(프로그램 종료)
==================================================================*/
void CloseClient(int signo)
{
	close(Sockfd);
	printf("\nChat client terminated.....\n");
	exit(0);
}

/*===============================================================
[Function Name] : *SendThread(void *arg)
[Description]   : 
    - 키보드 입력(표준입력)을 받아 서버로 전송하는 스레드
[Input]         : 없음(전역변수 Sockfd 사용)
[Output]        : 서버로 채팅 메시지 전송
[Call By]       : pthread_create() in main()
[Calls]         : send()
[Given]         : 전역변수 Sockfd
[Returns]       : 없음(스레드 함수이므로 pthread_exit)
==================================================================*/
void *SendThread(void *arg)
{
	char buf[MAX_BUF];
	int n;

	while (1) {
		if (fgets(buf, MAX_BUF, stdin) == NULL) {
			// EOF or error
			break;
		}
		n = send(Sockfd, buf, strlen(buf)+1, 0);
		if (n < 0) {
			perror("send");
			pthread_exit(NULL);
		}
	}
	pthread_exit(NULL);
}

/*===============================================================
[Function Name] : *ReceiveThread(void *arg)
[Description]   : 
    - 서버로부터 메시지를 수신하여 화면에 출력하는 스레드
[Input]         : 없음(전역변수 Sockfd 사용)
[Output]        : 화면에 받은 메시지 출력
[Call By]       : pthread_create() in main()
[Calls]         : recv()
[Given]         : 전역변수 Sockfd
[Returns]       : 없음(스레드 함수이므로 pthread_exit)
==================================================================*/
void *ReceiveThread(void *arg)
{
	char buf[MAX_BUF];
	int  n;

	while (1) {
		n = recv(Sockfd, buf, MAX_BUF, 0);
		if (n < 0) {
			perror("recv");
			pthread_exit(NULL);
		}
		if (n == 0) {
			fprintf(stderr, "Server terminated.....\n");
			pthread_exit(NULL);
		}
		printf("%s", buf);
		fflush(stdout);
	}
	pthread_exit(NULL);
}

/*===============================================================
[Function Name] : main(int argc, char *argv[])
[Description]   : 
    - 서버와 연결 후, ID 전송
    - 2개의 스레드 생성 (SendThread, ReceiveThread)
    - SIGINT 시그널 처리
[Input]         :
    int argc, char *argv[]
    (argv[1] - 서버 IP 주소)
[Output]        :
    키보드 입력 및 서버 메시지 송수신
[Call By]       : OS
[Calls]         : 
    CloseClient(), pthread_create(), SendThread(), ReceiveThread()
[Given]         : 전역변수 Sockfd
[Returns]       : int (프로그램 종료 상태)
==================================================================*/
int main(int argc, char *argv[])
{
	struct sockaddr_in servAddr;
	struct hostent *hp;
	pthread_t tidSend, tidRecv;
	char buf[MAX_BUF];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s ServerIPaddress\n", argv[0]);
		exit(1);
	}

	if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_port   = htons(SERV_TCP_PORT);

	// 호스트 이름 or IP 처리
	if (isdigit(argv[1][0])) {
		servAddr.sin_addr.s_addr = inet_addr(argv[1]);
	} else {
		if ((hp = gethostbyname(argv[1])) == NULL) {
			fprintf(stderr, "Unknown host: %s\n", argv[1]);
			exit(1);
		}
		memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length);
	}

	if (connect(Sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
		perror("connect");
		exit(1);
	}

	signal(SIGINT, CloseClient);

	// 사용자 ID 입력 및 서버 전송
	printf("Enter ID: ");
	fflush(stdout);
	if (fgets(buf, MAX_BUF, stdin) != NULL) {
		char *p = strchr(buf, '\n');
		if (p) *p = '\0'; // 개행 문자 제거
		if (send(Sockfd, buf, strlen(buf)+1, 0) < 0) {
			perror("send");
			exit(1);
		}
	}

	// 송신 스레드, 수신 스레드 생성
	if (pthread_create(&tidSend, NULL, SendThread, NULL) < 0) {
		perror("pthread_create for send");
		exit(1);
	}
	if (pthread_create(&tidRecv, NULL, ReceiveThread, NULL) < 0) {
		perror("pthread_create for recv");
		exit(1);
	}

	// 메인 스레드는 단순 대기
	pthread_join(tidSend, NULL);
	pthread_join(tidRecv, NULL);

	close(Sockfd);
	return 0;
}

