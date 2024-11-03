#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
/*===============================================================
[Function Name] : void CharAtaTime(char *str)
[Description]   : 입력받은 문자열을 한 글자씩 출력하며, 각 문자 출력 전에 
                  지연을 추가하는 함수.
[Input]         : char *str - 출력할 문자열.
[Output]        : 해당 문자열이 표준 출력으로 출력됨.
[Call By]       : main() 함수에서 호출됨.
[Calls]         : putc() - 단일 문자를 출력.
[Given]         : 
    - 함수는 표준 출력을 실시간으로 출력하기 위해 버퍼링을 해제함.
[Returns]       : Nothing
==================================================================*/
void CharAtaTime(char *str)
{
	char	*ptr;
	int		c, i;

	setbuf(stdout, NULL);
	for (ptr = str ; c = *ptr++ ; )  {
		for(i = 0 ; i < 999999 ; i++)
			;
		putc(c, stdout);
	}
}
/*===============================================================
[Program Name] : 문자 단위 출력 예제
[Description]  : fork()로 자식 프로세스를 생성하고, 부모와 자식 프로세스가 각각 문자열을 
                 문자 단위로 출력하는 예제. 
[Input]        : 없음. 사용자 입력을 받지 않음.
[Output]       : 자식과 부모 프로세스가 각기 다른 문자열을 출력.
                 - "output from child" (자식 프로세스 출력)
                 - "output from parent" (부모 프로세스 출력)
[Calls]        : 
    - fork()   : 자식 프로세스를 생성
    - CharAtaTime() - 자식 및 부모 프로세스에서 호출됨.
    - putc()   : 단일 문자를 출력
    - wait()   : 자식 프로세스가 종료될 때까지 대기
    - perror() : 오류 메시지를 표준 오류로 출력
    - exit()   : 프로그램을 특정 상태 코드로 종료
[특기사항]     : 
    - setbuf(stdout, NULL)으로 표준 출력을 버퍼링하지 않도록 설정하여 
      실시간으로 출력 결과를 볼 수 있도록 함.
    - 문자 단위 출력을 위해 `CharAtaTime` 함수를 호출하여 각 문자 사이에 지연을 추가.
    - <sys/types.h>: 다양한 데이터 타입 정의를 포함
    - <unistd.h>   : 유닉스 표준 함수들을 제공
    - <sys/wait.h> : 자식 프로세스의 종료를 기다리기 위한 헤더 파일
    - <stdlib.h>   : 프로세스 종료 및 메모리 관리 함수 포함
==================================================================*/
int main()
{
	pid_t	pid;


	if ((pid = fork()) < 0)  {
		perror("fork");
		exit(1);
	}
	else if (pid == 0)  {
		CharAtaTime("output from child\n");
	} 
	else  {
		wait(NULL);
		CharAtaTime("output from parent\n");
	}
}
