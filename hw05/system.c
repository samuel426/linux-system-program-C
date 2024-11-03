#include <stdio.h>
#include <stdlib.h>
/*===============================================================
[Program Name] : system.c 스템 명령 실행 예제
[Description]  : system() 함수를 사용하여 외부 시스템 명령어를 실행하고 
                 그 결과를 확인하는 프로그램.
[Input]        : Nothing. (시스템 명령어로 "date", "nosuchcommand", "who"를 사용)
[Output]       : 각 명령어 실행의 종료 상태 출력.
[Calls]        : 
                 - system(): 쉘을 통해 명령어를 실행.
[특기사항]     : 
    - system()은 명령어를 쉘을 통해 실행하고, 종료 상태를 반환함.
    - <stdlib.h>: 메모리 할당, 프로세스 제어 및 기타 유틸리티 함수 정의.
==================================================================*/
int main()
{
	int		status;

	if ((status = system("date")) < 0)  {
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);

	if ((status = system("nosuchcommand")) < 0)  {
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);

	/* $ who; exit 44 */
	if ((status = system("who")) < 0)  {
		perror("system");
		exit(1);
	}
	printf("exit status = %d\n", status);
}
