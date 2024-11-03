#include <stdio.h>
#include <stdlib.h>

#define	MAX_CMD		256
/*===============================================================
[Function Name] : void DoCmd(char *cmd)
[Description]   : 주어진 명령어를 실행하고 완료 메시지를 출력하는 함수.
[Input]         : char *cmd - 실행할 명령어 문자열.
[Output]        : Nothing.
[Call By]       : main()
[Calls]         : printf(), sleep()
[Given]         : 
    - cmd는 사용자로부터 입력받은 명령어.
[Returns]       : Nothing.
==================================================================*/
void DoCmd(char *cmd)
{
	printf("Doing %s", cmd);
	sleep(1);
	printf("Done\n");
}
/*===============================================================
[Program Name] : cmd_p.c 명령어 실행기
[Description]  : 사용자가 입력한 명령어를 별도의 프로세스에서 수행하는 인터프리터.
[Input]        : Nothing. (사용자로부터 명령어 입력을 받음)
[Output]       : 수행 중인 명령어와 완료 메시지를 출력.
[Calls]        : 
                 - DoCmd(): 명령어를 수행하는 함수.
[특기사항]     : 
    - 프로그램 종료를 원할 경우 'q'를 입력.
    - fork()를 사용하여 명령어를 실행하는 새로운 프로세스를 생성.
==================================================================*/
int main()
{
	char	cmd[MAX_CMD];
	int		pid;

	while (1)  {
		printf("CMD> ");
		fgets(cmd, MAX_CMD, stdin);	
		if (cmd[0] == 'q')
			break;

		if ((pid = fork()) < 0)  {
			perror("fork");
			exit(1);
		}
		else if (pid == 0)  {
			DoCmd(cmd);
			exit(0);
		}
#if 0
		else  {
			wait(NULL);
		}
#endif
	}
}
