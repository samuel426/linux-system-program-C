#include <stdio.h>
#include <signal.h>
#include <pwd.h>
#include <stdlib.h>

/*===============================================================
[Function Name]   : MyAlarmHandler
[Description]     : SIGALRM 시그널을 처리하는 핸들러로, 주기적으로 SIGALRM 신호를 발생시키고
                    "root" 사용자 정보를 가져오는 작업을 수행.
[Input]           : signo - 수신한 시그널 번호 (SIGALRM)
[Output]          : 없음
[Call By]         : signal 시스템 호출을 통해 SIGALRM 신호가 수신될 때 호출됨
[Calls]           : getpwnam - "root" 사용자 정보를 가져옴
[Given]           : 없음
[Returns]         : 없음
==================================================================*/
void MyAlarmHandler(int signo)
{
        struct passwd   *rootptr;

        signal(SIGALRM, MyAlarmHandler);  // SIGALRM 시그널 핸들러 등록
        alarm(1);  // 1초 후 다시 SIGALRM 시그널 발생

        printf("in signal handler\n");

        // "root" 사용자 정보 가져오기
        if ((rootptr = getpwnam("root")) == NULL)  {
                perror("getpwnam");
                exit(1);
        }

        return;
}

/*===============================================================
[Program Name]    : nonreent.c
[Description]     : SIGALRM 시그널을 처리하여 주기적으로 시그널을 발생시키고, 
                    "cjs" 사용자 정보를 계속해서 가져오는 프로그램.
[Input]           : 없음 (시그널에 의한 사용자 정보 가져오기)
[Output]          : "in signal handler" 메시지 출력 후, "cjs" 사용자의 정보를 반복적으로 가져옴
[Calls]           : MyAlarmHandler - SIGALRM 신호를 처리하여 주기적으로 실행되는 핸들러
                    getpwnam - "cjs" 사용자 정보를 가져옴
[특기사항]        : 
    - 이 프로그램은 "cjs" 사용자의 정보를 반복적으로 가져오며, 
      "root" 사용자 정보도 동시에 가져오는 구조입니다.
    - SIGALRM 시그널을 사용하여 주기적으로 알람을 발생시키고, 
      핸들러에서 `getpwnam("root")`를 호출하여 root 정보를 가져옵니다.
==================================================================*/

int main()
{
        struct passwd   *ptr;

        signal(SIGALRM, MyAlarmHandler);  // SIGALRM 시그널 핸들러 등록
        alarm(1);  // 1초 후 SIGALRM 시그널 발생

        // "cjs" 사용자 정보 반복적으로 가져오기
        for ( ; ; )  {
                if ((ptr = getpwnam("cjs")) == NULL)  {
                        perror("getpwnam");
                        exit(1);
                }

                // 가져온 사용자 이름이 "cjs"인지 확인
                if (strcmp(ptr->pw_name, "cjs") != 0)  {
                        printf("return value corrupted!, pw_name = %s\n", ptr->pw_name);
                        exit(0);
                }
        }

        return 0;
}

