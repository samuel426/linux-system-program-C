/*===============================================================
[Program Name] : semlib2.c
[Description]  : 
    - POSIX 세마포어를 사용자 정의 구조체를 이용하여 구현.
    - 세마포어 초기화, 대기, 시도, 해제, 값 조회, 삭제 기능 제공.
[Input]        :
    sem_t *sem;   // 사용자 정의 세마포어 구조체
    int pshared;  // 세마포어 공유 여부 (현재 지원하지 않음)
    int value;    // 초기화 값
    int *sval;    // 세마포어 현재 값 저장 포인터
[Output]       :
    성공 시 0 반환, 실패 시 -1 반환.
[Calls]        :
    pthread_mutex_init(), pthread_cond_init(), pthread_mutex_lock(), 
    pthread_mutex_unlock(), pthread_cond_wait(), pthread_cond_signal(), 
    pthread_mutex_destroy(), pthread_cond_destroy()
[특기사항]     : 
    - 스레드 간 동기화를 위해 사용자 정의 세마포어 구조체 사용.
    - 프로세스 간 공유는 지원하지 않음.
==================================================================*/

#include <stdio.h>
#include <pthread.h>
#include "semlib2.h"

/*===============================================================
[Function Name] : int sem_init(sem_t *sem, int pshared, int value)
[Description]   : 
    - 사용자 정의 세마포어를 초기화.
    - 조건 변수와 뮤텍스를 초기화하고 세마포어 값을 설정.
[Input]         :
    sem_t *sem;   // 세마포어 구조체 포인터
    int pshared;  // 프로세스 간 공유 여부 (지원하지 않음)
    int value;    // 초기 세마포어 값
[Output]        :
    성공 시 0 반환, 실패 시 -1 반환.
[Calls]         :
    pthread_mutex_init(), pthread_cond_init()
[Given]         :
    sem 포인터는 NULL이 아니어야 함.
[Returns]       :
    int; 상태 코드
==================================================================*/
int sem_init(sem_t *sem, int pshared, int value)
{
    if (pshared) {
        fprintf(stderr, "Function not supported\n");
        return -1;
    }

    if (pthread_mutex_init(&sem->mutex, NULL) < 0)
        return -1;

    if (pthread_cond_init(&sem->cond, NULL) < 0)
        return -1;

    sem->sval = value;

    return 0;
}

/*===============================================================
[Function Name] : int sem_wait(sem_t *sem)
[Description]   : 
    - 세마포어 값을 감소시켜 자원을 요청.
    - 값이 0이면 자원이 사용 가능할 때까지 대기.
[Input]         :
    sem_t *sem;   // 세마포어 구조체 포인터
[Output]        :
    성공 시 0 반환, 실패 시 -1 반환.
[Calls]         :
    pthread_mutex_lock(), pthread_cond_wait(), pthread_mutex_unlock()
[Given]         :
    세마포어는 초기화되어 있어야 함.
[Returns]       :
    int; 상태 코드
==================================================================*/
int sem_wait(sem_t *sem)
{
    if (pthread_mutex_lock(&sem->mutex) < 0)
        return -1;

    if (sem->sval == 0) {
        while (sem->sval == 0) {
            if (pthread_cond_wait(&sem->cond, &sem->mutex) < 0) {
                pthread_mutex_unlock(&sem->mutex);
                return -1;
            }
        }
    }
    sem->sval--;

    if (pthread_mutex_unlock(&sem->mutex) < 0)
        return -1;

    return 0;
}

/*===============================================================
[Function Name] : int sem_trywait(sem_t *sem)
[Description]   : 
    - 세마포어 값을 감소시켜 자원을 요청.
    - 값이 0이면 즉시 반환.
[Input]         :
    sem_t *sem;   // 세마포어 구조체 포인터
[Output]        :
    성공 시 0 반환, 실패 시 -1 반환.
[Calls]         :
    pthread_mutex_lock(), pthread_mutex_unlock()
[Given]         :
    세마포어는 초기화되어 있어야 함.
[Returns]       :
    int; 상태 코드
==================================================================*/
int sem_trywait(sem_t *sem)
{
    if (pthread_mutex_lock(&sem->mutex) < 0)
        return -1;

    if (sem->sval == 0) {
        pthread_mutex_unlock(&sem->mutex);
        return -1;
    }

    sem->sval--;

    if (pthread_mutex_unlock(&sem->mutex) < 0)
        return -1;

    return 0;
}

/*===============================================================
[Function Name] : int sem_post(sem_t *sem)
[Description]   : 
    - 세마포어 값을 증가시켜 자원을 반환.
    - 대기 중인 스레드가 있으면 신호를 보냄.
[Input]         :
    sem_t *sem;   // 세마포어 구조체 포인터
[Output]        :
    성공 시 0 반환, 실패 시 -1 반환.
[Calls]         :
    pthread_mutex_lock(), pthread_cond_signal(), pthread_mutex_unlock()
[Given]         :
    세마포어는 초기화되어 있어야 함.
[Returns]       :
    int; 상태 코드
==================================================================*/
int sem_post(sem_t *sem)
{
    if (pthread_mutex_lock(&sem->mutex) < 0)
        return -1;

    if (sem->sval == 0) {
        if (pthread_cond_signal(&sem->cond) < 0) {
            pthread_mutex_unlock(&sem->mutex);
            return -1;
        }
    }

    sem->sval++;

    if (pthread_mutex_unlock(&sem->mutex) < 0)
        return -1;

    return 0;
}

/*===============================================================
[Function Name] : int sem_getvalue(sem_t *sem, int *sval)
[Description]   : 
    - 세마포어의 현재 값을 반환.
[Input]         :
    sem_t *sem;   // 세마포어 구조체 포인터
    int *sval;    // 값을 저장할 변수의 포인터
[Output]        :
    성공 시 0 반환, 실패 시 -1 반환.
[Calls]         :
    Nothing
[Given]         :
    세마포어는 초기화되어 있어야 함.
[Returns]       :
    int; 상태 코드
==================================================================*/
int sem_getvalue(sem_t *sem, int *sval)
{
    *sval = sem->sval;
    return 0;
}

/*===============================================================
[Function Name] : int sem_destroy(sem_t *sem)
[Description]   : 
    - 세마포어를 제거하고 리소스를 해제.
[Input]         :
    sem_t *sem;   // 세마포어 구조체 포인터
[Output]        :
    성공 시 0 반환, 실패 시 -1 반환.
[Calls]         :
    pthread_mutex_destroy(), pthread_cond_destroy()
[Given]         :
    세마포어는 초기화되어 있어야 함.
[Returns]       :
    int; 상태 코드
==================================================================*/
int sem_destroy(sem_t *sem)
{
    if (pthread_mutex_destroy(&sem->mutex) < 0)
        return -1;

    if (pthread_cond_destroy(&sem->cond) < 0)
        return -1;

    return 0;
}

