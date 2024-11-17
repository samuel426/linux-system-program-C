#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

static unsigned int AlarmSecs;

/*===============================================================
[Function Name]   : SigAlarmHandler
[Description]     : SIGALRM 시그널을 처리하는 함수. 이 함수는 알람이 울릴 때마다 호출되며, 주어진 주기(초)마다 점(.)을 출력함.
[Input]           : signo - 시그널 번호 (SIGALRM)
[Output]          : 없음
[Call By]         : signal 시스템 호출을 통해 SIGALRM 시그널이 발생하면 호출됨.
[Calls]           : signal - SIGALRM 시그널을 처리할 함수 등록
                    alarm  - SIGALRM 시그널을 설정한 시간 뒤에 발생하도록 알람을 설정
[Given]           : AlarmSecs - SIGALRM 시그널 발생 주기 (SetPeriodicAlarm 함수에서 설정됨)
[Returns]         : 없음
==================================================================*/
void SigAlarmHandler(int signo)
{
        if (signal(SIGALRM, SigAlarmHandler) == SIG_ERR)  {
                perror("signal");
                exit(1);
        }

        alarm(AlarmSecs);  // 알람 주기 설정 (SIGALRM 후 다시 알람 설정)

        /* Do something */
        printf(".");  // 알람이 울릴 때마다 점(.)을 출력
        fflush(stdout);  // 출력 버퍼를 비워서 즉시 출력되도록 함

        return;
}


/*===============================================================
[Function Name]   : SetPeriodicAlarm
[Description]     : 주어진 시간 간격(nsecs)마다 SIGALRM 신호를 발생시키는 주기적인 알람을 설정하는 함수.
[Input]           : nsecs - 알람 발생 주기 (초 단위)
[Output]          : 성공 시 0을 반환, 실패 시 -1을 반환
[Call By]         : main 함수에서 호출됨
[Calls]           : signal - SIGALRM 시그널 처리 함수 등록
                    alarm  - 주어진 시간 뒤에 SIGALRM 시그널 발생
[Given]           : 없음
[Returns]         : 0 - 성공, -1 - 실패 (알람 설정 실패 시)
==================================================================*/
int SetPeriodicAlarm(unsigned int nsecs)
{
        if (signal(SIGALRM, SigAlarmHandler) == SIG_ERR)  {
                return -1;  // 시그널 처리 함수 등록 실패 시 -1 반환
        }
        AlarmSecs = nsecs;  // 알람 발생 주기 저장

        alarm(nsecs);  // 주어진 시간 간격 뒤에 SIGALRM 발생

        return 0;  // 성공적으로 알람 설정
}


/*===============================================================
[Program Name]    : alarm.c
[Description]     : 주기적으로 SIGALRM 시그널을 발생시키며, 해당 시그널을 처리하여 점(.)을 출력하는 프로그램.
[Input]           : 없음 (알람 주기는 SetPeriodicAlarm 함수에서 지정)
[Output]          : 점(.)이 1초마다 출력됨
[Calls]           : SetPeriodicAlarm - 주기적인 알람 설정
[특기사항]        : 
    - 시스템 시그널을 사용하여 일정 주기로 작업을 실행하는 방법을 학습할 수 있음.
    - 시스템 호출인 signal과 alarm을 사용하여 주기적인 이벤트를 처리하는 예제.
==================================================================*/

int main()
{
        printf("Doing something every one seconds\n");

        SetPeriodicAlarm(1);  // 1초마다 알람 발생

        for ( ; ; )
                pause();  // 프로그램이 종료되지 않도록 대기 상태 유지
}

