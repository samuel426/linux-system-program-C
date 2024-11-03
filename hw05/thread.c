#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

/*===============================================================
[Function Name] : void PrintMsg(char *msg)
[Description]   : 주어진 메시지를 출력하고, 스레드를 종료하는 함수.
[Input]         : char *msg - 출력할 메시지.
[Output]        : 없음.
[Call By]       : main()
[Calls]         : printf(), pthread_exit()
[Given]         : 
    - msg는 스레드에 의해 전달되는 인자.
[Returns]       : 없음.
==================================================================*/
void PrintMsg(char *msg)
{
	printf("%s", msg);

	pthread_exit(NULL);
}
/*===============================================================
[Program Name] : thread.c 스레드 생성 및 메시지 출력 예제
[Description]  : 두 개의 스레드를 생성하여 각각의 메시지를 출력하는 프로그램.
[Input]        : 없음. (스레드 인자로 "Hello, "와 "World!" 메시지를 전달)
[Output]       : 각 스레드에서 출력된 메시지와 스레드 종료 상태.
[Calls]        : 
                 - pthread_create(): 새로운 스레드를 생성.
                 - pthread_join(): 스레드가 종료될 때까지 대기.
[특기사항]     : 
    - <pthread.h>: POSIX 스레드 관련 함수 정의.
==================================================================*/
int main()
{
	pthread_t	tid1, tid2;
	char		*msg1 = "Hello, ";
	char		*msg2 = "World!\n";

	/* Thread ID: tid1, Thread function: PrintMsg, Thread argument: msg1 */
	if (pthread_create(&tid1, NULL, (void *)PrintMsg, (void *)msg1) < 0)  {
		perror("pthread_create");
		exit(1);
	}

	if (pthread_create(&tid2, NULL, (void *)PrintMsg, (void *)msg2) < 0)  {
		perror("pthread_create");
		exit(1);
	}

	printf("Threads created: tid=%ld, %ld\n", tid1, tid2);
	
	/* Wait for tid1 to exit */
	if (pthread_join(tid1, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}
	if (pthread_join(tid2, NULL) < 0)  {
		perror("pthread_join");
		exit(1);
	}

	printf("Threads terminated: tid=%ld, %ld\n", tid1, tid2);
}
