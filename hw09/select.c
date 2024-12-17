/*===============================================================
[Program Name] : select.c
[Description]  : 
    - 여러 종류의 소켓(TCP, UDP, UNIX 도메인 연결 지향형, 
      UNIX 도메인 비연결형)을 생성하고 관리하여 클라이언트의 요청을 처리한다.
    - select 시스템 호출을 사용하여 다중 소켓을 모니터링하고 
      이벤트가 발생한 소켓에 대해 적절한 요청 처리 함수를 호출한다.
    - SIGINT 시그널을 처리하여 서버 종료 시 모든 소켓을 닫고 소켓 파일을 삭제한다.
[Input]        : 
    - 클라이언트의 요청 메시지
    - 서버는 클라이언트의 주소 정보를 사용하여 응답을 전송
[Output]       : 
    - 클라이언트로 응답 메시지 전송
    - "Server daemon started....." 및 각 요청 처리 상태 메시지를 콘솔에 출력
[Calls]        : 
    - socket(), bind(), listen(), accept(), read(), write(), recvfrom(), sendto(), close(), signal(), remove(), select()
[특기사항]     : 
    - "select.h" 파일에 MsgType, SERV_TCP_PORT, SERV_UDP_PORT, UNIX_STR_PATH, UNIX_DG_PATH 등의 정의가 필요
    - TCP, UDP, UNIX 도메인 소켓을 모두 지원하는 멀티 프로토콜 서버 구현
    - select 시스템 호출을 사용하여 효율적으로 다중 소켓을 관리
    - 서버 종료 시 UNIX 도메인 소켓 파일을 삭제하여 리소스 정리
===============================================================*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include "select.h"

int	TcpSockfd;
int	UdpSockfd;
int	UcoSockfd;
int	UclSockfd;

/*===============================================================
[Function Name] : CloseServer
[Description]   : 
    - 모든 소켓을 닫고 UNIX 도메인 소켓 파일을 삭제한 후 종료 메시지를 출력하며 프로그램을 종료한다.
[Input]         : 
    - 없음
[Output]        : 
    - "Server daemon exit....." 메시지 출력
[Call By]       : 
    - SIGINT 시그널 핸들러
[Calls]         : 
    - close(), remove(), printf(), exit()
[Given]         : 
    - 글로벌 변수 TcpSockfd, UdpSockfd, UcoSockfd, UclSockfd가 유효한 소켓 디스크립터를 가리키고 있다고 가정
[Returns]       : 
    - 없음 (프로그램 종료)
===============================================================*/
void
CloseServer()
{
	close(TcpSockfd);   // TCP 소켓 닫기
	close(UdpSockfd);   // UDP 소켓 닫기
	close(UcoSockfd);   // UNIX 도메인 연결 지향형 소켓 닫기
	close(UclSockfd);   // UNIX 도메인 비연결형 소켓 닫기
	if (remove(UNIX_STR_PATH) < 0)  { // UNIX 도메인 연결 지향형 소켓 파일 삭제
		perror("remove");
	}
	if (remove(UNIX_DG_PATH) < 0)  {  // UNIX 도메인 비연결형 소켓 파일 삭제
		perror("remove");
	}

	printf("\nServer daemon exit.....\n");
	exit(0); // 프로그램 종료
}

/*===============================================================
[Function Name] : MakeTcpSocket
[Description]   : 
    - TCP 소켓을 생성하고 서버 주소에 바인딩한 후 연결 요청을 대기 상태로 만든다.
[Input]         : 
    - 없음
[Output]        : 
    - TCP 소켓이 생성되고 바인딩된 상태
[Call By]       : 
    - main()
[Calls]         : 
    - socket(), bind(), listen(), perror(), exit()
[Given]         : 
    - "select.h" 파일에 SERV_TCP_PORT가 정의되어 있어야 함
[Returns]       : 
    - 없음
===============================================================*/
void
MakeTcpSocket()
{
	struct sockaddr_in	servAddr;

	// TCP 소켓 생성
	if ((TcpSockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	// 서버 주소 구조체 초기화 및 설정
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERV_TCP_PORT);

	// 소켓과 서버 주소 바인딩
	if (bind(TcpSockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
		perror("bind");
		exit(1);
	}

	// 연결 요청 대기 상태로 설정
	listen(TcpSockfd, 5);
}

/*===============================================================
[Function Name] : MakeUdpSocket
[Description]   : 
    - UDP 소켓을 생성하고 서버 주소에 바인딩한다.
[Input]         : 
    - 없음
[Output]        : 
    - UDP 소켓이 생성되고 바인딩된 상태
[Call By]       : 
    - main()
[Calls]         : 
    - socket(), bind(), perror(), exit()
[Given]         : 
    - "select.h" 파일에 SERV_UDP_PORT가 정의되어 있어야 함
[Returns]       : 
    - 없음
===============================================================*/
void
MakeUdpSocket()
{
	struct sockaddr_in	servAddr;

	// UDP 소켓 생성
	if ((UdpSockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	// 서버 주소 구조체 초기화 및 설정
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sin_family = PF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERV_UDP_PORT);

	// 소켓과 서버 주소 바인딩
	if (bind(UdpSockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)  {
		perror("bind");
		exit(1);
	}
}

/*===============================================================
[Function Name] : MakeUcoSocket
[Description]   : 
    - UNIX 도메인 연결 지향형 소켓을 생성하고 서버 주소에 바인딩한 후 연결 요청을 대기 상태로 만든다.
[Input]         : 
    - 없음
[Output]        : 
    - UNIX 도메인 연결 지향형 소켓이 생성되고 바인딩된 상태
[Call By]       : 
    - main()
[Calls]         : 
    - socket(), bind(), listen(), strcpy(), strlen(), perror(), exit()
[Given]         : 
    - "select.h" 파일에 UNIX_STR_PATH가 정의되어 있어야 함
[Returns]       : 
    - 없음
===============================================================*/
void
MakeUcoSocket()
{
	struct sockaddr_un	servAddr;
	int					servAddrLen;

	// UNIX 도메인 연결 지향형 소켓 생성
	if ((UcoSockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	// 서버 주소 구조체 초기화 및 설정
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sun_family = PF_UNIX;
	strcpy(servAddr.sun_path, UNIX_STR_PATH); // 소켓 파일 경로 설정
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

	// 소켓과 서버 주소 바인딩
	if (bind(UcoSockfd, (struct sockaddr *) &servAddr, servAddrLen) < 0)  {
		perror("bind");
		exit(1);
	}

	// 연결 요청 대기 상태로 설정
	listen(UcoSockfd, 5);
}

/*===============================================================
[Function Name] : MakeUclSocket
[Description]   : 
    - UNIX 도메인 비연결형 소켓을 생성하고 서버 주소에 바인딩한다.
[Input]         : 
    - 없음
[Output]        : 
    - UNIX 도메인 비연결형 소켓이 생성되고 바인딩된 상태
[Call By]       : 
    - main()
[Calls]         : 
    - socket(), bind(), strcpy(), strlen(), perror(), exit()
[Given]         : 
    - "select.h" 파일에 UNIX_DG_PATH가 정의되어 있어야 함
[Returns]       : 
    - 없음
===============================================================*/
void
MakeUclSocket()
{
	struct sockaddr_un	servAddr;
	int					servAddrLen;

	// UNIX 도메인 비연결형 소켓 생성
	if ((UclSockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	// 서버 주소 구조체 초기화 및 설정
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sun_family = PF_UNIX;
	strcpy(servAddr.sun_path, UNIX_DG_PATH); // 소켓 파일 경로 설정
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

	// 소켓과 서버 주소 바인딩
	if (bind(UclSockfd, (struct sockaddr *) &servAddr, servAddrLen) < 0)  {
		perror("bind");
		exit(1);
	}
}

/*===============================================================
[Function Name] : ProcessTcpRequest
[Description]   : 
    - TCP 소켓을 통해 들어온 클라이언트의 요청을 읽고, 응답을 전송한다.
[Input]         : 
    - 없음 (글로벌 변수 TcpSockfd 사용)
[Output]        : 
    - 클라이언트의 요청 메시지를 콘솔에 출력
    - 클라이언트에게 응답 메시지를 전송하고 "Replied." 메시지를 콘솔에 출력
[Call By]       : 
    - main()
[Calls]         : 
    - accept(), read(), write(), printf(), perror(), exit(), close()
[Given]         : 
    - TcpSockfd가 유효한 TCP 소켓 디스크립터를 가리키고 있다고 가정
[Returns]       : 
    - 없음
===============================================================*/
void
ProcessTcpRequest()
{
	int					newSockfd, cliAddrLen, n;
	struct sockaddr_in	cliAddr;
	MsgType				msg;

	cliAddrLen = sizeof(cliAddr);
	// 클라이언트 연결 수락
	newSockfd = accept(TcpSockfd, (struct sockaddr *) &cliAddr, &cliAddrLen);
	if (newSockfd < 0)  {
		perror("accept");
		exit(1);
	}
		
	// 클라이언트로부터 메시지 읽기
	if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  {
		perror("read");
		exit(1);
	}
	printf("Received TCP request: %s.....", msg.data);

	// 응답 메시지 작성
	msg.type = MSG_REPLY;
	sprintf(msg.data, "This is a reply from %d.", getpid());

	// 클라이언트에게 응답 메시지 전송
	if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
		perror("write");
		exit(1);
	}
	printf("Replied.\n");

	close(newSockfd); // 클라이언트 소켓 닫기
}

/*===============================================================
[Function Name] : ProcessUdpRequest
[Description]   : 
    - UDP 소켓을 통해 들어온 클라이언트의 요청을 읽고, 응답을 전송한다.
[Input]         : 
    - 없음 (글로벌 변수 UdpSockfd 사용)
[Output]        : 
    - 클라이언트의 요청 메시지를 콘솔에 출력
    - 클라이언트에게 응답 메시지를 전송하고 "Replied." 메시지를 콘솔에 출력
[Call By]       : 
    - main()
[Calls]         : 
    - recvfrom(), sendto(), printf(), perror(), exit()
[Given]         : 
    - UdpSockfd가 유효한 UDP 소켓 디스크립터를 가리키고 있다고 가정
[Returns]       : 
    - 없음
===============================================================*/
void
ProcessUdpRequest()
{
	int					cliAddrLen, n;
	struct sockaddr_in	cliAddr;
	MsgType				msg;

	cliAddrLen = sizeof(cliAddr);
	// 클라이언트로부터 메시지 수신
	if ((n = recvfrom(UdpSockfd, (char *)&msg, sizeof(msg), 
				0, (struct sockaddr *)&cliAddr, &cliAddrLen)) < 0)  {
		perror("recvfrom");
		exit(1);
	}
	printf("Received UDP request: %s.....", msg.data);

	// 응답 메시지 작성
	msg.type = MSG_REPLY;
	sprintf(msg.data, "This is a reply from %d.", getpid());

	// 클라이언트에게 응답 메시지 전송
	if (sendto(UdpSockfd, (char *)&msg, sizeof(msg),
				0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0)  {
		perror("sendto");
		exit(1);
	}
	printf("Replied.\n");
}

/*===============================================================
[Function Name] : ProcessUcoRequest
[Description]   : 
    - UNIX 도메인 연결 지향형 소켓을 통해 들어온 클라이언트의 요청을 읽고, 응답을 전송한다.
[Input]         : 
    - 없음 (글로벌 변수 UcoSockfd 사용)
[Output]        : 
    - 클라이언트의 요청 메시지를 콘솔에 출력
    - 클라이언트에게 응답 메시지를 전송하고 "Replied." 메시지를 콘솔에 출력
[Call By]       : 
    - main()
[Calls]         : 
    - accept(), read(), write(), printf(), perror(), exit(), close()
[Given]         : 
    - UcoSockfd가 유효한 UNIX 도메인 연결 지향형 소켓 디스크립터를 가리키고 있다고 가정
[Returns]       : 
    - 없음
===============================================================*/
void
ProcessUcoRequest()
{
	int					newSockfd, cliAddrLen, n;
	struct sockaddr_un	cliAddr;
	MsgType				msg;

	cliAddrLen = sizeof(cliAddr);
	// 클라이언트 연결 수락
	newSockfd = accept(UcoSockfd, (struct sockaddr *) &cliAddr, &cliAddrLen);
	if (newSockfd < 0)  {
		perror("accept");
		exit(1);
	}
		
	// 클라이언트로부터 메시지 읽기
	if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  {
		perror("read");
		exit(1);
	}
	printf("Received UNIX-domain CO request: %s.....", msg.data);

	// 응답 메시지 작성
	msg.type = MSG_REPLY;
	sprintf(msg.data, "This is a reply from %d.", getpid());

	// 클라이언트에게 응답 메시지 전송
	if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
		perror("write");
		exit(1);
	}
	printf("Replied.\n");

	close(newSockfd); // 클라이언트 소켓 닫기
}

/*===============================================================
[Function Name] : ProcessUclRequest
[Description]   : 
    - UNIX 도메인 비연결형 소켓을 통해 들어온 클라이언트의 요청을 읽고, 응답을 전송한다.
[Input]         : 
    - 없음 (글로벌 변수 UclSockfd 사용)
[Output]        : 
    - 클라이언트의 요청 메시지를 콘솔에 출력
    - 클라이언트에게 응답 메시지를 전송하고 "Replied." 메시지를 콘솔에 출력
[Call By]       : 
    - main()
[Calls]         : 
    - recvfrom(), sendto(), printf(), perror(), exit()
[Given]         : 
    - UclSockfd가 유효한 UNIX 도메인 비연결형 소켓 디스크립터를 가리키고 있다고 가정
[Returns]       : 
    - 없음
===============================================================*/
void
ProcessUclRequest()
{
	int					cliAddrLen, n;
	struct sockaddr_un	cliAddr;
	MsgType				msg;

	cliAddrLen = sizeof(cliAddr);
	// 클라이언트로부터 메시지 수신
	if ((n = recvfrom(UclSockfd, (char *)&msg, sizeof(msg), 
				0, (struct sockaddr *)&cliAddr, &cliAddrLen)) < 0)  {
		perror("recvfrom");
		exit(1);
	}
	printf("Received UNIX-domain CL request: %s.....", msg.data);

	// 응답 메시지 작성
	msg.type = MSG_REPLY;
	sprintf(msg.data, "This is a reply from %d.", getpid());

	// 클라이언트에게 응답 메시지 전송
	if (sendto(UclSockfd, (char *)&msg, sizeof(msg),
				0, (struct sockaddr *)&cliAddr, cliAddrLen) < 0)  {
		perror("sendto");
		exit(1);
	}
	printf("Replied.\n");
}

/*===============================================================
[Function Name] : main
[Description]   : 
    - 서버 소켓들을 초기화하고 select 시스템 호출을 통해 
      다중 소켓을 모니터링하여 클라이언트의 요청을 처리한다.
[Input]         : 
    - 없음
[Output]        : 
    - "Server daemon started....." 메시지를 콘솔에 출력
    - 각 클라이언트 요청에 대한 처리 상태 메시지를 콘솔에 출력
[Call By]       : 
    - 시스템 호출에 의해 자동으로 호출
[Calls]         : 
    - signal(), MakeTcpSocket(), MakeUdpSocket(), MakeUcoSocket(), MakeUclSocket(), select(), FD_ZERO(), FD_SET(), FD_ISSET(), ProcessTcpRequest(), ProcessUdpRequest(), ProcessUcoRequest(), ProcessUclRequest(), perror(), exit()
[Given]         : 
    - "select.h" 파일에 필요한 정의들이 모두 포함되어 있어야 함
[Returns]       : 
    - 없음 (무한 루프 내에서 실행)
===============================================================*/
int main(int argc, char *argv[])
{
	fd_set	fdvar;
	int		count;

	// SIGINT 시그널 처리 등록 (Ctrl+C 시 CloseServer 호출)
	signal(SIGINT, CloseServer);

	// 각 소켓 생성 및 설정
	MakeTcpSocket();
	MakeUdpSocket();
	MakeUcoSocket();
	MakeUclSocket();

	printf("Server daemon started.....\n");

	while (1)  {
		// 파일 디스크립터 집합 초기화
		FD_ZERO(&fdvar);
		FD_SET(TcpSockfd, &fdvar);   // TCP 소켓 추가
		FD_SET(UdpSockfd, &fdvar);   // UDP 소켓 추가
		FD_SET(UcoSockfd, &fdvar);   // UNIX 도메인 연결 지향형 소켓 추가
		FD_SET(UclSockfd, &fdvar);   // UNIX 도메인 비연결형 소켓 추가

		// select 호출: 파일 디스크립터 집합 중 이벤트가 발생한 것을 감지
		if ((count = select(10, &fdvar, (fd_set *)NULL, (fd_set *)NULL, 
			(struct timeval *)NULL)) < 0)  {
			perror("select");
			exit(1);
		}

		// 이벤트가 발생한 파일 디스크립터에 대해 요청 처리
		while (count--)  {
			if (FD_ISSET(TcpSockfd, &fdvar))  {
				ProcessTcpRequest();
			}
			else if (FD_ISSET(UdpSockfd, &fdvar))  {
				ProcessUdpRequest();
			}
			else if (FD_ISSET(UcoSockfd, &fdvar))  {
				ProcessUcoRequest();
			}
			else if (FD_ISSET(UclSockfd, &fdvar))  {
				ProcessUclRequest();
			}
		}
	}
}

