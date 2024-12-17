/*===============================================================
[Program Name] : myusleep.c
[Description]  : 
    - 사용자 정의 함수 myusleep을 통해 지정된 시간 동안 프로그램의 실행을 일시 중지한다.
    - main 함수에서는 myusleep 함수를 호출하여 1.5초 동안 일시 중지한 후 종료한다.
[Input]        : 
    - myusleep 함수:
        - int secs   : 일시 중지할 초 단위 시간
        - int usecs  : 일시 중지할 마이크로초 단위 시간
[Output]       : 
    - "Sleep for 1.5 seconds....." 메시지를 콘솔에 출력하고 1.5초 동안 일시 중지한다.
[Calls]        : 
    - main 함수:
        - myusleep()
    - myusleep 함수:
        - select()
[특기사항]     : 
    - myusleep 함수는 select 시스템 호출을 이용하여 지정된 시간 동안 대기한다.
    - select 함수의 인자로 0을 전달하여 파일 디스크립터를 감시하지 않고 타이머만 사용한다.
    - 마이크로초 단위로 세밀한 시간 조절이 가능하다.
    - 오류 발생 시 perror를 통해 에러 메시지를 출력하고 함수에서 반환한다.
===============================================================*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

/*===============================================================
[Function Name] : myusleep
[Description]   : 
    - 지정된 초와 마이크로초 동안 프로그램의 실행을 일시 중지한다.
[Input]         : 
    - int secs   : 일시 중지할 초 단위 시간
    - int usecs  : 일시 중지할 마이크로초 단위 시간
[Output]        : 
    - 지정된 시간 동안 실행을 일시 중지한다.
[Call By]       : 
    - main()
[Calls]         : 
    - select()
[Given]         : 
    - secs와 usecs는 음수가 아닌 정수 값이어야 한다.
[Returns]       : 
    - 없음 (타이머가 만료되면 함수가 반환된다.)
===============================================================*/
void
myusleep(int secs, int usecs)
{
        struct timeval  tm;

        tm.tv_sec = secs;
        tm.tv_usec = usecs;

        if (select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &tm) < 0)  {
                perror("select");
                return;
        }
}

int main()
{
        printf("Sleep for 1.5 seconds.....\n");
        myusleep(1, 500000);
}

