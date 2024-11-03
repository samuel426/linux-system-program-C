#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

/*===============================================================
[Program Name] : exec.c 프로세스 생성 및 실행 예제
[Description]  : fork()를 사용하여 자식 프로세스를 생성하고, 
                 execl, execv, execlp를 통해 다른 프로그램을 실행하는 기능.
[Input]        : Nothing. (exec.c와 exec2.c 파일을 사용하는 예제)
[Output]       : 
                 - 각 exec 호출의 결과로 실행된 프로그램의 출력
[Calls]        : 
                 - fork(): 자식 프로세스를 생성.
                 - exec(): 프로세스 이미지를 실행하여 새로운 프로그램으로 대체.
                 - waitpid(): 자식 프로세스가 종료될 때까지 대기.
[특기사항]     : 
    - exec 계열 함수는 현재 프로세스를 대체하며, 실행할 프로그램의 경로와 인수를 정확히 제공해야 함.
    - <sys/types.h>: POSIX 시스템에서 데이터 유형을 정의.
    - <unistd.h>: POSIX 운영 체제 API를 위한 기본 함수와 상수 정의.
    - <stdlib.h>: 일반적인 유틸리티 함수 및 동적 메모리 할당을 위한 함수 정의.
==================================================================*/
int main() 
{
	pid_t pid;
	char *argv[] = { "mycp", "exec.c", "exec2.c", NULL };

	// execl
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	}
	else if (pid == 0) {
		// mycp exec.c exec2.c
		if (execl("./mycp", "mycp", "exec.c", "exec2.c", NULL) == -1) {	
			perror("execl");
			exit(1);
		}
	}
	if (waitpid(pid, NULL, 0) < 0) {
		perror("waitpid");
		exit(1);
	}

	// execv
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	}
	else if (pid == 0) {
		// mycp exec.c exec2.c
		if (execv("./mycp", argv) == -1) {
			perror("execv");
			exit(1);
		}
	}
	if (waitpid(pid, NULL, 0) < 0) {
		perror("waitpid");
		exit(1);
	}

	// execlp
	if ((pid = fork()) < 0) {
		perror("waitpid");
		exit(1);
	}
	else if (pid == 0) {
		// execute 'env' from PATH
		if(execlp("env", "env", NULL) == -1) {
			perror("execlp");
			exit(1);
		}
	}
	if (waitpid(pid, NULL, 0) < 0) {
		perror("waitpid");
		exit(1);
	}
	return 0;
}
