/*===============================================================
[Program Name] : dining2.c
[Description]  : 
    - 철학자의 식사 문제를 해결하기 위해 세마포어와 상태를 이용한 동기화 구현.
    - 철학자들이 포크를 들고 놓는 과정을 통해 충돌을 방지하며, 공정한 자원 분배를 보장.
[Input]        :
    세마포어 (Philosopher[NUM_MEN], Mutex)
    pthread_t tid[NUM_MEN];  // 철학자 스레드 식별자
    int id[NUM_MEN];         // 철학자 ID
[Output]       :
    철학자의 생각 및 식사 상태를 출력.
    모든 철학자가 완료 후 종료 메시지를 출력.
[Calls]        :
    pthread_create(), pthread_join(), sem_init(), sem_destroy(), 
    sem_wait(), sem_post(), ThreadUsleep()
[특기사항]     : 
    - 철학자는 왼쪽과 오른쪽 철학자의 상태를 확인한 후 포크를 사용 가능.
    - 상태 배열(State)을 통해 철학자의 현재 상태를 추적.
    - 랜덤 대기 시간을 통해 철학자의 활동을 분산.
==================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_MEN 5
#define NLOOPS 5

#define LEFT(i) ((i + NUM_MEN - 1) % NUM_MEN)
#define RIGHT(i) ((i + 1) % NUM_MEN)

enum { THINKING, EATING, HUNGRY };

sem_t Philosopher[NUM_MEN];
sem_t Mutex;
int State[NUM_MEN];

/*===============================================================
[Function Name] : void ThreadUsleep(int usecs)
[Description]   : 
    - 지정된 시간만큼 현재 스레드를 대기 상태로 유지.
[Input]         :
    int usecs;    // 대기 시간 (마이크로초 단위)
[Output]        :
    Nothing
[Call By]       :
    Thinking(int id), Eating(int id)
[Calls]         :
    pthread_cond_init(), pthread_mutex_init(), 
    pthread_cond_timedwait(), pthread_cond_destroy(), 
    pthread_mutex_destroy()
[Given]         :
    usecs 값은 0보다 커야 하며, 대기 시간 범위를 초과하지 않음.
[Returns]       :
    Nothing
==================================================================*/
void ThreadUsleep(int usecs)
{
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    struct timespec ts;
    struct timeval tv;

    if (pthread_cond_init(&cond, NULL) < 0) {
        perror("pthread_cond_init");
        pthread_exit(NULL);
    }
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("pthread_mutex_init");
        pthread_exit(NULL);
    }

    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + usecs / 1000000;
    ts.tv_nsec = (tv.tv_usec + (usecs % 1000000)) * 1000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }

    if (pthread_mutex_lock(&mutex) < 0) {
        perror("pthread_mutex_lock");
        pthread_exit(NULL);
    }
    if (pthread_cond_timedwait(&cond, &mutex, &ts) < 0) {
        perror("pthread_cond_timedwait");
        pthread_exit(NULL);
    }

    if (pthread_cond_destroy(&cond) < 0) {
        perror("pthread_cond_destroy");
        pthread_exit(NULL);
    }
    if (pthread_mutex_destroy(&mutex) < 0) {
        perror("pthread_mutex_destroy");
        pthread_exit(NULL);
    }
}

/*===============================================================
[Function Name] : void Thinking(int id)
[Description]   : 
    - 철학자가 생각하는 동작을 수행하며 대기 상태를 모사.
[Input]         :
    int id;       // 철학자 ID
[Output]        :
    철학자가 생각 및 식사를 원하는 메시지를 출력.
[Call By]       :
    DiningPhilosopher(int *pId)
[Calls]         :
    ThreadUsleep()
[Given]         :
    철학자의 ID는 유효해야 함.
[Returns]       :
    Nothing
==================================================================*/
void Thinking(int id)
{
    printf("Philosopher%d: Thinking.....\n", id);
    ThreadUsleep((rand() % 200) * 10000);
    printf("Philosopher%d: Want to eat.....\n", id);
}

/*===============================================================
[Function Name] : void Eating(int id)
[Description]   : 
    - 철학자가 식사하는 동작을 수행하며 대기 상태를 모사.
[Input]         :
    int id;       // 철학자 ID
[Output]        :
    철학자가 식사 중인 메시지를 출력.
[Call By]       :
    DiningPhilosopher(int *pId)
[Calls]         :
    ThreadUsleep()
[Given]         :
    철학자의 ID는 유효해야 함.
[Returns]       :
    Nothing
==================================================================*/
void Eating(int id)
{
    printf("Philosopher%d: Eating.....\n", id);
    ThreadUsleep((rand() % 100) * 10000);
}

/*===============================================================
[Function Name] : void Test(int id)
[Description]   : 
    - 철학자의 상태를 검사하고, 양쪽 철학자가 먹고 있지 않을 경우 식사 상태로 전환.
[Input]         :
    int id;       // 검사할 철학자 ID
[Output]        :
    철학자가 식사 가능한 경우 세마포어 신호 전달.
[Call By]       :
    PickUp(int id), PutDown(int id)
[Calls]         :
    sem_post()
[Given]         :
    철학자의 상태(State 배열)가 초기화되어야 함.
[Returns]       :
    Nothing
==================================================================*/
void Test(int id)
{
    if (State[id] == HUNGRY && State[LEFT(id)] != EATING &&
        State[RIGHT(id)] != EATING) {
        State[id] = EATING;

        if (sem_post(&Philosopher[id]) < 0) {
            perror("sem_post");
            pthread_exit(NULL);
        }
    }
}

/*===============================================================
[Function Name] : void PickUp(int id)
[Description]   : 
    - 철학자가 포크를 들고 식사 상태로 전환하기 위한 절차를 수행.
[Input]         :
    int id;       // 포크를 들 철학자 ID
[Output]        :
    Nothing
[Call By]       :
    DiningPhilosopher(int *pId)
[Calls]         :
    sem_wait(), sem_post(), Test(int id)
[Given]         :
    세마포어와 상태(State 배열)가 초기화되어야 함.
[Returns]       :
    Nothing
==================================================================*/
void PickUp(int id)
{
    if (sem_wait(&Mutex) < 0) {
        perror("sem_wait");
        pthread_exit(NULL);
    }

    State[id] = HUNGRY;
    Test(id);

    if (sem_post(&Mutex) < 0) {
        perror("sem_post");
        pthread_exit(NULL);
    }

    if (sem_wait(&Philosopher[id]) < 0) {
        perror("sem_wait");
        pthread_exit(NULL);
    }
}

/*===============================================================
[Function Name] : void PutDown(int id)
[Description]   : 
    - 철학자가 포크를 내려놓고 양쪽 철학자의 상태를 검사.
[Input]         :
    int id;       // 포크를 내려놓을 철학자 ID
[Output]        :
    Nothing
[Call By]       :
    DiningPhilosopher(int *pId)
[Calls]         :
    sem_wait(), sem_post(), Test(int id)
[Given]         :
    세마포어와 상태(State 배열)가 초기화되어야 함.
[Returns]       :
    Nothing
==================================================================*/
void PutDown(int id)
{
    if (sem_wait(&Mutex) < 0) {
        perror("sem_wait");
        pthread_exit(NULL);
    }

    State[id] = THINKING;

    Test(LEFT(id));
    Test(RIGHT(id));

    if (sem_post(&Mutex) < 0) {
        perror("sem_post");
        pthread_exit(NULL);
    }
}

/*===============================================================
[Function Name] : void DiningPhilosopher(int *pId)
[Description]   : 
    - 철학자의 생각, 포크 들기, 식사, 포크 내려놓기 과정을 반복.
[Input]         :
    int *pId;     // 철학자 ID를 가리키는 포인터
[Output]        :
    철학자의 생각 및 식사 상태 메시지 출력.
[Call By]       :
    Main()
[Calls]         :
    Thinking(int id), PickUp(int id), Eating(int id), PutDown(int id)
[Given]         :
    철학자의 ID는 유효해야 함.
[Returns]       :
    Nothing
==================================================================*/
void DiningPhilosopher(int *pId)
{
    int i;
    int id = *pId;

    for (i = 0; i < NLOOPS; i++) {
        Thinking(id);
        PickUp(id);
        Eating(id);
        PutDown(id);
    }

    printf("Philosopher%d: thinking & eating %d times.....\n", id, i);

    pthread_exit(NULL);
}

int main()
{
    pthread_t tid[NUM_MEN];
    int i, id[NUM_MEN];

    srand(0x8888);

    for (i = 0; i < NUM_MEN; i++) {
        if (sem_init(&Philosopher[i], 0, 0) < 0) {
            perror("sem_init");
            exit(1);
        }
        id[i] = i;
    }
    if (sem_init(&Mutex, 0, 1) < 0) {
        perror("sem_init");
        exit(1);
    }

    for (i = 0; i < NUM_MEN; i++) {
        if (pthread_create(&tid[i], NULL, (void *)DiningPhilosopher,
                           (void *)&id[i]) < 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    for (i = 0; i < NUM_MEN; i++) {
        if (pthread_join(tid[i], NULL) < 0) {
            perror("pthread_join");
            exit(1);
        }
    }

    for (i = 0; i < NUM_MEN; i++) {
        if (sem_destroy(&Philosopher[i]) < 0) {
            perror("sem_destroy");
        }
    }

    if (sem_destroy(&Mutex) < 0) {
        perror("sem_destroy");
        exit(1);
    }
}

