/*===============================================================
[Program Name] : ucls.c
[Description]  : 
    - UNIX 도메인 소켓을 이용한 비연결형(Connection-Less) 서버를 구현하여 클라이언트의 요청을 받고 응답을 전송한다.
[Input]        : 
    - 클라이언트의 요청 메시지
    - 서버는 클라이언트의 주소 정보를 사용하여 응답을 전송
[Output]       : 
    - 클라이언트로 응답 메시지 전송
    - "UNIX-domain Connection-Less Server started....." 등의 상태 메시지를 콘솔에 출력
[Calls]        : 
    - socket(), bind(), recvfrom(), sendto(), close(), signal(), remove()
[특기사항]     : 
    - 서버 종료를 위해 SIGINT 시그널(Ctrl+C) 처리
    - "unix.h" 파일에 MsgType과 UNIX_DG_PATH 등의 정의가 필요
    - UNIX 도메인 소켓 경로는 UNIX_DG_PATH로 정의
    - 비연결형 프로토콜을 사용하여 클라이언트와의 지속적인 연결 없이 데이터그램 단위로 통신
    - 서버 종료 시 소켓 파일을 삭제하여 리소스 정리
===============================================================*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include "unix.h"

int	Sockfd; // 서버 소켓 파일 디스크립터

/*===============================================================
[Function Name] : CloseServer
[Description]   : 
    - 서버 소켓을 닫고 소켓 파일을 삭제한 후 종료 메시지를 출력하며 프로그램을 종료한다.
[Input]         : 
    - 없음
[Output]        : 
    - "UNIX-domain Connection-Less Server exit....." 메시지 출력
[Call By]       : 
    - SIGINT 시그널 핸들러
[Calls]         : 
    - close(), remove(), printf(), exit()
[Given]         : 
    - 글로벌 변수 Sockfd가 유효한 소켓 디스크립터를 가리키고 있다고 가정
[Returns]       : 
    - 없음 (프로그램 종료)
===============================================================*/
void CloseServer()
{
	close(Sockfd); // 서버 소켓 닫기
	if (remove(UNIX_DG_PATH) < 0)  { // 소켓 파일 삭제
		perror("remove");
	}

	printf("\nUNIX-domain Connection-Less Server exit.....\n");
	exit(0); // 프로그램 종료
}

int main(int argc, char *argv[])
{
	int					servAddrLen, cliAddrLen, n; // 서버 주소 길이, 클라이언트 주소 길이, 읽은 바이트 수
	struct sockaddr_un	cliAddr, servAddr; // 클라이언트 및 서버 주소 구조체
	MsgType				msg; // 메시지 구조체 (unix.h에서 정의됨)

	signal(SIGINT, CloseServer); // SIGINT 시그널 처리 등록 (Ctrl+C 시 CloseServer 호출)

	// UNIX 도메인 소켓 생성
	if ((Sockfd = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)  {
		perror("socket");
		exit(1);
	}

	// 서버 주소 구조체 초기화 및 설정
	bzero((char *)&servAddr, sizeof(servAddr));
	servAddr.sun_family = PF_UNIX;
	strcpy(servAddr.sun_path, UNIX_DG_PATH); // 소켓 파일 경로 설정
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

	// 소켓과 주소 바인딩
	if (bind(Sockfd, (struct sockaddr *) &servAddr, servAddrLen) < 0)  {
		perror("bind");
		exit(1);
	}

	printf("UNIX-domain Connection-Less Server started.....\n");

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

