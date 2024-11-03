#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : wait.c 자식 프로세스 상태 확인 예제
[Description]  : fork()로 자식 프로세스를 생성하고, 부모 프로세스가 자식 프로세스의 종료를 기다린 후 
                 종료 상태를 확인하는 예제.
[Input]        : 없음. 사용자 입력을 받지 않음.
[Output]       : 자식 프로세스가 생성되었다는 메시지와 종료 상태를 출력.
                 - "I'm a child" (자식 프로세스 출력)
                 - "A child killed with [status] status" (부모 프로세스 출력)
[Calls]        : 
    - fork()   : 자식 프로세스를 생성
    - sleep()  : 자식 프로세스의 실행을 2초 지연
    - wait()   : 자식 프로세스의 종료를 기다림
    - perror() : 오류 메시지를 표준 오류로 출력
    - exit()   : 프로그램을 특정 상태 코드로 종료
[특기사항]     : 
    - 현재 코드에서는 wait() 호출이 없으므로 자식 프로세스의 종료를 기다리지 않음.
    - 자식 프로세스가 종료된 후 부모 프로세스가 상태를 출력하는 로직이 필요함.
    - <sys/types.h>: 다양한 데이터 타입 정의를 포함
    - <unistd.h>   : 유닉스 표준 함수들을 제공
    - <sys/wait.h> : wait() 및 관련 함수 사용을 위한 헤더 파일
    - <stdlib.h>   : 프로세스 종료 및 메모리 관리 함수 포함
==================================================================*/
int main()
{
	pid_t	pid;
	int		status;

	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		printf("I'm a child\n");
		sleep(2);
	}
	else  {
		/* wait */
		printf("A child killed with %d status\n", status);
	}
}
