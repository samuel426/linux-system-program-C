#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int		Var = 6;
char	Buf[] = "a write to stdout\n";


/*===============================================================
[Program Name] : fork.c 프로세스 변수 분리 예제
[Description]  : fork()를 이용해 자식 프로세스를 생성하고, 
                 부모와 자식이 서로 독립된 변수 값을 유지하는 것을 보여줌
[Input]        : 없음. 사용자 입력을 받지 않음.
[Output]       : 부모와 자식 프로세스 각각의 PID와 두 변수 Var, var의 값을 출력.
                 - "pid = [PID], Var = [Var 값], var = [var 값]"
[Calls]        : 
    - fork()   : 자식 프로세스를 생성
    - write()  : Buf 배열의 내용을 표준 출력으로 출력
    - getpid() : 호출 프로세스의 PID를 반환
    - perror() : 오류 메시지를 표준 오류로 출력
    - exit()   : 프로그램을 특정 상태 코드로 종료
    - sleep()  : 프로세스 일시 중지
[특기사항]     : 
    - fork() 후 부모와 자식 프로세스는 별도의 주소 공간을 가지며, 
      서로 다른 var와 Var 값을 유지함.
    - write()는 버퍼링 없이 즉시 출력되어 fork() 후 부모, 자식이 
      각각 별도의 "Before fork" 출력을 하지 않음.
    - <sys/types.h>: 다양한 데이터 타입 정의를 포함
    - <unistd.h>   : 유닉스 표준 함수들을 제공
    - <stdlib.h>   : 프로세스 종료 및 메모리 관리 함수 포함

==================================================================*/

int main()
{
	int		var;
	pid_t	pid;

	var = 88;
	write(STDOUT_FILENO, Buf, sizeof(Buf)-1);
	printf("Before fork\n");

	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		/* child */
		Var++;
		var++;
	}
	else  {
		/* parent */
		sleep(2);
	}

	printf("pid = %d, Var = %d, var = %d\n", getpid(), Var, var);
}
