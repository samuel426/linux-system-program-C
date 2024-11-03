#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define	MAX_CMD		256

/*===============================================================
[Function Name] : void DoCmd(char *cmd)
[Description]   : 주어진 명령어를 실행하고 완료 메시지를 출력하는 스레드 함수.
[Input]         : char *cmd - 실행할 명령어 문자열.
[Output]        : Nothing.
[Call By]       : main()
[Calls]         : printf(), sleep(), pthread_exit()
[Given]         : 
    - cmd는 사용자로부터 입력받은 명령어.
[Returns]       : Nothing.
==================================================================*/
void DoCmd(char *cmd)
{
	printf("Doing %s", cmd);
	sleep(1);
	printf("Done\n");

	pthread_exit(NULL);
}
/*===============================================================
[Program Name] : cmd_t.c 스레드 기반 명령어 실행기
[Description]  : 사용자가 입력한 명령어를 별도의 스레드에서 수행하는 프로그램.
[Input]        : Nothing. (사용자로부터 명령어 입력을 받음)
[Output]       : 수행 중인 명령어와 완료 메시지를 출력.
[Calls]        : 
                 - DoCmd(): 명령어를 수행하는 스레드 함수.
[특기사항]     : 
    - 프로그램 종료를 원할 경우 'q'를 입력.
    - pthread_create()를 사용하여 명령어를 실행하는 새로운 스레드를 생성.
==================================================================*/
int main()
{
	char		cmd[MAX_CMD];
	pthread_t	tid;

	while (1)  {
		printf("CMD> ");
		fgets(cmd, MAX_CMD, stdin);	
		if (cmd[0] == 'q')
			break;

		if (pthread_create(&tid, NULL, (void *)DoCmd, (void *)cmd) < 0)  {
			perror("pthread_create");
			exit(1);
		}

#if 0
		pthread_join(tid, NULL);
#endif
	}
}
