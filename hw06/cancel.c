#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>

pthread_t ThreadId[2];

/*===============================================================
[Function Name]   : SigIntHandler
[Description]     : SIGINT 신호가 수신되었을 때 호출되는 함수로, 두 개의 쓰레드를 종료하고 프로그램을 종료하는 역할을 함.
[Input]           : signo - 시그널 번호 (SIGINT)
[Output]          : 없음
[Call By]         : signal 시스템 호출을 통해 SIGINT 시그널 발생 시 호출됨
[Calls]           : pthread_cancel - 각 쓰레드의 실행을 취소
                    pthread_join - 쓰레드가 종료될 때까지 기다림
[Given]           : ThreadId - 종료할 쓰레드 ID 배열
[Returns]         : 없음
==================================================================*/
void SigIntHandler(int signo)
{
        int i;

        printf("Received a SIGINT signal by thread %ld\n", pthread_self());
        printf("Terminate other threads: tid=%ld, %ld\n", ThreadId[0], ThreadId[1]);

        for (i = 0 ; i < 2 ; i++)  {
                /* Cancel execution of all threads */
                if (pthread_cancel(ThreadId[i]))  {
                        perror("pthread_cancel");
                        exit(1);
                }
        }
        for (i = 0 ; i < 2 ; i++)  {
                if (pthread_join(ThreadId[i], NULL))  {
                        perror("pthread_join");
                        exit(1);
                }
        }
        printf("Threads terminated: tid=%ld, %ld\n", ThreadId[0], ThreadId[1]);

        exit(0);
}

/*===============================================================
[Function Name]   : Thread1
[Description]     : 첫 번째 쓰레드 함수로, 무한 루프를 돌며 계속 실행 중이며 취소 가능 상태로 설정됨.
[Input]           : dummy - 입력값 없음
[Output]          : 없음
[Call By]         : pthread_create 함수로 쓰레드 생성 시 호출됨
[Calls]           : pthread_setcancelstate - 쓰레드 취소 상태를 설정
                    pthread_setcanceltype - 쓰레드 취소 유형을 설정
[Given]           : 없음
[Returns]         : 없음
==================================================================*/
void Thread1(void *dummy)
{
        printf("Thread %ld created.....\n", pthread_self());

        if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))  {
                perror("pthread_setcancelstate");
                pthread_exit(NULL);
        }
        if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))  {
                perror("pthread_setcanceltype");
                pthread_exit(NULL);
        }

        while (1)
                ;  // 무한 루프
}

/*===============================================================
[Function Name]   : Thread2
[Description]     : 두 번째 쓰레드 함수로, 무한 루프를 돌며 계속 실행 중이며 취소 가능 상태로 설정됨.
[Input]           : dummy - 입력값 없음
[Output]          : 없음
[Call By]         : pthread_create 함수로 쓰레드 생성 시 호출됨
[Calls]           : pthread_setcancelstate - 쓰레드 취소 상태를 설정
                    pthread_setcanceltype - 쓰레드 취소 유형을 설정
[Given]           : 없음
[Returns]         : 없음
==================================================================*/
void Thread2(void *dummy)
{
        printf("Thread %ld created.....\n", pthread_self());

        if (pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL))  {
                perror("pthread_setcancelstate");
                pthread_exit(NULL);
        }
        if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL))  {
                perror("pthread_setcanceltype");
                pthread_exit(NULL);
        }

        while (1)
                ;  // 무한 루프
}

/*===============================================================
[Program Name]    : cancel.c
[Description]     : SIGINT 신호를 수신하면 두 개의 쓰레드를 취소하고 종료하는 프로그램.
[Input]           : 없음 (SIGINT 신호를 수신하여 처리)
[Output]          : "Received a SIGINT signal" 메시지 출력 후 쓰레드 종료
[Calls]           : pthread_create - 쓰레드를 생성
                    signal - SIGINT 시그널에 대한 처리 함수 등록
[특기사항]        : 
    - 쓰레드의 취소 상태와 취소 유형을 설정하여 비동기적으로 취소할 수 있도록 함.
    - SIGINT 신호에 의해 실행 중인 두 개의 쓰레드를 취소하고 종료하는 예제.
==================================================================*/

int main()
{
        if (pthread_create(&ThreadId[0], NULL, (void *)Thread1, NULL) < 0)  {
                perror("pthread_create");
                exit(1);
        }
        if (pthread_create(&ThreadId[1], NULL, (void *)Thread2, NULL) < 0)  {
                perror("pthread_create");
                exit(1);
        }

        signal(SIGINT, SigIntHandler);  // SIGINT 시그널을 처리할 핸들러 등록

        printf("Press ^C to quit\n");

        for ( ; ; )
                pause();  // 시그널 대기
}

