/*===============================================================
[Program Name] : chats.c (Chat Server with Threads)
[Description]  : 
    - 스레드를 사용하여 서버에서 다중 클라이언트 연결을 처리한다.
    - 클라이언트로부터 전달받은 메시지를 다른 클라이언트에게 브로드캐스팅한다.
[Input]        : 
    none (프로그램 실행 시 포트 번호 등을 별도로 받지 않고, 소스 내에서 설정)
[Output]       :
    채팅 메시지 송수신, 클라이언트 로그인/로그아웃 정보
[Calls]        : 
    int GetID()
    void SendToOtherClients(int id, char *buf)
    void ProcessClient(int id)
    void CloseServer(int signo)
    main()
[특기사항]     :
    - 소켓 프로그래밍 기반
    - POSIX pthreads 사용
==================================================================*/

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include "chat.h"

#define DEBUG
#define MAX_CLIENT       5
#define MAX_ID           32
#define MAX_BUF          256

/*===============================================================
[Function Name] : 구조체 정의 (ClientType)
[Description]   : 
    - 각 클라이언트의 상태 정보를 저장하기 위한 구조체
    - 소켓 FD, 사용중 여부, 스레드 ID, 사용자 ID를 포함
[Input]         : 없음
[Output]        : 없음
[Call By]       : main()
[Calls]         : 없음
[Given]         : 없음
[Returns]       : 없음
==================================================================*/
typedef struct  {
	int			sockfd;     // 클라이언트 소켓 식별자
	int			inUse;      // 현재 사용중인지 여부 (0: not in use, 1: in use)
	pthread_t	tid;        // 클라이언트 전용 스레드 ID
	char		uid[MAX_ID];// 클라이언트 사용자 ID
} ClientType;

int             Sockfd;
pthread_mutex_t Mutex;
ClientType      Client[MAX_CLIENT];

/*===============================================================
[Function Name] : GetID()
[Description]   : 
    - Client[] 배열에서 사용 가능한(빈) 인덱스를 찾아 리턴
[Input]         : 없음
[Output]        : 사용 가능한 Client[] 배열 인덱스
[Call By]       : main()
[Calls]         : 없음
[Given]         : Global 변수 Client[]
[Returns]       : int (클라이언트 배열의 사용 가능한 인덱스)
==================================================================*/
int GetID()
{
	int	i;
	for (i = 0 ; i < MAX_CLIENT ; i++)  {
		if (! Client[i].inUse)  {
			Client[i].inUse = 1;   // 해당 인덱스 사용중으로 마킹
			return i;
		}
	}
	return -1;  // 모든 클라이언트가 사용중일 때 -1 리턴(오류 처리용)
}

/*===============================================================
[Function Name] : SendToOtherClients(int id, char *buf)
[Description]   : 
    - id번 클라이언트가 보낸 메시지를 다른 클라이언트에게 전달(broadcast)
[Input]         : 
    int id       - 메시지를 보낸 클라이언트 인덱스
    char *buf    - 메시지 버퍼
[Output]        : 없음(단, 다른 클라이언트에게 메시지를 보냄)
[Call By]       : ProcessClient()
[Calls]         : pthread_mutex_lock(), pthread_mutex_unlock()
[Given]         : Global 변수 Client[]
[Returns]       : 없음
==================================================================*/
void SendToOtherClients(int id, char *buf)
{
	int		i;
	char	msg[MAX_BUF + MAX_ID];

	sprintf(msg, "%s> %s", Client[id].uid, buf);
#ifdef DEBUG
	printf("[DEBUG] Broadcasting: %s", msg);
	fflush(stdout);
#endif

	pthread_mutex_lock(&Mutex);                // 임계영역 시작
	for (i = 0 ; i < MAX_CLIENT ; i++)  {
		if (Client[i].inUse && (i != id))  {
			if (send(Client[i].sockfd, msg, strlen(msg)+1, 0) < 0)  {
				perror("send");
				exit(1);
			}
		}
	}
	pthread_mutex_unlock(&Mutex);              // 임계영역 끝
}

/*===============================================================
[Function Name] : ProcessClient(int id)
[Description]   : 
    - id번 클라이언트가 보낸 메시지를 수신하여 다른 클라이언트에게 전달
    - 클라이언트가 연결을 종료하면 log-out 처리 후 스레드 종료
[Input]         : 
    int id       - 클라이언트 배열 인덱스
[Output]        : 클라이언트 로그인/로그아웃 및 메시지 송수신 결과
[Call By]       : pthread_create()에 의해 생성된 스레드
[Calls]         : SendToOtherClients()
[Given]         : Global 변수 Client[], Mutex
[Returns]       : 없음 (스레드 함수이므로 pthread_exit()로 종료)
==================================================================*/
void ProcessClient(int id)
{
	char	buf[MAX_BUF];
	int		n;

	if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))  {
		perror("pthread_setcancelstate");
		exit(1);
	}
	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))  {
		perror("pthread_setcanceltype");
		exit(1);
	}

	// 클라이언트로부터 사용자 ID 수신
	if ((n = recv(Client[id].sockfd, Client[id].uid, MAX_ID, 0)) < 0)  {
		perror("recv");
		exit(1);
	}
	printf("Client %d log-in (ID: %s).....\n", id, Client[id].uid);

	// 메시지 수신 루프
	while (1)  {
		if ((n = recv(Client[id].sockfd, buf, MAX_BUF, 0)) < 0)  {
			perror("recv");
			exit(1);
		}
		if (n == 0)  {
			// 클라이언트 소켓 종료 처리
			printf("Client %d log-out (ID: %s).....\n", id, Client[id].uid);

			pthread_mutex_lock(&Mutex);
			close(Client[id].sockfd);
			Client[id].inUse = 0; // 다시 사용 가능 상태로
			pthread_mutex_unlock(&Mutex);

			strcpy(buf, "log-out.....\n");
			SendToOtherClients(id, buf);

			pthread_exit(NULL); // 스레드 종료
		}

		// 받은 메시지를 다른 클라이언트에게 전송
		SendToOtherClients(id, buf);
	}
}

/*===============================================================
[Function Name] : CloseServer(int signo)
[Description]   : 
    - SIGINT(Ctrl+C) 시그널을 받았을 때, 서버를 안전하게 종료
[Input]         : 
    int signo    - 시그널 번호
[Output]        : 서버 종료 메시지
[Call By]       : signal(SIGINT, CloseServer)
[Calls]         : pthread_cancel(), pthread_join(), close()
[Given]         : Global 변수 Sockfd, Client[], Mutex
[Returns]       : 없음(프로그램 종료)
==================================================================*/
void CloseServer(int signo)
{
	int		i;

	close(Sockfd);

	for (i = 0 ; i < MAX_CLIENT ; i++)  {
		if (Client[i].inUse)  {
			if (pthread_cancel(Client[i].tid))  {
				perror("pthread_cancel");
				exit(1);
			}
			if (pthread_join(Client[i].tid, NULL))  {
				perror("pthread_join");
				exit(1);
			}
			close(Client[i].sockfd);
		}
	}
	if (pthread_mutex_destroy(&Mutex) < 0)  {
		perror("pthread_mutex_destroy");
		exit(1);
	}

	printf("\nChat server terminated.....\n");
	exit(0);
}

/*===============================================================
[Function Name] : main(int argc, char *argv[])
[Description]   : 
    - 서버 소켓 생성 및 초기화
    - 클라이언트 접속을 accept하고, 각 접속마다 스레드를 생성
    - SIGINT 시그널 처리 설정
[Input]         : 
    int argc, char *argv[]  - 프로그램 실행 인자(사용하지 않음)
[Output]        : 서버 시작 메시지
[Call By]       : OS
[Calls]         : GetID(), pthread_create(), ProcessClient(), ...
[Given]         : Global 변수 Sockfd, Mutex, Client[]
[Returns]       : int (프로그램 종료 상태)
==================================================================*/
int main(int argc, char *argv[])
{
	int					newSockfd, cliAddrLen, id, one = 1;
	struct sockaddr_in	cliAddr, servAddr;

	signal(SIGINT, CloseServer);
	if (pthread_mutex_init(&Mutex, NULL) < 0)  {
		perror("pthread_mutex_init");
		exit(1);
	}

	if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	if (setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)  {
		perror("setsockopt");
		exit(1);
	}

	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family      = PF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port        = htons(SERV_TCP_PORT);

	if (bind(Sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
		perror("bind");
		exit(1);
	}

	listen(Sockfd, 5);

	printf("Chat server started.....\n");

	cliAddrLen = sizeof(cliAddr);
	while (1)  {
		newSockfd = accept(Sockfd, (struct sockaddr *) &cliAddr, (socklen_t *)&cliAddrLen);
		if (newSockfd < 0)  {
			perror("accept");
			exit(1);
		}

		id = GetID();
		if (id < 0) {
			// 더 이상 클라이언트를 받을 수 없음
			fprintf(stderr, "Maximum number of clients reached.\n");
			close(newSockfd);
			continue;
		}
		Client[id].sockfd = newSockfd;
		
		if (pthread_create(&Client[id].tid, NULL, (void *)ProcessClient, (void *)(long)id) < 0)  {
			perror("pthread_create");
			exit(1);
		}
	}

	return 0;
}

