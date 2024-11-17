#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/*===============================================================
[Function Name]   : SigChldHandler
[Description]     : SIGCHLD 시그널을 처리하는 핸들러로, 자식 프로세스가 종료되었을 때 호출됩니다.
[Input]           : signo - 수신한 시그널 번호 (SIGCHLD)
[Output]          : 없음
[Call By]         : SIGCHLD 시그널을 수신할 때 호출됨
[Calls]           : waitpid - 종료된 자식 프로세스의 종료 상태를 확인
[Given]           : 없음
[Returns]         : 없음
==================================================================*/
void SigChldHandler(int signo)
{
    pid_t pid;
    int status;

    /* 종료된 자식 프로세스의 종료 상태를 확인 */
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("Child process %d terminated with exit status %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child process %d terminated by signal %d\n", pid, WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("Child process %d stopped by signal %d\n", pid, WSTOPSIG(status));
        }
    }
}

/*===============================================================
[Program Name]   : sigchld.c
[Description]     : 부모 프로세스가 자식 프로세스 종료를 SIGCHLD 시그널을 통해 비동기적으로 처리하는 프로그램.
                    자식 프로세스가 종료되면 SIGCHLD 시그널이 부모 프로세스로 전달되어 자식 프로세스의 종료 상태를 출력.
[Input]           : 없음
[Output]          : 자식 프로세스가 종료되었을 때, 부모 프로세스는 그 종료 상태를 출력.
[Calls]           : SigChldHandler - SIGCHLD 시그널을 처리하는 핸들러
[특기사항]   : 
                    - `SIGCHLD` 시그널을 비동기적으로 처리하기 위해 `signal()` 함수를 사용하여 시그널 핸들러를 등록합니다.
                    - `waitpid()`는 `WNOHANG` 옵션을 사용하여 자식 프로세스가 종료되지 않았을 때도 즉시 반환되도록 설정합니다.
                    - 부모 프로세스는 자식 프로세스의 종료를 기다리지 않고, 계속 실행되며 자식 프로세스의 종료 상태를 비동기적으로 처리합니다.
==================================================================*/

int main()
{
    pid_t pid;

    /* SIGCHLD 핸들러 등록 */
    if (signal(SIGCHLD, SigChldHandler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    else if (pid == 0) {
        /* 자식 프로세스 */
        printf("I'm a child\n");
        sleep(2);  // 2초 동안 대기 후 종료
        exit(0);
    }
    else {
        /* 부모 프로세스 */
        printf("Parent process: waiting for child to terminate asynchronously...\n");

        while (1);  // 부모 프로세스는 비동기적으로 자식 프로세스의 종료를 기다림
    }

    return 0;
}

