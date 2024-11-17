#include <stdio.h>
#include <signal.h>

/*===============================================================
[Function Name]   : SigAlarmHandler
[Description]     : SIGALRM 시그널을 수신했을 때 호출되는 함수로, 
                    아무 작업 없이 리턴하여 pause() 호출을 깨어나게 함.
[Input]           : signo - 수신한 시그널 번호 (SIGALRM)
[Output]          : 없음
[Call By]         : signal 시스템 호출을 통해 SIGALRM 신호가 수신될 때 호출됨
[Calls]           : 없음
[Given]           : 없음
[Returns]         : 없음
==================================================================*/
void SigAlarmHandler(int signo)
{
        /* nothing to do, just return to wake up the pause */
        return;
}

/*===============================================================
[Function Name]   : mysleep
[Description]     : 주어진 시간(nsecs) 동안 SIGALRM 신호를 이용해 잠시 대기하는 함수
[Input]           : nsecs - 대기할 시간 (초)
[Output]          : 대기한 시간(초)
[Call By]         : main 함수에서 호출됨
[Calls]           : signal - SIGALRM 신호에 대한 처리 함수 등록
                    alarm - SIGALRM 신호를 발생시키는 타이머 설정
                    pause - 신호를 기다리기 위한 시스템 호출
[Given]           : 없음
[Returns]         : 대기한 시간(초)
==================================================================*/
unsigned int mysleep(unsigned int nsecs)
{
        /* SIGALRM signal handler: SigAlarmHandler */
        if (signal(SIGALRM, SigAlarmHandler) == SIG_ERR)  {
                return nsecs;
        }

        alarm(5);  // 5초 후 SIGALRM 시그널 발생

        pause();  // 시그널을 기다리며 대기

        return alarm(0);  // 알람을 취소하고 남은 시간 반환
}

/*===============================================================
[Program Name]    : mysleep.c
[Description]     : mysleep 함수는 SIGALRM 신호를 이용하여 주어진 시간 동안 대기하는 프로그램
[Input]           : 없음 (대기할 시간은 프로그램 코드 내에서 설정됨)
[Output]          : "Wait for 5 seconds..." 메시지 출력 후, 5초 대기
[Calls]           : mysleep - 대기 시간을 설정하고, SIGALRM 시그널을 처리함
[특기사항]        : 
    - SIGALRM 시그널을 처리하기 위해 `SigAlarmHandler`를 사용.
    - `alarm` 함수로 시그널 타이머를 설정하고, `pause`로 시그널을 기다림.
==================================================================*/

int main()
{
        printf("Wait for 5 seconds...\n");

        mysleep(5);  // 5초 동안 대기

        return 0;
}

