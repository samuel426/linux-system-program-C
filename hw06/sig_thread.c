#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>

#define THREAD_MAIN
/*
#define THREAD_1
#define THREAD_2
*/

/*===============================================================
[Function Name]   : SigIntHandler
[Description]     : SIGINT 시그널을 처리하는 핸들러로, 시그널을 받은 스레드 ID와 함께
                    "Received a SIGINT signal by thread" 메시지를 출력한 후 프로세스를 종료.
[Input]           : signo - 수신한 시그널 번호 (SIGINT)
[Output]          : 없음
[Call By]         : SIGINT 시그널을 수신할 때 호출됨
[Calls]           : 없음
[Given]           : 없음
[Returns]         : 없음
==================================================================*/
void SigIntHandler(int signo)
{
        printf("Received a SIGINT signal by thread %ld\n", pthread_self());
        printf("Terminate this process\n");

        exit(0);  // 프로세스 종료
}

/*===============================================================
[Function Name]   : Thread1
[Description]     : 스레드 1을 생성한 후 SIGINT 시그널을 처리하는 핸들러를 등록하고 무한 대기.
[Input]           : dummy - 사용되지 않음
[Output]          : 없음
[Call By]         : main 함수에서 pthread_create를 통해 호출됨
[Calls]           : signal, pthread_self
[Given]           : THREAD_1이 정의되어 있어야 SIGINT 핸들러 등록
[Returns]         : 없음
==================================================================*/
void Thread1(void *dummy)
{
#ifdef  THREAD_1
        signal(SIGINT, SigIntHandler);  // SIGINT 핸들러 등록
#endif

        while (1)
                ;  // 무한 대기
}

/*===============================================================
[Function Name]   : Thread2
[Description]     : 스레드 2를 생성한 후 SIGINT 시그널을 처리하는 핸들러를 등록하고 무한 대기.
[Input]           : dummy - 사용되지 않음
[Output]          : 없음
[Call By]         : main 함수에서 pthread_create를 통해 호출됨
[Calls]           : signal, pthread_self
[Given]           : THREAD_2가 정의되어 있어야 SIGINT 핸들러 등록
[Returns]         : 없음
==================================================================*/
void Thread2(void *dummy)
{
#ifdef  THREAD_2
        signal(SIGINT, SigIntHandler);  // SIGINT 핸들러 등록
#endif

        while (1)
                ;  // 무한 대기
}

/*===============================================================
[Program Name]    : sig_thread.c
[Description]     : 두 개의 스레드(Thread1, Thread2)를 생성하고, 각각에 대해 SIGINT 시그널을 처리하는 핸들러를 등록.
                    메인 스레드에도 SIGINT 핸들러를 등록하고, ^C를 통해 프로그램 종료.
[Input]           : 없음
[Output]          : 각 스레드가 수신한 SIGINT 시그널에 대해 "Received a SIGINT signal by thread" 메시지 출력, 
                    메인 스레드가 SIGINT를 수신하면 "Terminate this process" 메시지 출력
[Calls]           : SigIntHandler - 시그널을 처리하는 핸들러
[특기사항]        : 
    - 프로그램은 `pthread_create`로 두 개의 스레드를 생성하고, 각 스레드에서 `SIGINT` 시그널을 처리합니다.
    - `THREAD_MAIN`, `THREAD_1`, `THREAD_2` 매크로 정의에 따라 메인 스레드 및 각 스레드에서 `SIGINT`를 처리할지 여부가 결정됩니다.
    - ^C를 눌러 시그널을 발생시키면 해당 스레드가 수신한 시그널을 처리합니다.
==================================================================*/

int main()
{
        pthread_t       tid1, tid2;

        /* 스레드 1 생성 */
        if (pthread_create(&tid1, NULL, (void *)Thread1, NULL) < 0)  {
                perror("pthread_create");
                exit(1);
        }

        /* 스레드 2 생성 */
        if (pthread_create(&tid2, NULL, (void *)Thread2, NULL) < 0)  {
                perror("pthread_create");
                exit(1);
        }

        printf("Create two threads: tid1=%ld, tid2=%ld\n", tid1, tid2);
        printf("Main thread: tid=%ld\n", pthread_self());

#ifdef  THREAD_MAIN
        /* SIGINT 핸들러 등록 (메인 스레드) */
        signal(SIGINT, SigIntHandler);
#endif

        printf("Press ^C to quit\n");

        for ( ; ; )
                pause();  // 무한 대기 (시그널 수신 대기)
}

