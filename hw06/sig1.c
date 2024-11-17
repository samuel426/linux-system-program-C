#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

/*===============================================================
[Function Name]   : SigIntHandler
[Description]     : SIGINT 시그널을 처리하는 핸들러로, 해당 시그널을 수신하면
                    종료 메시지를 출력하고 프로세스를 종료.
[Input]           : signo - 수신한 시그널 번호 (SIGINT)
[Output]          : 없음
[Call By]         : signal 시스템 호출을 통해 SIGINT 신호가 수신될 때 호출됨
[Calls]           : 없음
[Given]           : 없음
[Returns]         : 없음
==================================================================*/
void SigIntHandler(int signo)
{
        printf("Received a SIGINT signal\n");
        printf("Terminate this process\n");

        exit(0);  // 프로세스 종료
}

/*===============================================================
[Program Name]    : sig1.c
[Description]     : SIGINT 시그널을 처리하여 프로그램을 종료하는 예제.
                    사용자가 ^C를 누르면 SIGINT 시그널이 발생하고, 이 시그널을
                    처리하여 종료 메시지를 출력한 후 프로세스를 종료.
[Input]           : 없음
[Output]          : "Press ^C to quit" 메시지 출력 후, ^C를 누르면 종료 메시지 출력
[Calls]           : SigIntHandler - SIGINT 시그널을 처리하여 종료하는 핸들러
[특기사항]        : 
    - 이 프로그램은 `signal` 시스템 호출을 사용하여 SIGINT 시그널을 처리하며, 
      사용자 입력에 따라 종료합니다.
==================================================================*/

int main()
{
        /* SIGINT 시그널 핸들러 등록: SigIntHandler */
        signal(SIGINT, SigIntHandler);  // SIGINT 시그널을 SigIntHandler로 처리

        printf("Press ^C to quit\n");

        for ( ; ; )
                pause();  // 무한 대기 (시그널 수신 대기)
}

