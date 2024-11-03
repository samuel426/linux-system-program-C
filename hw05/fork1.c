#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>


/*===============================================================
[Program Name] : fork1.c 프로세스 생성 예제
[Description]  : fork() 함수를 사용하여 자식 프로세스를 생성하는 방법을 보여줌
[Input]        : 없음. 이 프로그램은 사용자 입력을 받지 않음.
[Output]       : 부모와 자식 프로세스의 PID(프로세스 ID)를 출력.
                 - 부모 프로세스 출력: "I am [부모 PID]. My child is [자식 PID]."
                 - 자식 프로세스 출력: "I am [자식 PID]. My parent is [부모 PID]."
[Calls]        : 
    - fork()    : 자식 프로세스를 생성
    - getpid()  : 호출 프로세스의 PID를 반환
    - getppid() : 호출 프로세스의 부모 PID를 반환
    - perror()  : 오류 메시지를 표준 오류로 출력
    - exit()    : 프로그램을 특정 상태 코드로 종료
[특기사항]     : 
    - fork() 함수는 새로운 프로세스를 생성하는 데 사용됨.
    - fork() 호출 후 자식 프로세스는 0을 받고, 부모 프로세스는 자식의 PID를 받음.
    - 자식과 부모 프로세스는 fork() 결과에 따라 각각 다른 코드를 실행함.
    - 이 코드는 유닉스 계열 환경에서 기본적인 프로세스 처리 예제를 설명하기 위해 작성됨.
    - <sys/types.h>: 다양한 데이터 타입 정의를 포함
    - <unistd.h>   : 유닉스 표준 함수들을 제공
    - <stdlib.h>   : 프로세스 종료 및 메모리 관리 함수 포함

==================================================================*/

int main()
{
	int		pid;

	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		/* child */
		printf("I am %d. My parent is %d.\n", getpid(), getppid());
	}
	else  {
		/* parent */
		printf("I am %d. My child is %d.\n", getpid(), pid);
	}
}
