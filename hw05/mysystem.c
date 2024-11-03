#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*===============================================================
[Function Name] : int mysystem(char *cmd)
[Description]   : cmd에서 지정한 명령을 실행하고 종료 상태를 반환.
[Input]         : char *cmd - 실행할 명령어 문자열.
[Output]        : 명령어 실행의 종료 상태.
[Call By]       : main()
[Calls]         : fork(), execv(), waitpid()
[Given]         : 없음.
[Returns]       : 성공 시 0을 반환, 실패 시 -1을 반환.
==================================================================*/
int mysystem(char *cmd) {
    pid_t pid; // 자식 프로세스 ID
    int status; // 자식 프로세스 종료 상태
    char *argv[3]; // execv에 전달할 인자 배열

    argv[0] = "/bin/sh"; // 기본 셸
    argv[1] = "-c"; // 셸 옵션
    argv[2] = cmd; // 실행할 명령어
    argv[3] = NULL; // 마지막 요소는 NULL로 종료

    if ((pid = fork()) < 0) {
        perror("fork"); // fork 실패 시 에러 메시지 출력
        return -1;
    } else if (pid == 0) {
        // 자식 프로세스에서 명령 실행
        execv(argv[0], argv);
        perror("execv"); // execv가 실패하면 에러 메시지 출력
        exit(1);
    }

    // 부모 프로세스에서 자식 프로세스가 종료될 때까지 대기
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid"); // waitpid 실패 시 에러 메시지 출력
        return -1;
    }

    return status; // 명령어 실행의 종료 상태 반환
}
/*===============================================================
[Program Name] : mysystem.c
[Description]  : 사용자가 입력한 명령어를 시스템 호출을 통해 실행하는 함수.
[Input]        : char *cmd - 실행할 명령어를 담고 있는 문자열.
[Output]       : 명령어 실행 결과를 출력하며, 종료 상태를 반환.
[Calls]        : fork(), execv(), waitpid(), perror(), exit()
[특기사항]     : 
    - 시스템 명령어를 실행하기 위해 fork와 execv를 사용.
    - 프로세스의 종료 상태를 확인하기 위해 waitpid를 사용.
==================================================================*/

int main() {
    char command[256]; // 입력할 명령어를 저장할 배열

    printf("Enter a command: ");
    fgets(command, sizeof(command), stdin); // 사용자로부터 명령어 입력 받기

    // 명령어 실행
    int status = mysystem(command);
    printf("Command exited with status: %d\n", status); // 종료 상태 출력

    return 0;
}

