/*===============================================================
[Program Name] : semlib.c
[Description]  : 
    - 세마포어를 생성, 초기화, 조작 및 제거하는 유틸리티 함수 제공.
    - 세마포어 기반 동기화를 위한 필수 함수 구현.
[Input]        :
    key_t key;        // 세마포어를 식별하기 위한 키
    int semid;        // 세마포어 식별자
    int value;        // 초기화 값 또는 설정 값
[Output]       :
    함수 성공 시 0 또는 세마포어 식별자 반환, 실패 시 -1 반환.
[Calls]        :
    semget(), semctl(), semop(), perror()
[특기사항]     : 
    - 세마포어는 프로세스 간 동기화를 보장하며, 생성 및 사용 시 적절한 키가 필요.
==================================================================*/

#include <stdio.h>
#include "semlib.h"

/*===============================================================
[Function Name] : int semInit(key_t key)
[Description]   : 
    - 주어진 키로 세마포어를 생성.
    - IPC_CREAT 플래그를 사용하여 세마포어가 없으면 생성.
[Input]         :
    key_t key;    // 세마포어를 식별하기 위한 키
[Output]        :
    세마포어 식별자 반환 (성공 시), -1 반환 (실패 시)
[Calls]         :
    semget(), perror()
[Given]         :
    유효한 키 값이 필요.
[Returns]       :
    int; 세마포어 식별자
==================================================================*/
int semInit(key_t key)
{
    int semid;

    if ((semid = semget(key, 1, 0600 | IPC_CREAT)) < 0) {
        perror("semget");
        return -1;
    }

    return semid;
}

/*===============================================================
[Function Name] : int semInitValue(int semid, int value)
[Description]   : 
    - 세마포어의 초기 값을 설정.
[Input]         :
    int semid;    // 세마포어 식별자
    int value;    // 초기화 값
[Output]        :
    세마포어 식별자 반환 (성공 시), -1 반환 (실패 시)
[Calls]         :
    semctl(), perror()
[Given]         :
    세마포어가 초기화되어 있어야 함.
[Returns]       :
    int; 세마포어 식별자
==================================================================*/
int semInitValue(int semid, int value)
{
    union semun {
        int val;
    } semun;

    semun.val = value;
    if (semctl(semid, 0, SETVAL, semun) < 0) {
        perror("semctl");
        return -1;
    }

    return semid;
}

/*===============================================================
[Function Name] : int semWait(int semid)
[Description]   : 
    - 세마포어 값을 감소시켜 자원을 요청.
    - 자원이 사용 가능할 때까지 대기.
[Input]         :
    int semid;    // 세마포어 식별자
[Output]        :
    0 반환 (성공 시), -1 반환 (실패 시)
[Calls]         :
    semop(), perror()
[Given]         :
    세마포어가 초기화되어 있어야 함.
[Returns]       :
    int; 상태 코드
==================================================================*/
int semWait(int semid)
{
    struct sembuf semcmd;

    semcmd.sem_num = 0;
    semcmd.sem_op = -1;
    semcmd.sem_flg = SEM_UNDO;
    if (semop(semid, &semcmd, 1) < 0) {
        perror("semop");
        return -1;
    }

    return 0;
}

/*===============================================================
[Function Name] : int semTryWait(int semid)
[Description]   : 
    - 세마포어 값을 감소시켜 자원을 요청.
    - 자원이 사용 가능하지 않을 경우 즉시 반환.
[Input]         :
    int semid;    // 세마포어 식별자
[Output]        :
    0 반환 (성공 시), -1 반환 (실패 시)
[Calls]         :
    semop(), perror()
[Given]         :
    세마포어가 초기화되어 있어야 함.
[Returns]       :
    int; 상태 코드
==================================================================*/
int semTryWait(int semid)
{
    struct sembuf semcmd;

    semcmd.sem_num = 0;
    semcmd.sem_op = -1;
    semcmd.sem_flg = IPC_NOWAIT | SEM_UNDO;
    if (semop(semid, &semcmd, 1) < 0) {
        perror("semop");
        return -1;
    }

    return 0;
}

/*===============================================================
[Function Name] : int semPost(int semid)
[Description]   : 
    - 세마포어 값을 증가시켜 자원을 반환.
[Input]         :
    int semid;    // 세마포어 식별자
[Output]        :
    0 반환 (성공 시), -1 반환 (실패 시)
[Calls]         :
    semop(), perror()
[Given]         :
    세마포어가 초기화되어 있어야 함.
[Returns]       :
    int; 상태 코드
==================================================================*/
int semPost(int semid)
{
    struct sembuf semcmd;

    semcmd.sem_num = 0;
    semcmd.sem_op = 1;
    semcmd.sem_flg = SEM_UNDO;
    if (semop(semid, &semcmd, 1) < 0) {
        perror("semop");
        return -1;
    }

    return 0;
}

/*===============================================================
[Function Name] : int semGetValue(int semid)
[Description]   : 
    - 현재 세마포어 값을 반환.
[Input]         :
    int semid;    // 세마포어 식별자
[Output]        :
    세마포어 값 반환 (성공 시), -1 반환 (실패 시)
[Calls]         :
    semctl(), perror()
[Given]         :
    세마포어가 초기화되어 있어야 함.
[Returns]       :
    int; 세마포어 값
==================================================================*/
int semGetValue(int semid)
{
    union semun {
        int val;
    } dummy;

    return semctl(semid, 0, GETVAL, dummy);
}

/*===============================================================
[Function Name] : int semDestroy(int semid)
[Description]   : 
    - 세마포어를 제거.
[Input]         :
    int semid;    // 세마포어 식별자
[Output]        :
    0 반환 (성공 시), -1 반환 (실패 시)
[Calls]         :
    semctl(), perror()
[Given]         :
    세마포어가 초기화되어 있어야 함.
[Returns]       :
    int; 상태 코드
==================================================================*/
int semDestroy(int semid)
{
    union semun {
        int val;
    } dummy;

    if (semctl(semid, 0, IPC_RMID, dummy) < 0) {
        perror("semctl");
        return -1;
    }

    close(semid);

    return 0;
}

