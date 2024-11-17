#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

/*===============================================================
[Function Name]   : SigUsrHandler
[Description]     : SIGUSR1과 SIGUSR2 시그널을 처리하는 핸들러로, 수신된 시그널에 따라
                    각각 "Received a SIGUSR1 signal" 또는 "Received a SIGUSR2 signal"을 출력.
                    그 외의 시그널을 수신하면 "Received unknown signal" 메시지를 출력하고
                    프로세스를 종료.
[Input]           : signo - 수신한 시그널 번호 (SIGUSR1, SIGUSR2, 또는 기타)
[Output]          : 없음
[Call By]         : signal 시스템 호출을 통해 SIGUSR1과 SIGUSR2 시그널이 수신될 때 호출됨
[Calls]           : 없음
[Given]           : 없음
[Returns]         : 없음
==================================================================*/
void SigUsrHandler(int signo)
{
        if (signo == SIGUSR1)  {
                printf("Received a SIGUSR1 signal\n");
        }
        else if (signo == SIGUSR2)  {
                printf("Received a SIGUSR2 signal\n");
        }
        else  {
                printf("Received unknown signal\n");
                printf("Terminate this process\n");
                exit(0);  // 프로세스 종료
        }
}

/*===============================================================
[Program Name]    : sig2.c
[Description]     : SIGUSR1과 SIGUSR2 시그널을 처리하는 예제.
                    사용자가 SIGUSR1 또는 SIGUSR2 시그널을 보내면 각 시그널에 맞는 메시지를
                    출력하고, 그 외의 시그널을 받으면 "Received unknown signal" 메시지를 출력하고 종료.
[Input]           : 없음
[Output]          : SIGUSR1 또는 SIGUSR2 시그널을 수신 시 각 메시지 출력, 그 외의 시그널 시 종료 메시지 출력
[Calls]           : SigUsrHandler - SIGUSR1과 SIGUSR2 시그널을 처리하는 핸들러
[특기사항]        : 
    - 이 프로그램은 `signal` 시스템 호출을 사용하여 SIGUSR1과 SIGUSR2 시그널을 처리합니다.
    - SIGUSR1과 SIGUSR2 이외의 시그널을 받으면 프로세스를 종료합니다.
==================================================================*/

int main()
{
        /* SIGUSR1, SIGUSR2 시그널 핸들러 등록: SigUsrHandler */
        if (signal(SIGUSR1, SigUsrHandler) == SIG_ERR)  {
                perror("signal");
                exit(1);
        }

        if (signal(SIGUSR2, SigUsrHandler) == SIG_ERR)  {
                perror("signal");
                exit(1);
        }

        /* 시그널 수신 대기 */
        for ( ; ; )
                pause();  // 무한 대기 (시그널 수신 대기)
}

